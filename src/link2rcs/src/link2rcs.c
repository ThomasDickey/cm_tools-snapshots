#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/cm_tools.vcs/src/link2rcs/src/RCS/link2rcs.c,v 11.2 1993/09/22 15:26:26 dickey Exp $";
#endif

/*
 * Title:	link2rcs.c (link/directory tree)
 * Author:	T.E.Dickey
 * Created:	29 Nov 1989
 * Modified:
 *		22 Sep 1993, gcc warnings.  Purify found an alloc-too-small.
 *		03 Sep 1993, qsort-definitions.
 *		15 Oct 1991, converted to ANSI
 *		04 Jun 1991, check for the special case in which the
 *			     environment-variable is the current path.
 *		20 May 1991, mods to compile on apollo sr10.3
 *		21 Aug 1990, corrected code in 'path_to()', which was not
 *			     properly testing for leading "./" in its argument.
 *			     Pass the value of the "-s" option down to
 *			     'find_src()' so that in case the user combines this
 *			     with a wildcard on the actual source-paths, we can
 *			     prune off the beginning portion of the source-path
 *			     arguments (fixes a problem using 'relpath()').
 *		04 May 1990, sort/purge list of items to remove repeats.  Added
 *			     "-b" option (currently only "-b0" value) to purge
 *			     the tree when no RCS is in a sub-directory.
 *		03 May 1990, added "-f" option to support linkages to ordinary
 *			     files.  Added "-e" option (apollo-only) to create
 *			     links with a given environment variable.
 *		27 Apr 1990, use 'chmod()' to ensure path-mode to cover up
 *			     apollo sr10/sr9 bug.
 *		14 Mar 1990, corrected reference path for 'relpath()'
 *			     computation.  Force mkdir-mode to 755.
 *		07 Dec 1989, lint (SunOs 3.4)
 *		06 Dec 1989, corrected 'getdir()' procedure (did 'abspath()' in
 *			     wrong place), and fixed a couple of places where a
 *			     "." leaf could mess up handling of regression-
 *			     tests.
 *		01 Dec 1989, Don't remake links if they have the same link-text.
 *			     Make the merge-listing less verbose than the
 *			     script.  Prevent 'walktree()' from scanning
 *			     directories to which we will make a link
 *
 * Function:	Scan the given list of source-directories, building a matching
 *		tree under the destination directory, with symbolic links to
 *		the RCS-directories.
 *
 * Options:	(see usage)
 *
 * To do:	add options for the following
 *		-b	specify baseline version which must appear in tree
 *		-p	create working directories only for those which the
 *			user has permissions (link to src for the rest)
 *
 *		test src-RCS to see if it is a symbolic link (resolve link)
 *
 *		Add option like "-e", which causes a specified symbolic link
 *		(in a fixed position) to be used as the src-pointer.
 */

#define	QSORT_SRC LIST
#define	STR_PTYPES
#include	<ptypes.h>
#include	<rcsdefs.h>
#include	<td_qsort.h>
#include	<ctype.h>

/************************************************************************
 *	local definitions						*
 ************************************************************************/

#define	WARN	FPRINTF(stderr,
#define	TELL	if(verbose >= 0) PRINTF(
#define	VERBOSE	if(verbose >  0) PRINTF(

#define	is_PREFIX(dst,src,len)	(len < strlen(dst) && !strncmp(dst,src,len))

typedef	struct	_list	{
	struct	_list	*link;	/* link to next item in list */
	char	*path;		/* path to define */
	char	*from;		/* link to define */
	char	*what;		/* annotation for sym-links */
	} LIST;

	/*ARGSUSED*/
	def_ALLOC(LIST)

static	LIST	*list;
static	int	allnames;		/* "-a" option */
static	int	baseline = -1;		/* "-b" option */
static	int	files_too;		/* "-f" option */
static	int	merge;			/* "-m" option */
static	int	no_op;			/* "-n" option */
static	int	relative;		/* "-r" option */
static	int	verbose;		/* "-v" option */

