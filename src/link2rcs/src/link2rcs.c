#ifndef	lint
static	char	Id[] = "$Id: link2rcs.c,v 5.5 1989/11/30 12:00:36 dickey Exp $";
#endif	lint

/*
 * Title:	link2rcs.c (link/directory tree)
 * Author:	T.E.Dickey
 * Created:	29 Nov 1989
 *
 * Function:	Scan the given list of source-directories, building a matching
 *		tree under the destination directory, with symbolic links to
 *		the RCS-directories.
 *
 * Options:	(see usage)
 */

#define	STR_PTYPES
#include	"ptypes.h"
#include	"rcsdefs.h"
#include	<ctype.h>
extern	char	*pathcat();
extern	char	*pathleaf();
extern	char	*rcs_dir();
extern	char	*relpath();
extern	char	*txtalloc();

extern	int	optind;		/* 'getopt()' index to argv */
extern	char	*optarg;	/* 'getopt()' argument in argv */

/************************************************************************
 *	local definitions						*
 ************************************************************************/

#define	WARN	FPRINTF(stderr,
#define	TELL	if(verbose >= 0) PRINTF(
#define	VERBOSE	if(verbose >  0) PRINTF(

typedef	struct	_list	{
	struct	_list	*link;	/* link to next item in list */
	char	*path;		/* path to define */
	char	*from;		/* link to define */
	} LIST;

	/*ARGSUSED*/
	def_ALLOC(LIST)

static	LIST	*list;
static	int	allnames;		/* "-a" option */
static	int	merge;			/* "-m" option */
static	int	no_op;			/* "-n" option */
static	int	relative;		/* "-r" option */
static	int	verbose;		/* "-v" option */

static	char	Source[BUFSIZ] = ".";
static	char	Target[BUFSIZ] = ".";
static	char	Current[BUFSIZ];

/*
 * This procedure is invoked from 'walktree()' for each file/directory which
 * is found in the specified tree.  Note that 'walktree()' walks the tree in
 * sorted-order.
 */
/*ARGSUSED*/
static
src_stat(path, name, sp, readable, level)
char	*path;
char	*name;
struct	stat	*sp;
{
	auto	LIST	*p, *q;

	if (sp == 0)
		;
	else if ((sp->st_mode & S_IFMT) == S_IFDIR) {
		auto	char	tmp[BUFSIZ],
				dst[BUFSIZ],
				*s = pathcat(tmp, path, name),
				*t;
		abspath(s);		/* get rid of "." and ".." names */
		t = pathleaf(s);	/* obtain leaf-name for "-a" option */
		if (*t == '.' && !allnames)	return (-1);
		/* patch: test for link-to-link */
		p = ALLOC(LIST,1);
		if (q = list) {
			while (q->link)
				q = q->link;
			q->link = p;
		} else
			list = p;
		p->link = 0;
		p->path = txtalloc(relpath(dst, Source, tmp) + 2);
		p->from = sameleaf(s, rcs_dir()) ? txtalloc(tmp) : 0;
	}
	return(readable);
}

/*
 * Process a single argument to obtain the list of source-directory names
 */
static
find_src(name)
char	*name;
{
	auto	char	tmp[BUFSIZ];
	(void)getwd(tmp);
	name = pathcat(tmp, tmp, name);
	TELL "** src-path = %s\n", name);
	(void)walktree((char *)0, name, src_stat, "r", 0);
}

static
exists(name)
char	*name;
{
	FPRINTF(stderr, "?? %-16s%s\n", "exists:", name);
	(void)fflush(stderr);
	exit(FAIL);
}

/*
 * Check for conflicts with existing directory or link-names.  We can replace
 * a link with a link, but directories should not be modified!
 */
static
conflict(path,mode)
char	*path;
{
	auto	struct	stat	sb;

	if (lstat(path, &sb) >= 0) {
		if (!merge)
			exists(path);
		if ((sb.st_mode & S_IFMT) == mode) {	/* compatible! */
			if (mode == S_IFLNK) {
				VERBOSE "%% rm -f %s\n", path);
				if (!no_op) {
					if (unlink(path) < 0)
						failed(path);
				}
			} else {
				TELL "** merged:         %s\n", path);
				return (TRUE);
			}
			return (FALSE);
		} else {
			exists(path);
		}
		TELL "** merged:         %s\n", path);
		return (TRUE);
	}
	return (FALSE);
}

/*
 * Create a single directory
 */
static
make_dir(path)
char	*path;
{
	if (!conflict(path, S_IFDIR)) {
		VERBOSE "%% mkdir %s\n", path);
		if (!no_op) {
			if (mkdir(path, 0777) < 0)
				failed(path);
		}
	}
}

/*
 * Create a symbolic link
 */
static
make_lnk(src, dst)
char	*src, *dst;
{
	if (!conflict(dst, S_IFLNK)) {
		VERBOSE "%% ln -s %s %s\n", src, dst);
		if (!no_op) {
			if (symlink(src, dst) < 0)
				failed(src);
		}
	}
}

/*
 * Process the list of source/target pairs to construct the required directories
 * and links.
 */
static
make_dst(path)
char	*path;
{
	auto	LIST	*p;
	auto	char	dst[BUFSIZ];
	auto	char	tmp[BUFSIZ];

	TELL "** dst-path = %s\n", path);
	(void)strcpy(dst, path);
	for (p = list; p; p = p->link) {
		if (p->from == 0) {	/* directory */
			TELL "** make directory: %s\n", p->path);
			make_dir(pathcat(dst, path, p->path));
		} else {		/* symbolic-link */
			TELL "** link-to-RCS:    %s\n", p->path);
			make_lnk(relative ? relpath(tmp, dst, p->from)
					  : p->from,
				p->path);
		}
	}
}

/*
 * Validate a directory-name argument, and save an absolute-path so we can
 * chdir-there.
 */
static
char	*
getdir(buffer)
char	*buffer;
{
	if (chdir(optarg) < 0)
		failed(optarg);
	abspath(strcpy(buffer, optarg));
	(void)chdir(Current);
	return (optarg);
}

/*
 * Display the options recognized by this utility
 */
static
usage()
{
	static	char	*msg[] = {
	 "Usage: link2rcs [options] [directory [...]]"
	,""
	,"Constructs a template directory tree with symbolic links to the src's"
	,"RCS-directories."
	,""
	,"Options:"
	,"  -a      process all directory names (else \".\"-names ignored)"
	,"  -d dir  specify destination-directory (distinct from -s, default .)"
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

main(argc, argv)
char	*argv[];
{
	auto	char	*src_dir = Source,
			*dst_dir = Target;
	register int j;

	(void)getwd(Current);
	while ((j = getopt(argc, argv, "ad:mnrqs:v")) != EOF)
		switch (j) {
		case 'a':	allnames++;			break;
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
	 * Process the list of source-directory specifications:
	 */
	if (chdir(src_dir) < 0)
		failed(src_dir);
	(void)getwd(Source);
	if (optind < argc) {
		for (j = optind; j < argc; j++)
			find_src(argv[j]);
	} else
		find_src(".");

	/*
	 * Construct the list of destination-directory names:
	 */
	(void)chdir(Target);
	make_dst(Target);

	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
