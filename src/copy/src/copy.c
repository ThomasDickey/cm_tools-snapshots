#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/cm_tools.vcs/src/copy/src/RCS/copy.c,v 11.0 1992/02/17 15:21:03 ste_cm Rel $";
#endif

/*
 * Title:	copy.c (enhanced unix copy utility)
 * Author:	T.E.Dickey
 * Created:	16 Aug 1988
 * Modified:
 *		03 Feb 1992, ensure that if I am copying directory to name that
 *			     happens to be a file, I leave the directory mode
 *			     executable.
 *		11 Oct 1991, convert to ANSI
 *		31 May 1991, lint (SunOs): ifdef'd out 'convert()'
 *		20 May 1991, mods to compile on apollo sr10.3
 *		28 Jun 1990, corrected handling of (non-Apollo) user requests to
 *			     copy to a directory-path beginning with "~".
 *		14 May 1990, corrected logic which verifies args in 'copyit()'.
 *			     Also, added kludge so that copy via symbolic link
 *			     to directory.  Sort of works (if there are more
 *			     than 2 args) -- must fix.  Cleaned up last change
 *			     by permitting user to coerce the copy into a
 *			     destination-directory by supplying a trailing '/'.
 *		08 May 1990, corrected handling of tilde in src-path.  Added
 *			     "-l" option for copying link-targets.  Retain
 *			     tilde-expansion on both src/dst in 'copyit()'
 *		30 Apr 1990, use of 'convert()' broke sr9.7 compatibility
 *			     (fixed).  Corrected bug which assumed 'stat()'
 *			     modified arg even if err.
 *		25 Apr 1990, last change broke test for directory-not-writeable.
 *			     fixed.  Refined test-for-identical inodes
 *		24 Apr 1990, double-check test for repeated-names by verifying
 *			     that the source and destination are really on the
 *			     same device.  Use 'abspath()' on args to /bin/cp
 *			     for sr10.2, since the cp-program does not do "~"
 *			     processing itself.  Modified use of 'abspath()'
 *			     so we use it only for converting the tilde-stuff
 *			     (got into trouble with things like "RCS/.." when
 *			     RCS is itself a symbolic link).  Note that the
 *			     verbose trace is not quite right yet.
 *		24 Jan 1990, added missing 'break' in 'skip_dots()' loop
 *		03 Jan 1990, corrected handling of relative pathnames in source-
 *			     args.  This is fixed by not passing the "../"
 *			     constructs to the destination-name!  Also, modified
 *			     the test for writeable destination-directory so it
 *			     handles directory-names (such as "." or "~") which
 *			     have no "/".
 *		10 Oct 1989, added code for apollo SR10.1 (ifdef'd to permit SR9
 *			     version) which uses the new 'cp' utility to copy
 *			     files (and their types, as in SR9.7).
 *		06 Sep 1989, use access-defs in "ptypes.h"
 *		29 Aug 1989, corrected error-check after 'mkdir()' (if merging
 *			     directories).
 *		30 Mar 1989, modified normal unix file-copy code so that if the
 *			     destination file could not be opened for output,
 *			     then we restore its protection.
 *		27 Mar 1989, added logic to test for success of Aegis 'cpf'
 *			     (must rely on the file-statistics differing).
 *			     Also, use 'abspath()' on the argument passed to
 *			     'mkdir()' to make this tolerant of '.' in the
 *			     arguments.
 *		13 Mar 1989, added "-s" (set-uid) option.
 *		06 Mar 1989, added code to copy symbolic links
 *		24 Jan 1989, use 'pathleaf()' to correct pathnames of copy into
 *			     an existing directory.  Added copy-directory with
 *			     "-m" option.
 *		25 Aug 1988, added "-u" option for checkin/checkout setuid.
 *		23 Aug 1988, replaced call on 'name2s()' by call to apollo
 *			     naming-server to translate names (works better)
 *		19 Aug 1988, corrected 'name2s()' call to handle "/bin/[".
 *		18 Aug 1988, use 'name2s()' so AEGIS can handle our mixed-case
 *			     filenames.
 *		17 Aug 1988, use AEGIS 'cpf' to copy files on apollo.
 *
 * Function:	Provides a portable copy function on unix systems, with the
 *		following functions (which are provided by some, but not all
 *		unix 'cp' programs):
 *
 *		* preserves modification time of copied file
 *		* lists names as they are copied
 *
 * patch:	if running as root, should try also to keep owner/mode of the
 *		destination intact.
 *
 * patch:	this copies only files, directories and symbolic links; must
 *		handle other stuff such as devices.
 *
 * patch:	how do we keep from losing the destination file if we have a
 *		fault in the system?
 *
 * patch:	should have an option to preserve the sense of hard-links.
 */