#ifdef	apollo				/* "-e" option */
static	char	*env_path;		/* variable-name */
static	int	env_size;		/* corresponding path-size */
#endif

static	char	Source[BUFSIZ] = ".";
static	char	Target[BUFSIZ] = ".";
static	char	Current[BUFSIZ];

static	char	*fmt_link = "link-to-RCS:";
static	char	*fmt_file = "link-to-file";

/*
 * Print normal-trace using a common format
 */
static void
tell_it(
_ARX(char *,	tag)
_AR1(char *,	path)
	)
_DCL(char *,	tag)
_DCL(char *,	path)
{
	TELL "** %-16s%s\n", tag, path);
}

/*
 * Check for leaf-names suppressed if "-a" option is not given.
 */
static int
suppress_dots(
_AR1(char *,	src))
_DCL(char *,	src)
{
	register char *t;
	abspath(src);		/* get rid of "." and ".." names */
	t = pathleaf(src);	/* obtain leaf-name for "-a" option */
	return (*t == '.' && !allnames);
}

/*
 * Add to the end of the linked-list so we process subdirectories after their
 * parents.
 */
static
LIST	*
new_LIST(_AR0)
{
	register LIST	*p = ALLOC(LIST,1),
			*q = list;
	if (q != 0) {
		while (q->link)
			q = q->link;
		q->link = p;
	} else
		list = p;
	p->link = 0;
	return (p);
}

/*
 * Convert Source+src to destination-path, accounting for user-specified
 * relative path, etc.
 */
static
char	*
path_to(
_AR1(char *,	src))
_DCL(char *,	src)
{
	auto	char	dst[BUFSIZ],
			*d = relpath(dst,Source,src);
	if (!strcmp(d, "."))
		return ".";
	if (!strncmp(d, "./", 2))
		d += 2;
	return txtalloc(d);
}

static
char	*
path_from(
_AR1(char *,	src))
_DCL(char *,	src)
{
#ifdef	apollo
	auto	char	tmp[BUFSIZ];
	if (env_path != 0) {
		FORMAT(tmp, "$(%s)%s", env_path, src + env_size);
		src = tmp;
	}
#endif
	return (txtalloc(src));
}

/*
 * This procedure is invoked from 'walktree()' for each file/directory which
 * is found in the specified tree.  Note that 'walktree()' walks the tree in
 * sorted-order.
 */
/*ARGSUSED*/
static int
src_stat(
_ARX(char *,	path)
_ARX(char *,	name)
_ARX(struct stat*,sp)
_ARX(int,	readable)
_AR1(int,	level)
	)
_DCL(char *,	path)
_DCL(char *,	name)
_DCL(struct stat*,sp)
_DCL(int,	readable)
_DCL(int,	level)
{
	auto	LIST	*p;
	auto	char	tmp[BUFSIZ],
			*s = pathcat(tmp, path, name);

	if (sp == 0)
		;
	else if ((sp->st_mode & S_IFMT) == S_IFDIR) {
		if (suppress_dots(s))	return (-1);
		p = new_LIST();
		/* patch: test for link-to-link */
		p->path = path_to(tmp);
		p->from = sameleaf(s, rcs_dir()) ? path_from(tmp) : 0;
		p->what = fmt_link;
		if (p->from != 0)
			readable = -1;
	} else if (files_too && ((sp->st_mode & S_IFMT) == S_IFREG)) {
		if (suppress_dots(s))	return (-1);
		p = new_LIST();
		p->path = path_to(tmp);
		p->from = path_from(tmp);
		p->what = fmt_file;
	}
	return(readable);
}

/*
 * Process a single argument to obtain the list of source-directory names
 */
static void
find_src(
_ARX(char *,	path)
_AR1(char *,	name)
	)
