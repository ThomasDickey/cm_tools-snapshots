/*
 * Title:	copy.c (enhanced unix copy utility)
 * Author:	T.E.Dickey
 * Created:	16 Aug 1988
 * Modified:
 *		26 Dec 2000, add -p, -z options.  Make copy-dir less verbose.
 *			     Resolve conflict between -u and -U options.
 *		15 Jan 1999, use pathcat2 when combining data read from the
 *			     directory entries, since "~" does not mean anything
 *			     in that context.
 *		24 Sep 1996, add '-U' option
 *		25 Aug 1996, fix conflict between -l, -u options
 *		28 Jan 1995, retain 's' modes on destination directory.
 *		18 Jun 1994, removed 'S' setuid option.  Corrected test for
 *			     overwrite of same-inode.  Added S/D MSDOS
 *			     compatibility options for Linux. Make trace to
 *			     standard output.
 *		12 May 1994, made "nothing copied" message less verbose
 *		22 Sep 1993, gcc warnings
 *		02 Dec 1992, use 'rmdir()' rather than 'unlink()'.  Added logic
 *			     to ensure that we don't try to recreate a directory
 *			     that already exists.  Added "-f" (force) option.
 *		01 Dec 1992, renamed "-u" to "-S".  Added "-u" option.
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
#define		TIM_PTYPES	/* include time-definitions */
#include	<ptypes.h>
#include	<errno.h>

MODULE_ID("$Id: copy.c,v 11.24 2000/12/26 18:06:35 tom Exp $")

#define	if_Verbose	if (v_opt)
#define	if_Debug	if (v_opt > 1)

#define	VERBOSE	if_Verbose PRINTF
#define	DEBUG	if_Debug   PRINTF

#define	S_MODEBITS	(~S_IFMT)

#if !defined(S_IFLNK) && !defined(lstat)
#define	lstat	stat
#endif	/* S_IFLNK */

#ifdef	linux
#define	DOS_VISIBLE 1
#endif

#if DOS_VISIBLE
typedef	enum _systype { Unix, MsDos} SysType;
static	SysType	dst_type;
static	SysType	src_type;
#endif

static	long	total_dirs,
		total_links,
		total_files,
		total_bytes;
static	int	d_opt,		/* obtain source from destination arg */
		f_opt,		/* force: write into protected destination */
		i_opt,		/* interactive: force prompt before overwrite */
		l_opt,		/* copy symbolic-link-targets */
		m_opt,		/* merge directories */
		n_opt,		/* true if we don't actually do copies */
		p_opt,		/* true if we try to preserve ownership */
		s_opt,		/* enable set-uid/gid in target files */
		u_opt,		/* update (1=all, 2=newer) */
		v_opt,		/* verbose */
		z_opt;		/* no dotfiles or dot-directories */

/************************************************************************
 *	local procedures						*
 ************************************************************************/

static	int	copyit(		/* forward-reference */
		_arx(char *,	parent)
		_arx(Stat_t *,	parent_sb)
		_arx(char *,	src)
		_arx(char *,	dst)
		_arx(int,	no_dir_yet)
		_ar1(int,	tested_acc));

static
void	problem(
	_ARX(char *,	command)
	_AR1(char *,	argument)
		)
	_DCL(char *,	command)
	_DCL(char *,	argument)
{
	char temp[BUFSIZ + MAXPATHLEN];
	strcpy(temp, command);
	strcat(temp, " ");
	strcat(temp, argument);
	perror(temp);
}

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
char *	convert(
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
	DEBUG("++ \"%s\" => \"%s\"\n", src, dst);
	return (dst);
}
#else		/* apollo sr10.x or unix */
/*
 * Use 'abshome()' to expand the tilde-only portion of the name to avoid
 * conflict between ".." trimming and symbolic links.
 */