#define		ACC_PTYPES	/* include access-definitions */
#define		DIR_PTYPES	/* include directory-definitions */
#define		STR_PTYPES	/* include string-definitions */
#include	<ptypes.h>
#include	<errno.h>

#define	TELL	FPRINTF(stderr,
#define	VERBOSE	if (v_opt) TELL
#define	DEBUG	if (v_opt > 1)	TELL

#ifndef	S_IFLNK
#define	lstat	stat
#endif	/* S_IFLNK */

static	long	total_dirs,
		total_links,
		total_files,
		total_bytes;
static	int	d_opt,		/* obtain source from destination arg */
		i_opt,		/* interactive: force prompt before overwrite */
		l_opt,		/* copy symbolic-link-targets */
		m_opt,		/* merge directories */
		n_opt,		/* true if we don't actually do copies */
		s_opt,		/* enable set-uid/gid in target files */
		v_opt;		/* verbose */
static	int	no_dir_yet;	/* disables access-test on destination-dir */

/************************************************************************
 *	local procedures						*
 ************************************************************************/

#ifdef	apollo
#ifndef apollo_sr10
#include	</sys/ins/base.ins.c>
#include	</sys/ins/name.ins.c>

/*
 * Use the Apollo naming-server to perform complete substitution on a pathname.
 * This is used (rather than 'name2s()'), because Apollo's 'cpf' does not work
 * properly when we have a symbolic link whose text is an absolute pathname
 * (i.e., //dickey/tmp).
 *
 * We still must use 'name2s()' to make a correct string for a leaf which does
 * not yet exist.
 */
static
char *
convert(
_ARX(char *,	dst)
_AR1(char *,	src)
	)
_DCL(char *,	dst)
_DCL(char *,	src)
{
	name_$pname_t	in_name, out_name;
	short		in_len,
			out_len;
	register char	*s, *d;
	status_$t	st;

#ifdef	lint
	out_len = 0;
#endif
	in_len = strlen(strcpy(in_name, src));
	name_$get_path(in_name, in_len, out_name, out_len, st);
	if (st.all == status_$ok)
		strncpy(dst, out_name, (size_t)out_len)[out_len] = EOS;
	else {
		if (	(st.all == name_$not_found)
		&&	(s = strrchr(in_name, '/'))
		&&	(s > in_name)
		&&	(s[-1] != '/') ) {
			*s = EOS;
			d = convert(dst, in_name);
			d += strlen(d);
			*d++ = '/';
			(void)name2s(d, BUFSIZ - (d - dst), ++s, 6);
		} else
			(void)name2s(dst, BUFSIZ, src, 6);
	}
	DEBUG "++ \"%s\" => \"%s\"\n", src, dst);
	return (dst);
}
#else		/* apollo sr10.x or unix */
/*
 * Use 'abshome()' to expand the tilde-only portion of the name to avoid
 * conflict between ".." trimming and symbolic links.
 */
static
char *
convert(
_ARX(char *,	dst)
_AR1(char *,	src)
	)
_DCL(char *,	dst)
_DCL(char *,	src)
{
	abshome(strcpy(dst, src));
	return (dst);
}
#endif		/* apollo sr9.7	*/
#endif		/* apollo */