_DCL(char *,	path)
_DCL(char *,	name)
{
	auto	char	tmp[BUFSIZ];
	auto	size_t	len = strlen(path);

	/* prune the source-path from the beginning of 'name[]' in case the
	 * user got it by a wildcard-expansion
	 */
	if (!strncmp(path, name, len)
	&&  name[len] == '/')
		name += (len + 1);

	/* process the 'name[]' argument
	 */
	(void)getwd(tmp);
	abspath(name = pathcat(tmp, tmp, name));
	TELL "** src-path = %s\n", path_from(name));
	(void)walktree((char *)0, name, src_stat, "r", 0);
}

static void
exists(
_AR1(char *,	name))
_DCL(char *,	name)
{
	FPRINTF(stderr, "?? %s: already exists\n", name);
	(void)fflush(stderr);
	exit(FAIL);
}

static int
tell_merged(
_AR1(char *,	path))
_DCL(char *,	path)
{
	tell_it("(no change)",path);
	return (TRUE);
}

static int
samelink(
_ARX(char *,	dst)
_AR1(char *,	src)
	)
_DCL(char *,	dst)
_DCL(char *,	src)
{
	auto	int	len;
	auto	char	bfr[BUFSIZ];
	len = readlink(dst, bfr, sizeof(bfr));
	if (len > 0) {
		bfr[len] = EOS;
		return (!strcmp(bfr, src));
	}
	return (FALSE);
}

/*
 * Check for conflicts with existing directory or link-names.  We can replace
 * a link with a link, but directories should not be modified!
 */
static int
conflict(
_ARX(char *,	path)
_ARX(int,	mode)
_AR1(char *,	from)
	)
_DCL(char *,	path)
_DCL(int,	mode)
_DCL(char *,	from)
{
	auto	struct	stat	sb;

	if (lstat(path, &sb) >= 0) {
		if (!merge)
			exists(path);
		if ((sb.st_mode & S_IFMT) == mode) {	/* compatible! */
			if (mode == S_IFLNK) {
				if (samelink(path,from))
					return (tell_merged(path));
				VERBOSE "%% rm -f %s\n", path);
				if (!no_op) {
					if (unlink(path) < 0)
						failed(path);
				}
			} else {
				return (tell_merged(path));
			}
			return (FALSE);
		} else {
			exists(path);
		}
		return (tell_merged(path));
	}
	return (FALSE);
}

/*
 * Create a single directory
 */
static void
make_dir(
_AR1(char *,	path))
_DCL(char *,	path)
{
	if (strcmp(path,".")
	&&  !conflict(path, S_IFDIR, ".")) {
		tell_it("make directory:", path);
		VERBOSE "%% mkdir %s\n", path);
		if (!no_op) {
			if (mkdir(path, 0755) < 0)
				failed(path);
			(void)chmod(path, 0755);
		}
	}
}

/*
 * Create a symbolic link
 */
static void
make_lnk(
_ARX(char *,	src)
_ARX(char *,	dst)
_AR1(char *,	what)
	)
_DCL(char *,	src)
_DCL(char *,	dst)
_DCL(char *,	what)
{
	if (!conflict(dst, S_IFLNK, src)) {
		tell_it(what, dst);
		VERBOSE "%% ln -s %s %s\n", src, dst);
		if (!no_op) {
			if (symlink(src, dst) < 0)
				failed(src);
		}
	}
}

/* filter slashes in pathnames to newlines so directories sort in proper order*/
static
char	*
deslash(
_ARX(char *,	dst)
_AR1(LIST *,	p)
	)
_DCL(char *,	dst)
_DCL(LIST *,	p)
{
	auto	 char	*base = dst;
	register char	*src = p->path;
	do {
		register char	c = *src++;
		*dst = (c == '/') ? '\n' : c;
	} while (*dst++);
	return (base);
}

static
QSORT_FUNC(compar_LIST)
{
	QSORT_CAST(q1,p1)
	QSORT_CAST(q2,p2)
	char	x[BUFSIZ], y[BUFSIZ];
	return (strcmp(deslash(x,p1), deslash(y,p2)));
}