static
char *	convert(
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
char *	skip_dots(
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

static
int	SetOwner(
	_ARX(char *,	path)
	_ARX(int,	uid)
	_AR1(int,	gid)
		)
	_DCL(Stat_t *,	dst)
	_DCL(int,	uid)
	_DCL(int,	gid)
{
	if (p_opt) {
		DEBUG("++ chown %03o %s\n", uid, path);
		DEBUG("++ chgrp %03o %s\n", gid, path);
		if (!n_opt && (chown(path, uid, gid) < 0) && getuid() == 0) {
			problem("chown", path);
			return -1;
		}
	}
	return 0;
}

/*
 * Enable protection of a file temporarily, so we can write into it, or
 * set the protection after we're done copying.
 */
static
int	SetMode(
	_ARX(char *,	path)
	_AR1(int,	mode)
		)
	_DCL(char *,	path)
	_DCL(int,	mode)
{
	Stat_t	sb;

	/* if the mkdir inherited an s-bit, keep it... */
	if (stat(path, &sb) == 0) {
		if (isDIR(sb.st_mode))
			mode |= (sb.st_mode & (S_ISUID | S_ISGID));
	}
	DEBUG("++ chmod %03o %s\n", mode, path);
	if (!n_opt) {
		if (chmod(path, mode) < 0) {
			problem("chmod", path);
			return -1;
		}
	}
	return 0;
}

/*
 * Set the file's date.
 */
static
int	SetDate(
	_ARX(char *,	path)
	_ARX(time_t,	modified)
	_AR1(time_t,	accessed)
		)
	_DCL(char *,	path)
	_DCL(time_t,	modified)
	_DCL(time_t,	accessed)
{
#if	DOS_VISIBLE
	if (dst_type != src_type) {
		if (dst_type == MsDos) {	/* Linux to MsDos */
			modified &= ~1L;
			modified -= gmt_offset(modified);
		} else {			/* MsDos to Linux */
			modified += gmt_offset(modified);
		}
	}
#endif
	if_Debug {
		struct	tm split;
		split = *localtime(&modified);
		PRINTF("++ touch %02d%02d%02d%02d%04d.%02d %s\n",
			split.tm_mon + 1,
			split.tm_mday,
			split.tm_hour,
			split.tm_min,
			split.tm_year + 1900,
			split.tm_sec,
			path);
	}
	return n_opt ? 0 : setmtime(path, modified, accessed);
}

/*
 * Restore protection of a file that we changed temporarily
 */
static
void	RestoreMode(
	_ARX(char *,	path)
	_AR1(Stat_t *,	sb)
		)
	_DCL(char *,	path)
	_DCL(Stat_t *,	sb)
{
	(void)SetMode(path, (int)(sb->st_mode & S_MODEBITS));
}

/*
 * On apollo machines, each file has an object type, which is not necessarily
 * mapped into the unix system properly.  Invoke the native APOLLO program
 * to do the copy.
 */
static
int	copyfile(
	_ARX(char *,	src)
	_ARX(char *,	dst)
	_ARX(int,	previous)
	_AR1(Stat_t *,	new_sb)
		)
	_DCL(char *,	src)
	_DCL(char *,	dst)
	_DCL(int,	previous)
	_DCL(Stat_t *,	new_sb)
{
	int	retval	= -1;
	char	bfr1[BUFSIZ];
#ifdef	apollo
	char	bfr2[BUFSIZ];

	if (access(src,R_OK) < 0) {
		problem("access", src);
		return(-1);
	}

	VERBOSE("** copy %s to %s\n", src, dst);
	if (n_opt)
		return 0;

	*bfr1 = EOS;
#ifdef	__STDC__
	catarg(bfr1, "-p");	/* ...so we can test for success of copy */
	catarg(bfr1, "-o");	/* ...to copy "real" apollo objects */
	if (previous)
		catarg(bfr1, "-f");
	catarg(bfr1, convert(bfr2, src));
	catarg(bfr1, convert(bfr2, dst));
	DEBUG("++ cp %s\n", bfr1);
	if (execute("/bin/cp", bfr1) < 0)
#else	/* apollo sr9 */
	catarg(bfr1, convert(bfr2,src));
	catarg(bfr1, convert(bfr2,dst));
	catarg(bfr1, "-pdt");	/* ...so we can test for success of copy */
	if (previous)
		catarg(bfr1, "-r");
	DEBUG("++ cpf %s\n", bfr1);
	if (execute("/com/cpf", bfr1) < 0)
#endif	/* apollo sr10/sr9 */
	{
		(void)fflush(stdout);
		FPRINTF(stderr, "?? copy to %s failed\n", dst);
		(void)fflush(stderr);
		return (-1);
	}
	if (previous) {		/* verify that we copied file */
		Stat_t	sb;
		if (stat(dst, &sb) < 0)
			return (-1);
		if ((sb.st_mtime != new_sb->st_mtime)
		||  (sb.st_size  != new_sb->st_size))
			return (-1);	/* copy was not successful */
	}
	retval = 0;
#else	/* unix	*/
	FILE	*ifp, *ofp;
	unsigned num;
	int	did_chmod = TRUE;
	int	old_mode = new_sb->st_mode & S_MODEBITS,
		tmp_mode = old_mode | S_IWUSR;	/* must be writeable! */

	VERBOSE("** copy %s to %s\n", src, dst);
	if (n_opt)
		return 0;

	if ((ifp = fopen(src, "r")) == 0) {
		problem("fopen(src)", src);
	} else if (previous && SetMode(dst, tmp_mode) < 0) {
		did_chmod = FALSE;
	} else if ((ofp = fopen(dst, previous ? "w+" : "w")) == 0) {
		problem("fopen(dst)", dst);
	} else {
		retval = 0;		/* probably will go ok now */
		while ((num = fread(bfr1, 1, sizeof(bfr1), ifp)) > 0)
			if (fwrite(bfr1, 1, (size_t)num, ofp) != num) {
					/* no, error found anyway */
				retval = -1;
				problem("fwrite", dst);
				break;
			}
		FCLOSE(ofp);
	}
	FCLOSE(ifp);
	if (retval < 0		/* restore old-mode in case of err */
	 && did_chmod)
		RestoreMode(dst, new_sb);
#endif	/* apollo/unix */
	return (retval);
}

#ifdef	S_IFLNK
static
int	ReadLink(
	_ARX(char *,	src)
	_AR1(char *,	dst)
		)
	_DCL(char *,	src)
	_DCL(char *,	dst)
{
	register int	len = readlink(src, dst, BUFSIZ);
	if (len >= 0)
		dst[len] = EOS;
	return (len >= 0);
}

static
int	samelink(
	_ARX(char *,	src)
	_AR1(char *,	dst)
		)
	_DCL(char *,	src)
	_DCL(char *,	dst)
{
	auto	char	bfr1[BUFSIZ],
			bfr2[BUFSIZ];
	if (ReadLink(src, bfr1) && ReadLink(dst, bfr2))
		return !strcmp(bfr1, bfr2);
	return FALSE;
}

static
int	copylink(
	_ARX(char *,	src)
	_AR1(char *,	dst)
		)
	_DCL(char *,	src)
	_DCL(char *,	dst)
{
	auto	char	bfr[BUFSIZ];

	VERBOSE("** link %s to %s\n", src, dst);
	if (!n_opt) {
		if (!ReadLink(src, bfr)) {
			problem("ReadLink(src)", src);
			return(-1);
		}
		if (symlink(bfr, dst) < 0) {
			problem("symlink", dst);
			return(-1);
		}
	}
	return (0);
}
#endif	/* S_IFLNK */

static
int	dir_exists(	/* patch: like 'stat_dir()', but uses lstat */
	_ARX(char *,	path)
	_AR1(Stat_t *,	sb)
		)
	_DCL(char *,	path)
	_DCL(Stat_t *,	sb)
{
	if (lstat(path, sb) >= 0) {
		if (isDIR(sb->st_mode))
			return 0;
		errno = ENOTDIR;
	}
	return -1;
}

/*
 * Given a path, find the directory, so we can test access
 */
static
void	FindDir(
	_ARX(char *,	parent)
	_ARX(Stat_t *,	parent_sb)
	_AR1(char *,	path)
		)
	_DCL(char *,	parent)
	_DCL(Stat_t *,	parent_sb)
	_DCL(char *,	path)
{
	if (dir_exists(path, parent_sb) >= 0)
		(void)strcpy(parent, path);
	else
		(void)strcpy(parent, pathhead(path, parent_sb));
}

/*
 * (Re)creates the destination-directory, recursively copies the contents of
 * the source-directory into the destination.
 */
static
int	copydir(
	_ARX(char *,	src)
	_ARX(char *,	dst)
	_AR1(int,	previous)
		)
	_DCL(char *,	src)
	_DCL(char *,	dst)
	_DCL(int,	previous)
{
	auto	DIR	*dp;
	auto	DirentT *de;
	auto	Stat_t	dst_sb;
	auto	char	bfr1[BUFSIZ],
			bfr2[BUFSIZ];
	auto	int	no_dir_yet = FALSE;

	DEBUG("copydir(%s, %s, %d)\n", src, dst, previous);
	abshome(strcpy(bfr1, dst));
	if (previous > 0) {
		DEBUG("++ rmdir %s\n", bfr1);
		if (!n_opt) {
			if (rmdir(bfr1) < 0) {
				problem("rmdir", dst);
				return(-1);
			}
		}
	}

	if (previous >= 0) {	/* called from 'copyit()' */
		if (dir_exists(bfr1, &dst_sb) < 0) {
			VERBOSE("** make directory \"%s\"\n", dst);
			if (!n_opt) {
				int	omask	= umask(0);
				int	ok_make	= (mkdir(bfr1, 0755) >= 0);
				(void)umask(omask);
				if (!ok_make) {
					auto	int	save = errno;
					if (dir_exists(bfr1, &dst_sb) < 0) {
						errno = save;
						problem("dir_exists", dst);
						return (-1);
					}
				}
			}
			no_dir_yet = n_opt;
		} else {
			total_dirs--;	/* am not really copying it... */
		}
	} else if (dir_exists(bfr1, &dst_sb) < 0) {
		failed(dst);
	}

	if ((dp = opendir(src)) != NULL) {
		char	parent[MAXPATHLEN];
		int	tested	= 0,
			forced	= 0;

		(void)strcpy(parent, bfr1);
		while ((de = readdir(dp)) != NULL) {
			if (!dotname(de->d_name)) {
				forced |= copyit(parent, &dst_sb,
					pathcat2(bfr1, src, de->d_name),
					pathcat2(bfr2, dst, de->d_name),
					no_dir_yet,
					tested++);
			}
		}
		(void)closedir(dp);
		if (forced)
			RestoreMode(dst, &dst_sb);
	}
	return (0);	/* no errors found */
}

/*
 * Set up and perform a COPY.  Returns true the first time we must modify the
 * parent's protection.
 */
static
int	copyit(
	_ARX(char *,	parent)
	_ARX(Stat_t *,	parent_sb)
	_ARX(char *,	src)
	_ARX(char *,	dst)
	_ARX(int,	no_dir_yet)	/* true if we can test access */
	_AR1(int,	tested_acc)	/* true iff we already tested */
		)
	_DCL(char *,	parent)
	_DCL(Stat_t *,	parent_sb)
	_DCL(char *,	src)
	_DCL(char *,	dst)
	_DCL(int,	no_dir_yet)
	_DCL(int,	tested_acc)
{
	Stat_t	dst_sb, src_sb;
	int	num,
		ok1, ok2,
		forced	= FALSE;
	char	bfr1[BUFSIZ],
		bfr2[BUFSIZ],
		temp[BUFSIZ];

	if (z_opt && *fleaf(src) == '.')
		return 0;

	abshome(strcpy(bfr1, src));
	abshome(strcpy(bfr2, dst));
	src_sb.st_mode =
	dst_sb.st_mode = 0;

	/* Verify that the source and destinations are distinct */
	ok1 = stat(bfr1, &src_sb) >= 0;
	ok2 = stat(bfr2, &dst_sb) >= 0;
	if (ok1 && ok2) {
		int same_inode	=  (src_sb.st_ino == dst_sb.st_ino)
				&& (src_sb.st_dev == dst_sb.st_dev);
		if (same_inode) { /* files might be hard-linked */
			DEBUG("?? %s and %s are identical (not copied)\n",
				src, dst);
			return FALSE;
		}
		if (u_opt) {
#ifdef	S_IFLNK
			if (!l_opt) {
				lstat(bfr1, &src_sb);
				lstat(bfr2, &dst_sb);
			}
#endif
			if (isFILE(src_sb.st_mode) && isFILE(dst_sb.st_mode)) {
#if DOS_VISIBLE
				if (dst_type != src_type) {
					time_t it = dst_sb.st_mtime;
					it &= ~1L;
					if (dst_type == MsDos) {
						it += gmt_offset(it);
					} else {
						it -= gmt_offset(it);
					}
					dst_sb.st_mtime = it;
					src_sb.st_mtime &= ~1L;
				}
#endif
				if (u_opt >= 2
				 && (src_sb.st_mtime < dst_sb.st_mtime))
					return FALSE;

				if ((src_sb.st_size  == dst_sb.st_size)
				 && (src_sb.st_mtime == dst_sb.st_mtime))
					return FALSE;
			}
#ifdef	S_IFLNK
			if (isLINK(src_sb.st_mode) && isLINK(dst_sb.st_mode)) {
				if (samelink(src,dst))
					return FALSE;
			}
#endif
		}
	}
#ifdef	S_IFLNK
	else if (u_opt) {
		if (!l_opt) {
			lstat(bfr1, &src_sb);
			lstat(bfr2, &dst_sb);
		}
		if (isLINK(src_sb.st_mode) && isLINK(dst_sb.st_mode)) {
			if (samelink(src,dst))
				return FALSE;
		}
	}
#endif
	DEBUG("** src: \"%s\"\n** dst: \"%s\"\n", bfr1, bfr2);

	if (!no_dir_yet && !tested_acc) { /* we must test-access */
		int	writeable;
		if (!*parent)	/* ...we haven't tested-access here yet */
			FindDir(parent, parent_sb, bfr2);
		writeable = (access(parent, W_OK | X_OK | R_OK) >= 0);
		DEBUG(".. ACC: \"%s\" %s\n", parent, writeable ? "YES" : "NO");
		if (!writeable) {
			if (f_opt) {
				if (SetMode(parent, 0755) < 0)
					return FALSE;
				forced = TRUE;
			} else {
				(void)fflush(stdout);
				FPRINTF(stderr, "?? directory is not writeable: \"%s\"\n",
					parent);
				(void)fflush(stderr);
				return FALSE;
			}
		}
	}

	/* Verify that the source is a legal file */
#ifdef	S_IFLNK
	if ((!l_opt && (lstat(bfr1, &src_sb) < 0)) || src_sb.st_mode == 0) {
		(void)fflush(stdout);
		FPRINTF(stderr, "?? file not found: \"%s\"\n", src);
		(void)fflush(stderr);
		return forced;
	}
#endif
	if (!isFILE(src_sb.st_mode)
#ifdef	S_IFLNK
	&&  !isLINK(src_sb.st_mode)
#endif
	&&  !isDIR(src_sb.st_mode)) {
		(void)fflush(stdout);
		FPRINTF(stderr, "?? not a file: \"%s\"\n", src);
		(void)fflush(stderr);
		return forced;
	}
	src = bfr1;		/* correct tilde, if any */
	dst = bfr2;

	/* Check to see if we can overwrite the destination */
	if ((num = (lstat(dst, &dst_sb) >= 0)) != 0) {
		if (isFILE(dst_sb.st_mode)
#ifdef	S_IFLNK
		||  isLINK(dst_sb.st_mode)
#endif
		) {
			if (i_opt) {
				(void)fflush(stdout);
				FPRINTF(stderr, "%s ? ", dst);
				(void)fflush(stderr);
				if (fgets(temp, sizeof(temp), stdin)) {
					if (*temp != 'y' && *temp != 'Y')
						return forced;
				} else
					return forced;
			}
		} else if (isDIR(dst_sb.st_mode) && isDIR(src_sb.st_mode)) {
			num = FALSE;	/* we will merge directories */
		} else {
			(void)fflush(stdout);
			FPRINTF(stderr, "?? cannot overwrite \"%s\"\n", dst);
			(void)fflush(stderr);
			return forced;
		}
	} else
		dst_sb = src_sb;

	/* Unless disabled, copy the file */
	if (isDIR(src_sb.st_mode) && copydir(src,dst,num) < 0)
		return forced;

#ifdef	S_IFLNK
	if (!n_opt
	 && num != 0
	 && !isDIR(src_sb.st_mode)
	 && isLINK(dst_sb.st_mode)) {
		if (unlink(dst) < 0) {
			problem("unlink(dst)", dst);
			return forced;
		}
	}
	if (isLINK(src_sb.st_mode)) {
		if (copylink(src,dst) < 0)
			return forced;
	}
#endif	/* S_IFLNK */

	if (isFILE(src_sb.st_mode)
	 && copyfile(src,dst,num,&src_sb) < 0)
		return forced;

	if (isFILE(src_sb.st_mode) || isDIR(src_sb.st_mode)) {
		int	mode	= dst_sb.st_mode & S_MODEBITS;
		if (isFILE(src_sb.st_mode) && s_opt) {
			/* mustn't be writeable! */
			mode	&= ~(S_IWUSR | S_IWGRP | S_IWOTH);
			mode	|=  (S_ISUID | S_ISGID);
		}
		(void)SetOwner(dst, src_sb.st_uid, src_sb.st_gid);
		if (isDIR(src_sb.st_mode) && !isDIR(dst_sb.st_mode))
			mode	|= 0111;
		(void)SetMode(dst, mode);
		(void)SetDate(dst, src_sb.st_mtime, src_sb.st_atime);
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
	return forced;
}

static
void	usage(_AR0)
{
	auto	char	bfr[BUFSIZ];
	static	const	char	*const tbl[] = {
 "Usage: copy [options] {[-d] | source [...]} destination"
,""
,"Options:"
,"  -d      infer source (leaf) from destination path"
,"  -f      force (write into protected destination"
,"  -i      interactive (prompt before overwriting)"
#ifdef	S_IFLNK
,"  -l      copy link-targets (otherwise, copy links as-is)"
#endif
,"  -m      merge directories"
,"  -n      no-op (show what would be copied)"
,"  -p      preserve ownership"
,"  -s      enable set-uid/gid in target"
,"  -u      update-only (copies only new files or those differing in size or date)"
,"  -U      update-only (same as -u, but copies newer files)"
,"  -v      verbose"
#if	DOS_VISIBLE
,"  -S      source is local-time filesystem"
,"  -D      destination is local-time filesystem"
,"  -F file specify MSDOS/Unix name-conversions"
#endif
		};
	unsigned j;
	setbuf(stderr, bfr);
	for (j = 0; j < SIZEOF(tbl); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	(void)fflush(stderr);
	(void)exit(FAIL);
}

/*
 * Process argument list, turning it into source/destination pairs
 */
static
void	arg_pairs(
	_ARX(int,	argc)
	_AR1(char **,	argv)
		)
	_DCL(int,	argc)
	_DCL(char **,	argv)
{
	register int	j;
	auto	Stat_t	parent_sb,
			dst_sb,
			src_sb;
	auto	int	num, ok_dst;
	auto	char	parent[MAXPATHLEN];
	auto	char	dst[BUFSIZ];

	if ((num = (argc - optind)) < 2) {
		FPRINTF(stderr, "?? You must give both source and destination names\n");
		usage();
	}

	/* hacks to allow copying to a symbolic-link, or into a directory */
	abshome(strcpy(dst, argv[argc-1]));
	(void)strcpy(parent, dst);
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
				VERBOSE("** merge directories\n");
				if ((lstat(argv[optind], &src_sb) >= 0)
				&&  (isDIR(src_sb.st_mode))) {
					(void)copydir(
						argv[optind],
						argv[argc-1], -1);
					return;
				}
			}
			FPRINTF(stderr, "?? both arguments must be directories with -m\n");
			usage();
		}

		if (isDIR(dst_sb.st_mode)) { /* copy items into directory */
			int	tested	= 0,
				forced	= 0;
			for (j = optind; j < argc-1; j++) {
			auto	char	*s = dst + strlen(dst),
					*t = skip_dots(argv[j]);
				*s = EOS;
				if (s[-1] != '/')
					(void)strcpy(s, "/");
				(void)strcat(s, pathleaf(t));
				forced |= copyit(parent, &dst_sb,
					argv[j], dst, FALSE, tested++);
				*s = EOS;
			}
			if (forced)
				RestoreMode(parent, &dst_sb);
		} else if (num != 2) {
			(void)fflush(stdout);
			FPRINTF(stderr, "?? Destination is not a directory\n");
			usage();
		} else if (isFILE(dst_sb.st_mode)
#ifdef	S_IFLNK
			|| isLINK(dst_sb.st_mode)
#endif
		) {
			*parent = EOS;
			if (copyit(parent, &parent_sb,
					argv[optind], dst, FALSE, FALSE))
				RestoreMode(parent, &parent_sb);
		} else {
			(void)fflush(stdout);
			FPRINTF(stderr, "?? Destination is not a file\n");
			usage();
		}
	} else {
		if (num != 2) {
			(void)fflush(stdout);
			FPRINTF(stderr, "?? Wrong number of arguments\n");
			usage();
		}
		*parent = EOS;
		if (copyit(parent, &parent_sb, argv[optind], dst, FALSE, FALSE))
			RestoreMode(parent, &parent_sb);
	}
}

/*
 * Process argument list, obtaining the source name (actually the leaf) from
 * each argument.
 */
static
void	derived(
	_ARX(int,	argc)
	_AR1(char **,	argv)
		)
	_DCL(int,	argc)
	_DCL(char **,	argv)
{
	register int	j;
	register char	*s;
	auto	int	tested	= 0,
			forced	= 0;
	auto	char	parent[MAXPATHLEN],
			dst[BUFSIZ];
	auto	Stat_t	parent_sb;

	FindDir(parent, &parent_sb, dst);
	for (j = optind; j < argc; j++) {
		(void)strcpy(dst, argv[j]);
		while ((s = strrchr(dst, '/')) != NULL) {
			if (s[1]) {
				forced |= copyit(parent, &parent_sb,
					s+1,
					dst,
					FALSE, tested++);
				break;
			} else
				*s = EOS;
		}
		if (s == 0) {
			(void)fflush(stdout);
			FPRINTF(stderr, "?? No destination directory: \"%s\"\n", argv[j]);
			(void)fflush(stderr);
		}
	}
	if (forced)
		RestoreMode(parent, &parent_sb);
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/
/*ARGSUSED*/
_MAIN
{
	register int	j;

	while ((j = getopt(argc, argv, "dfilmnpsuUvzSDF:")) != EOF) switch (j) {
	case 'd':	d_opt = TRUE;	break;
	case 'f':	f_opt = TRUE;	break;
	case 'i':	i_opt = TRUE;	break;
#ifdef	S_IFLNK
	case 'l':	l_opt = TRUE;	break;
#endif
	case 'm':	m_opt = TRUE;	break;
	case 'n':	n_opt = TRUE;	break;
	case 'p':	p_opt = TRUE;	break;
	case 's':	s_opt = TRUE;	break;
	case 'u':	u_opt = 1;	break;
	case 'U':	u_opt = 2;	break;
	case 'v':	v_opt++;	break;
	case 'z':	z_opt = TRUE;	break;
#if	DOS_VISIBLE
	case 'S':	src_type = MsDos;		break;
	case 'D':	dst_type = MsDos;		break;
	case 'F':	/* patch */			break;
#endif
	default:	usage();
	}

	if (d_opt)
		derived(argc, argv);
	else
		arg_pairs(argc, argv);

#define	MAY		n_opt ? "would be " : ""
#define	WOULD(n,y,s)	n, n == 1 ? y : s, MAY
#define	SUMMARY(n)	WOULD(n, "", "s")

	if (total_dirs)	VERBOSE("** %ld director%s %scopied\n",
			WOULD(total_dirs,"y","ies"));
#ifdef	S_IFLNK
	if (total_links)VERBOSE("** %ld link%s %scopied\n",
			SUMMARY(total_links));
#endif
	if (total_files)VERBOSE("** %ld file%s %scopied, %ld bytes\n",
			SUMMARY(total_files),
			total_bytes);
	if (!(total_dirs || total_links || total_files))
		DEBUG("** nothing %scopied\n", MAY);

	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