/*
 * This procedure is used in the special case in which a user supplies source
 * arguments beginning with ".." constructs.  Strip these off before appending
 * to the destination-directory.
 */
static
char	*
skip_dots(
_AR1(char *,	path))
_DCL(char *,	path)
{
	while (path[0] == '.') {
		if (path[1] == '.') {
			if (path[2] == '/')
				path += 3;		/* skip "../" */
			else {
				if (path[2] == EOS)
					path += 2;	/* skip ".." */
				break;
			}
		} else if (path[1] == '/') {
			path += 2;			/* skip "./" */
		} else if (path[1] == EOS) {
			path++;				/* skip "." */
		} else
			break;				/* other .name */
	}
	return (path);
}

/*
 * On apollo machines, each file has an object type, which is not necessarily
 * mapped into the unix system properly.  Invoke the native APOLLO program
 * to do the copy.
 */
static
int
copyfile(
_ARX(char *,	src)
_ARX(char *,	dst)
_ARX(int,	previous)
_AR1(struct stat *,new_sb)
	)
_DCL(char *,	src)
_DCL(char *,	dst)
_DCL(int,	previous)
_DCL(struct stat *,new_sb)
{
	int	retval	= -1;
	char	bfr1[BUFSIZ];
#ifdef	apollo
	char	bfr2[BUFSIZ];

	if (access(src,R_OK) < 0) {
		perror(src);
		return(-1);
	}
	*bfr1 = EOS;
#ifdef	__STDC__
	catarg(bfr1, "-p");	/* ...so we can test for success of copy */
	catarg(bfr1, "-o");	/* ...to copy "real" apollo objects */
	if (previous)
		catarg(bfr1, "-f");
	catarg(bfr1, convert(bfr2, src));
	catarg(bfr1, convert(bfr2, dst));
	DEBUG "++ cp %s\n", bfr1);
	if (execute("/bin/cp", bfr1) < 0)
#else	/* apollo sr9 */
	catarg(bfr1, convert(bfr2,src));
	catarg(bfr1, convert(bfr2,dst));
	catarg(bfr1, "-pdt");	/* ...so we can test for success of copy */
	if (previous)
		catarg(bfr1, "-r");
	DEBUG "++ cpf %s\n", bfr1);
	if (execute("/com/cpf", bfr1) < 0)
#endif	/* apollo sr10/sr9 */
	{
		TELL "?? copy to %s failed\n", dst);
		return (-1);
	}
	if (previous) {		/* verify that we copied file */
		struct	stat	sb;
		if (stat(dst, &sb) < 0)
			return (-1);
		if ((sb.st_mtime != new_sb->st_mtime)
		||  (sb.st_size  != new_sb->st_size))
			return (-1);	/* copy was not successful */
	}
	retval = 0;
#else	/* unix	*/
	FILE	*ifp, *ofp;
	int	num;
	int	old_mode = new_sb->st_mode & 0777,
		tmp_mode = old_mode | 0600;	/* must be writeable! */

	if ((ifp = fopen(src, "r")) == 0) {
		perror(src);
	} else if (previous && chmod(dst, tmp_mode) < 0) {
		perror(dst);
	} else if ((ofp = fopen(dst, previous ? "w+" : "w")) == 0) {
		perror(dst);
	} else {
		retval = 0;		/* probably will go ok now */
		while ((num = fread(bfr1, 1, sizeof(bfr1), ifp)) > 0)
			if (fwrite(bfr1, 1, num, ofp) != num) {
					/* no, error found anyway */
				retval = -1;
				perror(dst);
				break;
			}
		FCLOSE(ofp);
	}
	FCLOSE(ifp);
	if (retval < 0)		/* restore old-mode in case of err */
		(void)chmod(dst, old_mode);
#endif	/* apollo/unix */
	return (retval);
}

#ifdef	S_IFLNK
static
int
copylink(
_ARX(char *,	src)
_AR1(char *,	dst)
	)