/* compress duplicate items out of the LIST-vector, returns the resulting len */
static
int	unique_LIST(
	_ARX(LIST *,	vec)
	_AR1(unsigned,	count)
		)
	_DCL(LIST *,	vec)
	_DCL(unsigned,	count)
{
	register int	j, k;
	for (j = k = 0; k < count; j++, k++) {
		if (j != k)
			vec[j] = vec[k];
		while ((k < count) && (vec[k+1].path == vec[j].path))
			k++;
	}
	return (j);
}

/* given a directory-entry, find if a subordinate RCS-directory exists */
static int
has_children(
_ARX(LIST *,	vec)
_ARX(unsigned,	count)
_AR1(int,	old)
	)
_DCL(LIST *,	vec)
_DCL(unsigned,	count)
_DCL(int,	old)
{
	register int	new;
	auto	 size_t	len = strlen(vec[old].path);

	if (vec[old].what == fmt_file)		/* preserve files */
		return (TRUE);
	if (vec[old].from != 0)
		/* patch: check baseline-version here */
		return (TRUE);

	for (new = old+1; new < count; new++) {
		if (vec[new].what == fmt_file)	/* skip files */
			continue;
		if (vec[new].from == 0)		/* skip non-RCS entries */
			continue;
		if (is_PREFIX(vec[new].path,vec[old].path,len))
			/* patch: check baseline-version here */
			return (TRUE);
	}
	return (FALSE);
}

/* skip past the specified directory-entry and all of its children */
static int
skip_children(
_ARX(LIST *,	vec)
_ARX(unsigned,	count)
_AR1(int,	old)
	)
_DCL(LIST *,	vec)
_DCL(unsigned,	count)
_DCL(int,	old)
{
	register int	new;
	auto	 size_t	len = strlen(vec[old].path);

	for (new = old+1; new < count; new++) {
		if (!is_PREFIX(vec[new].path,vec[old].path,len))
			break;
	}
	return (new);
}

/* purge entries which do not have an underlying RCS-directory */
static int
purge_LIST(
_ARX(LIST *,	vec)
_AR1(unsigned,	count)
	)
_DCL(LIST *,	vec)
_DCL(unsigned,	count)
{
	register int	j, k;
	for (j = k = 0; k < count; j++, k++) {
		while (!has_children(vec,count,k))
			k = skip_children(vec,count,k);
		if (k < count)
			vec[j] = vec[k];
	}
	return (j);
}

/*
 * Process the list of source/target pairs to construct the required directories
 * and links.
 */
static void
make_dst(
_AR1(char *,	path))
_DCL(char *,	path)
{
	auto	LIST	*p, *q;
	auto	char	dst[BUFSIZ];
	auto	char	tmp[BUFSIZ];
	auto	unsigned count;
	register int	j;

	TELL "** dst-path = %s\n", path);

	/* sort/purge the linked list */
	for (p = list, count = 0; p; p = p->link)
		count++;
	if (count == 0)
		return;
	else if (count > 1) {
		static	LIST	dummy;
		auto	LIST	*vec = ALLOC(LIST,count+1);
		for (p = list, count = 0; p; p = q) {
			vec[count++] = *p;
			q = p->link;
			dofree((char *)p);
		}
		vec[count] = dummy;
		qsort((char *)vec, (LEN_QSORT)count, sizeof(LIST), compar_LIST);
		count = unique_LIST(vec,count);
		if (baseline >= 0)
			count = purge_LIST(vec,count);
		if (count == 0)
			return;
		list = &vec[0];
		for (j = 1; j < count; j++)
			vec[j-1].link = &vec[j];
		vec[count-1].link = 0;
	}

	/* process the linked-list */
	(void)strcpy(dst, path);
	for (p = list; p; p = p->link) {
		if (p->from == 0) {	/* directory */
			(void)pathcat(dst, path, p->path);
			make_dir(p->path);
		} else {		/* symbolic-link */
			register char *s;
			(void)pathcat(dst, path, p->path);
			if ((s = strrchr(dst, '/')) != NULL)
				*s = EOS;
			make_lnk(relative ? relpath(tmp, dst, p->from)
					  : p->from,
				p->path,
				p->what);
		}
	}
}