_DCL(char *,	src)
_DCL(char *,	dst)
{
	auto	char	bfr[BUFSIZ];
	auto	int	len;

	if ((len = readlink(src, bfr, sizeof(bfr))) < 0) {
		perror(src);
		return(-1);
	}
	bfr[len] = EOS;
	if (symlink(bfr, dst) < 0) {
		perror(dst);
		return(-1);
	}
	return (0);
}
#endif	/* S_IFLNK */

static
int
copydir(
_ARX(char *,	src)
_ARX(char *,	dst)
_AR1(int,	previous)
	)
_DCL(char *,	src)
_DCL(char *,	dst)
_DCL(int,	previous)
{
	auto	DIR		*dp;
	auto	struct	direct	*de;
	auto	char		bfr1[BUFSIZ],
				bfr2[BUFSIZ];
	auto	int		save_dir = no_dir_yet;

	DEBUG "copydir(%s, %s, %d)\n", src, dst, previous);
	abshome(strcpy(bfr1, dst));
	if (previous > 0) {
		if (!n_opt) {
			if (unlink(bfr1) < 0) {
				perror(dst);
				return(-1);
			}
		}
	}

	if (previous >= 0) {	/* called from 'copyit()' */
		VERBOSE "** make directory \"%s\"\n", dst);
		if (!n_opt) {
			int	omask	= umask(0);
			int	ok_make	= mkdir(bfr1, 0755) == 0;
			(void)umask(omask);
			if (!ok_make) {
				auto	int		save = errno;
				auto	struct	stat	sb;
				if ((stat(bfr1, &sb) < 0) || !isDIR(sb.st_mode)) {
					errno = save;
					perror(dst);
					return (-1);
				}
			}
		}
		no_dir_yet = n_opt;
	}

	if (dp = opendir(src)) {
		while (de = readdir(dp)) {
			if (!dotname(de->d_name))
				copyit(	pathcat(bfr1, src, de->d_name),
					pathcat(bfr2, dst, de->d_name));
		}
		(void)closedir(dp);
	}
	no_dir_yet = save_dir;
	return (0);	/* no errors found */
}

/*
 * Set up and perform a COPY
 */
static
int
copyit(
_ARX(char *,	src)
_AR1(char *,	dst)
	)
_DCL(char *,	src)
_DCL(char *,	dst)
{
	struct	stat	dst_sb, src_sb;
	int	num,
		ok1, ok2;
	char	bfr1[BUFSIZ],
		bfr2[BUFSIZ],
		temp[BUFSIZ];

	abshome(strcpy(bfr1, src));
	abshome(strcpy(bfr2, dst));
	src_sb.st_mode =
	dst_sb.st_mode = 0;

	/* Verify that the source and destinations are distinct */
	ok1 = stat(bfr1, &src_sb) >= 0;
	ok2 = stat(bfr2, &dst_sb) >= 0;
	if (ok1 && ok2) {
		if (src_sb.st_ino == dst_sb.st_ino
		&&  src_sb.st_dev == dst_sb.st_dev) {
			TELL "?? %s and %s are identical (not copied)\n",
				src, dst);
			exit(FAIL);
		}
	}
	DEBUG "** src: \"%s\"\n** dst: \"%s\"\n", bfr1, bfr2);

	if (!no_dir_yet) {
		if (isDIR(dst_sb.st_mode))
			(void)strcpy(temp, bfr2);
		else
			(void)strcpy(temp, pathhead(bfr2, &dst_sb));
		if (access(temp, W_OK) < 0) {
			TELL "?? directory is not writeable: \"%s\"\n", temp);
			return;
		}
	}

	VERBOSE "** copy \"%s\" to \"%s\"\n", src, dst);

	/* Verify that the source is a legal file */
#ifdef	S_IFLNK
	if ((!l_opt && (lstat(bfr1, &src_sb) < 0)) || src_sb.st_mode == 0) {
		TELL "?? file not found: \"%s\"\n", src);
		return;
	}
#endif
	if (!isFILE(src_sb.st_mode)
#ifdef	S_IFLNK
	&&  !isLINK(src_sb.st_mode)
#endif
	&&  !isDIR(src_sb.st_mode)) {
		TELL "?? not a file: \"%s\"\n", src);
		return;
	}
	src = bfr1;		/* correct tilde, if any */
	dst = bfr2;

	/* Check to see if we can overwrite the destination */
	if (num = (lstat(dst, &dst_sb) >= 0)) {
		if (isFILE(dst_sb.st_mode)
#ifdef	S_IFLNK
		||  isLINK(dst_sb.st_mode)
#endif
		) {
			if (i_opt) {
				TELL "%s ? ", dst);
				if (gets(temp)) {
					if (*temp != 'y' && *temp != 'Y')
						return;
				} else
					return;
			}
		} else if (isDIR(dst_sb.st_mode) && isDIR(src_sb.st_mode)) {
			num = FALSE;	/* we will merge directories */
		} else {
			TELL "?? cannot overwrite \"%s\"\n", dst);
			return;
		}
	} else
		dst_sb = src_sb;

	/* Unless disabled, copy the file */
	if (isDIR(src_sb.st_mode) && copydir(src,dst,num) < 0)
		return;

	if (!n_opt) {
#ifdef	S_IFLNK
		if (num && !isDIR(src_sb.st_mode) && isLINK(dst_sb.st_mode)) {
			if (unlink(dst) < 0) {
				perror(dst);
				return;
			}
		}
		if (isLINK(src_sb.st_mode)) {
			if (copylink(src,dst) < 0)
				return;
		}
#endif	/* S_IFLNK */
		if (isFILE(src_sb.st_mode) && copyfile(src,dst,num,&src_sb) < 0)
			return;
		if (isFILE(src_sb.st_mode) || isDIR(src_sb.st_mode)) {
			int	mode	= dst_sb.st_mode & 0777;
			if (isFILE(src_sb.st_mode) && s_opt) {
				mode	&= ~0222;	/* can't be writeable */
				mode	|=  (S_ISUID | S_ISGID);
			}
			if (isDIR(src_sb.st_mode) && !isDIR(dst_sb.st_mode))
				mode	&= 0111;
			(void)chmod(dst, mode);
			(void)setmtime(dst, src_sb.st_mtime);
		}
	}

	if (isDIR(src_sb.st_mode))
		total_dirs++;

	if (isFILE(src_sb.st_mode)) {
		total_files++;
		total_bytes += src_sb.st_size;
	}
#ifdef	S_IFLNK
	if (isLINK(src_sb.st_mode))
		total_links++;
#endif
}

static
void
usage(_AR0)
{
	auto	char	bfr[BUFSIZ];
	static	char	*tbl[] = {
 "Usage: copy [options] {[-d] | source [...]} destination"
,""
,"Options:"
,"  -d  infer source (leaf) from destination path"
,"  -i  interactive (prompt before overwriting)"
#ifdef	S_IFLNK
,"  -l  copy link-targets"
#endif
,"  -m  merge directories"
,"  -n  no-op (show what would be copied)"
,"  -s  enable set-uid/gid in target"
,"  -u  reset effective uid before executing"
,"  -v  verbose"
		};
	register int	j;
	setbuf(stderr, bfr);
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	(void)fflush(stderr);
	(void)exit(FAIL);
}

/*
 * Process argument list, turning it into source/destination pairs
 */
static
void
arg_pairs(
_ARX(int,	argc)
_AR1(char **,	argv)
	)
_DCL(int,	argc)
_DCL(char **,	argv)
{
	register int	j;
	auto	struct	stat	dst_sb,
				src_sb;
	auto	int	num, ok_dst;
	auto	char	dst[BUFSIZ];

	if ((num = (argc - optind)) < 2) {
		TELL "?? You must give both source and destination names\n");
		usage();
	}

	/* hacks to allow copying to a symbolic-link, or into a directory */
	abshome(strcpy(dst, argv[argc-1]));