/*
 * Validate a directory-name argument, and save an absolute-path so we can
 * chdir-there.
 */
static
char	*
getdir(
_AR1(char *,	buffer))
_DCL(char *,	buffer)
{
	abspath(strcpy(buffer, optarg));
	if (chdir(optarg) < 0)
		failed(optarg);
	(void)chdir(Current);
	return (optarg);
}

/*
 * Display the options recognized by this utility
 */
static void
usage(_AR0)
{
	static	char	*msg[] = {
	 "Usage: link2rcs [options] [directory [...]]"
	,""
	,"Constructs a template directory tree with symbolic links to the src's"
	,"RCS-directories."
	,""
	,"Options:"
	,"  -a      process all directory names (else \".\"-names ignored)"
	,"  -b num  purge entries without the specified baseline version"
	,"          use \"-b0\" to purge directories which have no RCS beneath"
#ifdef	apollo
	,"  -e env  specify environment-variable to use in symbolic links"
#endif
	,"  -d dir  specify destination-directory (distinct from -s, default .)"
	,"  -f      link to files also"
	,"  -m      merge against destination"
	,"  -n      no-op"
	,"  -r      construct relative symbolic links"
	,"  -q      quiet undoes normal verbosness"
	,"  -s dir  specify source-directory (default .)"
	,"  -v      verbose (show detailed actions)"
	};
	register int	j;
	for (j = 0; j < sizeof(msg)/sizeof(msg[0]); j++)
		WARN "%s\n", msg[j]);
	(void)fflush(stderr);
	(void)exit(FAIL);
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/

/*ARGSUSED*/
_MAIN
{
	auto	char	*src_dir = Source,
			*dst_dir = Target;
	auto	char	*p;
	register int j;

	(void)getwd(Current);
	while ((j = getopt(argc, argv, "ab:d:e:fmnrqs:v")) != EOF)
		switch (j) {
		case 'a':	allnames++;			break;
		case 'b':	baseline = strtol(optarg, &p, 0);
				if (*p != EOS)
					usage();
				break;
#ifdef	apollo
		case 'e':	env_path = optarg;		break;
#endif
		case 'f':	files_too++;			break;
		case 'm':	merge++;			break;
		case 'n':	no_op++;			break;
		case 'r':	relative++;			break;
		case 'q':	verbose--;			break;
		case 'v':	verbose++;			break;
		case 's':	src_dir = getdir(Source);	break;
		case 'd':	dst_dir = getdir(Target);	break;
		default:
			usage();
			/*NOTREACHED*/
		}

	/*
	 * Verify that the source/target directories are distinct
	 */
	TELL "** SRC=%s\n", src_dir);
	TELL "** DST=%s\n", dst_dir);
	abspath(Source);
	abspath(Target);
	if (!strcmp(Source,Target))
		usage();

	/*
	 * If environment-variable is specified in src-path, verify that it is
	 * defined properly.
	 */
#ifdef	apollo
	if (env_path != 0) {
		extern	char	*getenv();
		auto	size_t	len;
		auto	char	tmp[BUFSIZ];

		if ((p = getenv(env_path)) == 0) {
			WARN "?? variable \"%s\" is not defined\n", env_path);
			exit(FAIL);
		}
		abspath(p = strcpy(tmp, p));
		len = strlen(p);
		if (strcmp(Source,p)
		 && !is_PREFIX(Source,p,len)) {
			WARN "?? value of \"%s\" is not a prefix of \"%s\"\n",
				env_path, Source);
			exit(FAIL);
		}
		env_size = len;
	}
#endif

	/*
	 * Process the list of source-directory specifications:
	 */
	if (chdir(src_dir) < 0)
		failed(src_dir);
	(void)getwd(Source);
	if (optind < argc) {
		for (j = optind; j < argc; j++)
			find_src(src_dir, argv[j]);
	} else
		find_src(src_dir, ".");

	/*
	 * Construct the list of destination-directory names:
	 */
	(void)chdir(Target);
	make_dst(Target);

	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