#ifdef	S_IFLNK
	if (num == 2 && (dst[strlen(dst)-1] != '/'))
		ok_dst = (lstat(dst, &dst_sb) >= 0);
	else
#endif
		ok_dst = ( stat(dst, &dst_sb) >= 0);
	(void)strcpy(dst, argv[argc-1]);	/* restore for verbose-mode */

	if (ok_dst) {

		if (m_opt) {
			if (num == 2 && isDIR(dst_sb.st_mode)) {
				VERBOSE "** merge directories\n");
				if ((lstat(argv[optind], &src_sb) >= 0)
				&&  (isDIR(src_sb.st_mode))) {
					(void)copydir(
						argv[optind],
						argv[argc-1], -1);
					return;
				}
			}
			TELL "?? both arguments must be directories with -m\n");
			usage();
		}

		if (isDIR(dst_sb.st_mode)) {
			/* copy one or more items into directory */
			for (j = optind; j < argc-1; j++) {
			auto	char	*s = dst + strlen(dst),
					*t = skip_dots(argv[j]);
				*s = EOS;
				if (s[-1] != '/')
					(void)strcpy(s, "/");
				(void)strcat(s, pathleaf(t));
				copyit(argv[j], dst);
				*s = EOS;
			}
		} else if (num != 2) {
			TELL "?? Destination is not a directory\n");
			usage();
		} else if (isFILE(dst_sb.st_mode)
#ifdef	S_IFLNK
			|| isLINK(dst_sb.st_mode)
#endif
		) {
			copyit(argv[optind], dst);
		} else {
			TELL "?? Destination is not a file\n");
			usage();
		}
	} else {
		if (num != 2) {
			TELL "?? Wrong number of arguments\n");
			usage();
		}
		copyit(argv[optind], dst);
	}
}

/*
 * Process argument list, obtaining the source name (actually the leaf) from
 * each argument.
 */
static
void
derived(
_ARX(int,	argc)
_AR1(char **,	argv)
	)
_DCL(int,	argc)
_DCL(char **,	argv)
{
	register int	j;
	char	dst[BUFSIZ], *s;

	for (j = optind; j < argc; j++) {
		(void)strcpy(dst, argv[j]);
		while (s = strrchr(dst, '/')) {
			if (s[1]) {
				copyit(s+1, dst);
				break;
			} else
				*s = EOS;
		}
		if (s == 0)
			TELL "?? No destination directory: \"%s\"\n", argv[j]);
	}
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/
/*ARGSUSED*/
_MAIN
{
	register int	j;

	while ((j = getopt(argc, argv, "dilmnsuv")) != EOF) switch (j) {
	case 'd':	d_opt = TRUE;	break;
	case 'i':	i_opt = TRUE;	break;
#ifdef	S_IFLNK
	case 'l':	l_opt = TRUE;	break;
#endif
	case 'm':	m_opt = TRUE;	break;
	case 'n':	n_opt = TRUE;	break;
	case 's':	s_opt = TRUE;	break;
	case 'u':	(void)setuid(getuid());	break;
	case 'v':	v_opt++;	break;
	default:	usage();
	}

	if (d_opt)
		derived(argc, argv);
	else
		arg_pairs(argc, argv);

#define	MAY		n_opt ? "would be " : ""
#define	WOULD(n,y,s)	n, n == 1 ? y : s, MAY
#define	SUMMARY(n)	WOULD(n, "", "s")

	if (total_dirs)	VERBOSE "** %d director%s %scopied\n",
			WOULD(total_dirs,"y","ies"));
#ifdef	S_IFLNK
	if (total_links)VERBOSE "** %ld link%s %scopied\n",
			SUMMARY(total_links));
#endif
	if (total_files)VERBOSE "** %ld file%s %scopied, %ld bytes\n",
			SUMMARY(total_files),
			total_bytes);
	if (!(total_dirs || total_links || total_files))
		VERBOSE "** nothing %scopied\n", MAY);

	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
