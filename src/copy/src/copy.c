/*
 * Title:	copy.c (enhanced unix copy utility)
 * Author:	T.E.Dickey
 * Created:	16 Aug 1988
 * Modified:
 *		03 Jul 2002, workaround for broken NFS implementation, which
 *			     allows root to open a file for input but not to
 *			     read data from it.
 *		28 Aug 2001, show percent-progress on very large files.
 *			     Remove apollo code.
 *		09 Jan 2001, add -a option.
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

MODULE_ID("$Id: copy.c,v 11.31 2004/03/08 00:45:53 tom Exp $")

#define	if_Verbose	if (v_opt)
#define	if_Debug	if (v_opt > 1)

#define	S_MODEBITS	(~S_IFMT)

#if !defined(S_IFLNK) && !defined(lstat)
#define	lstat	stat
#endif /* S_IFLNK */

#ifdef	linux
#define	DOS_VISIBLE 1
#endif

#if defined(DOS_VISIBLE)
typedef enum _systype {
    Unix, MsDos
} SysType;
static SysType dst_type;
static SysType src_type;
#endif

static long total_dirs;
static long total_links;
static long total_files;
static long total_bytes;
static int d_opt;		/* obtain source from destination arg */
static int f_opt;		/* force: write into protected destination */
static int i_opt;		/* interactive: force prompt before overwrite */
static int l_opt;		/* copy symbolic-link-targets */
static int m_opt;		/* merge directories */
static int n_opt;		/* true if we don't actually do copies */
static int p_opt;		/* true if we try to preserve ownership */
static int s_opt;		/* enable set-uid/gid in target files */
static int u_opt;		/* update (1=all, 2=newer) */
static int v_opt;		/* verbose */
static int z_opt;		/* no dotfiles or dot-directories */

/************************************************************************
 *	local procedures						*
 ************************************************************************/

static int copyit(		/* forward-reference */
		     char *parent,
		     Stat_t * parent_sb,
		     char *src,
		     char *dst,
		     int no_dir_yet,
		     int tested_acc);

static void
problem(char *command, char *argument)
{
    char temp[BUFSIZ + MAXPATHLEN];
    strcpy(temp, command);
    strcat(temp, " ");
    strcat(temp, argument);
    perror(temp);
}

/*
 * This procedure is used in the special case in which a user supplies source
 * arguments beginning with ".." constructs.  Strip these off before appending
 * to the destination-directory.
 */
static char *
skip_dots(char *path)
{
    while (path[0] == '.') {
	if (path[1] == '.') {
	    if (path[2] == '/')
		path += 3;	/* skip "../" */
	    else {
		if (path[2] == EOS)
		    path += 2;	/* skip ".." */
		break;
	    }
	} else if (path[1] == '/') {
	    path += 2;		/* skip "./" */
	} else if (path[1] == EOS) {
	    path++;		/* skip "." */
	} else
	    break;		/* other .name */
    }
    return (path);
}

static int
SetOwner(char *path, int uid, int gid)
{
    if (p_opt) {
	if_Debug PRINTF("++ chown %03o %s\n", uid, path);
	if_Debug PRINTF("++ chgrp %03o %s\n", gid, path);
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
static int
SetMode(char *path, int mode)
{
    Stat_t sb;

    /* if the mkdir inherited an s-bit, keep it... */
    if (stat(path, &sb) == 0) {
	if (isDIR(sb.st_mode))
	    mode |= (sb.st_mode & (S_ISUID | S_ISGID));
    }
    if_Debug PRINTF("++ chmod %03o %s\n", mode, path);
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
static int
SetDate(char *path, time_t modified, time_t accessed)
{
#if	defined(DOS_VISIBLE)
    if (dst_type != src_type) {
	if (dst_type == MsDos) {	/* Linux to MsDos */
	    modified &= ~1L;
	    modified -= gmt_offset(modified);
	} else {		/* MsDos to Linux */
	    modified += gmt_offset(modified);
	}
    }
#endif
    if_Debug {
	struct tm split;
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
static void
RestoreMode(char *path, Stat_t * sb)
{
    (void) SetMode(path, (int) (sb->st_mode & S_MODEBITS));
}

/*
 * Show percent-progress if we're verbose
 */
static void
progress(unsigned long numer, unsigned long denom)
{
    static time_t last;
    time_t now;

    if_Verbose {
	if (denom != 0
	    && isatty(fileno(stderr))) {
	    if (numer == 0) {
		last = time((time_t *) 0);
	    } else if ((now = time((time_t *) 0)) != last) {
		last = now;
		FPRINTF(stderr, "%.1f%%\r",
			(numer * 100.0) / denom);
	    }
	}
    }
}

/*
 * Copy the file...
 */
static int
copyfile(char *src, char *dst, int previous, Stat_t * new_sb)
{
    int retval = -1;
    char bfr1[BUFSIZ];
    FILE *ifp, *ofp;
    unsigned long num;
    unsigned long transferred = 0;
    int did_chmod = TRUE;
    int old_mode = new_sb->st_mode & S_MODEBITS;
    int tmp_mode = old_mode | S_IWUSR;	/* must be writeable! */
    size_t want = (((long) new_sb->st_size > (long) sizeof(bfr1))
		   ? sizeof(bfr1)
		   : new_sb->st_size);

    if_Verbose PRINTF("** copy %s to %s\n", src, dst);
    if (n_opt)
	return 0;

    if ((ifp = fopen(src, "r")) == 0) {
	problem("fopen(src)", src);
    } else if ((num = fread(bfr1, sizeof(char), want, ifp)) < want) {
	if_Verbose PRINTF("?? cannot read %s\n", src);
	did_chmod = FALSE;
    } else if (previous && SetMode(dst, tmp_mode) < 0) {
	did_chmod = FALSE;
    } else if ((ofp = fopen(dst, previous ? "w+" : "w")) == 0) {
	problem("fopen(dst)", dst);
    } else {
	retval = 0;		/* probably will go ok now */
	do {
	    progress(transferred, new_sb->st_size);
	    if (fwrite(bfr1, sizeof(char), (size_t) num, ofp) != num) {
		/* no, error found anyway */
		retval = -1;
		problem("fwrite", dst);
		break;
	    }
	    progress(transferred += num, new_sb->st_size);
	} while ((num = fread(bfr1, sizeof(char), want, ifp)) > 0);
	FCLOSE(ofp);
	progress(transferred, new_sb->st_size);
    }
    if (ifp != 0)
	FCLOSE(ifp);
    if (retval < 0		/* restore old-mode in case of err */
	&& did_chmod)
	RestoreMode(dst, new_sb);
    return (retval);
}

#ifdef	S_IFLNK
static int
ReadLink(char *src, char *dst)
{
    int len = readlink(src, dst, BUFSIZ);
    if (len >= 0)
	dst[len] = EOS;
    return (len >= 0);
}

static int
samelink(char *src, char *dst)
{
    char bfr1[BUFSIZ];
    char bfr2[BUFSIZ];
    if (ReadLink(src, bfr1) && ReadLink(dst, bfr2))
	return !strcmp(bfr1, bfr2);
    return FALSE;
}

static int
copylink(char *src, char *dst)
{
    char bfr[BUFSIZ];

    if_Verbose PRINTF("** link %s to %s\n", src, dst);
    if (!n_opt) {
	if (!ReadLink(src, bfr)) {
	    problem("ReadLink(src)", src);
	    return (-1);
	}
	if (symlink(bfr, dst) < 0) {
	    problem("symlink", dst);
	    return (-1);
	}
    }
    return (0);
}
#endif /* S_IFLNK */

	/* patch: like 'stat_dir()', but uses lstat */
static int
dir_exists(char *path, Stat_t * sb)
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
static void
FindDir(char *parent, Stat_t * parent_sb, char *path)
{
    if (dir_exists(path, parent_sb) >= 0)
	(void) strcpy(parent, path);
    else
	(void) strcpy(parent, pathhead(path, parent_sb));
}

/*
 * (Re)creates the destination-directory, recursively copies the contents of
 * the source-directory into the destination.
 */
static int
copydir(char *src, char *dst, int previous)
{
    DIR *dp;
    DirentT *de;
    Stat_t dst_sb;
    char bfr1[BUFSIZ];
    char bfr2[BUFSIZ];
    int no_dir_yet = FALSE;

    if_Debug PRINTF("copydir(%s, %s, %d)\n", src, dst, previous);
    abshome(strcpy(bfr1, dst));
    if (previous > 0) {
	if_Debug PRINTF("++ rmdir %s\n", bfr1);
	if (!n_opt) {
	    if (rmdir(bfr1) < 0) {
		problem("rmdir", dst);
		return (-1);
	    }
	}
    }

    if (previous >= 0) {	/* called from 'copyit()' */
	if (dir_exists(bfr1, &dst_sb) < 0) {
	    if_Verbose PRINTF("** make directory \"%s\"\n", dst);
	    if (!n_opt) {
		int omask = umask(0);
		int ok_make = (mkdir(bfr1, 0755) >= 0);
		(void) umask(omask);
		if (!ok_make) {
		    int save = errno;
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
	char parent[MAXPATHLEN];
	int tested = 0, forced = 0;

	(void) strcpy(parent, bfr1);
	while ((de = readdir(dp)) != NULL) {
	    if (!dotname(de->d_name)) {
		forced |= copyit(parent, &dst_sb,
				 pathcat2(bfr1, src, de->d_name),
				 pathcat2(bfr2, dst, de->d_name),
				 no_dir_yet,
				 tested++);
	    }
	}
	(void) closedir(dp);
	if (forced)
	    RestoreMode(dst, &dst_sb);
    }
    return (0);			/* no errors found */
}

/*
 * Set up and perform a COPY.  Returns true the first time we must modify the
 * parent's protection.
 */
static int
copyit(char *parent,
       Stat_t * parent_sb,
       char *src,
       char *dst,
       int no_dir_yet,		/* true if we can test access */
       int tested_acc)		/* true iff we already tested */
{
    Stat_t dst_sb, src_sb;
    int num, ok1, ok2, forced = FALSE;
    char bfr1[BUFSIZ], bfr2[BUFSIZ], temp[BUFSIZ];

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
	int same_inode = (src_sb.st_ino == dst_sb.st_ino)
	&& (src_sb.st_dev == dst_sb.st_dev);
	if (same_inode) {	/* files might be hard-linked */
	    if_Debug PRINTF("?? %s and %s are identical (not copied)\n",
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
#if defined(DOS_VISIBLE)
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

		if ((src_sb.st_size == dst_sb.st_size)
		    && (src_sb.st_mtime == dst_sb.st_mtime))
		    return FALSE;
	    }
#ifdef	S_IFLNK
	    if (isLINK(src_sb.st_mode) && isLINK(dst_sb.st_mode)) {
		if (samelink(src, dst))
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
	    if (samelink(src, dst))
		return FALSE;
	}
    }
#endif
    if_Debug PRINTF("** src: \"%s\"\n** dst: \"%s\"\n", bfr1, bfr2);

    if (!no_dir_yet && !tested_acc) {	/* we must test-access */
	int writeable;
	if (!*parent)		/* ...we haven't tested-access here yet */
	    FindDir(parent, parent_sb, bfr2);
	writeable = (access(parent, W_OK | X_OK | R_OK) >= 0);
	if_Debug PRINTF(".. ACC: \"%s\" %s\n", parent, writeable ? "YES" :
			"NO");
	if (!writeable) {
	    if (f_opt) {
		if (SetMode(parent, 0755) < 0)
		    return FALSE;
		forced = TRUE;
	    } else {
		(void) fflush(stdout);
		FPRINTF(stderr, "?? directory is not writeable: \"%s\"\n",
			parent);
		(void) fflush(stderr);
		return FALSE;
	    }
	}
    }

    /* Verify that the source is a legal file */
#ifdef	S_IFLNK
    if ((!l_opt && (lstat(bfr1, &src_sb) < 0)) || src_sb.st_mode == 0) {
	(void) fflush(stdout);
	FPRINTF(stderr, "?? file not found: \"%s\"\n", src);
	(void) fflush(stderr);
	return forced;
    }
#endif
    if (!isFILE(src_sb.st_mode)
#ifdef	S_IFLNK
	&& !isLINK(src_sb.st_mode)
#endif
	&& !isDIR(src_sb.st_mode)) {
	(void) fflush(stdout);
	FPRINTF(stderr, "?? not a file: \"%s\"\n", src);
	(void) fflush(stderr);
	return forced;
    }
    src = bfr1;			/* correct tilde, if any */
    dst = bfr2;

    /* Check to see if we can overwrite the destination */
    if ((num = (lstat(dst, &dst_sb) >= 0)) != 0) {
	if (isFILE(dst_sb.st_mode)
#ifdef	S_IFLNK
	    || isLINK(dst_sb.st_mode)
#endif
	    ) {
	    if (i_opt) {
		(void) fflush(stdout);
		FPRINTF(stderr, "%s ? ", dst);
		(void) fflush(stderr);
		if (fgets(temp, sizeof(temp), stdin)) {
		    if (*temp != 'y' && *temp != 'Y')
			return forced;
		} else
		    return forced;
	    }
	} else if (isDIR(dst_sb.st_mode) && isDIR(src_sb.st_mode)) {
	    num = FALSE;	/* we will merge directories */
	} else {
	    (void) fflush(stdout);
	    FPRINTF(stderr, "?? cannot overwrite \"%s\"\n", dst);
	    (void) fflush(stderr);
	    return forced;
	}
    } else
	dst_sb = src_sb;

    /* Unless disabled, copy the file */
    if (isDIR(src_sb.st_mode) && copydir(src, dst, num) < 0)
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
	if (copylink(src, dst) < 0)
	    return forced;
    }
#endif /* S_IFLNK */

    if (isFILE(src_sb.st_mode)
	&& copyfile(src, dst, num, &src_sb) < 0)
	return forced;

    if (isFILE(src_sb.st_mode) || isDIR(src_sb.st_mode)) {
	int mode = dst_sb.st_mode & S_MODEBITS;
	if (isFILE(src_sb.st_mode) && s_opt) {
	    /* mustn't be writeable! */
	    mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
	    mode |= (S_ISUID | S_ISGID);
	}
	(void) SetOwner(dst, src_sb.st_uid, src_sb.st_gid);
	if (isDIR(src_sb.st_mode) && !isDIR(dst_sb.st_mode))
	    mode |= 0111;
	(void) SetMode(dst, mode);
	(void) SetDate(dst, src_sb.st_mtime, src_sb.st_atime);
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

static void
usage(void)
{
    char bfr[BUFSIZ];
    static const char *const tbl[] =
    {
	"Usage: copy [options] {[-d] | source [...]} destination"
	,""
	,"Options:"
	,"  -a      include dot-files (this is the default, use to override -z)"
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
	,"  -z      suppress dot-files"
#if	defined(DOS_VISIBLE)
	,"  -S      source is local-time filesystem"
	,"  -D      destination is local-time filesystem"
	,"  -F file specify MSDOS/Unix name-conversions"
#endif
    };
    unsigned j;
    setbuf(stderr, bfr);
    for (j = 0; j < SIZEOF(tbl); j++)
	FPRINTF(stderr, "%s\n", tbl[j]);
    (void) fflush(stderr);
    (void) exit(FAIL);
}

/*
 * Process argument list, turning it into source/destination pairs
 */
static void
arg_pairs(int argc, char **argv)
{
    int j;
    Stat_t parent_sb;
    Stat_t dst_sb;
    Stat_t src_sb;
    int num, ok_dst;
    char parent[MAXPATHLEN];
    char dst[BUFSIZ];

    if ((num = (argc - optind)) < 2) {
	FPRINTF(stderr,
		"?? You must give both source and destination names\n");
	usage();
    }

    /* hacks to allow copying to a symbolic-link, or into a directory */
    abshome(strcpy(dst, argv[argc - 1]));
    (void) strcpy(parent, dst);
#ifdef	S_IFLNK
    if (num == 2 && (dst[strlen(dst) - 1] != '/'))
	ok_dst = (lstat(dst, &dst_sb) >= 0);
    else
#endif
	ok_dst = (stat(dst, &dst_sb) >= 0);
    (void) strcpy(dst, argv[argc - 1]);		/* restore for verbose-mode */

    if (ok_dst) {

	if (m_opt) {
	    if (num == 2 && isDIR(dst_sb.st_mode)) {
		if_Verbose PRINTF("** merge directories\n");
		if ((lstat(argv[optind], &src_sb) >= 0)
		    && (isDIR(src_sb.st_mode))) {
		    (void) copydir(
				      argv[optind],
				      argv[argc - 1], -1);
		    return;
		}
	    }
	    FPRINTF(stderr,
		    "?? both arguments must be directories with -m\n");
	    usage();
	}

	if (isDIR(dst_sb.st_mode)) {	/* copy items into directory */
	    int tested = 0, forced = 0;
	    for (j = optind; j < argc - 1; j++) {
		char *s = dst + strlen(dst);
		char *t = skip_dots(argv[j]);
		*s = EOS;
		if (s[-1] != '/')
		    (void) strcpy(s, "/");
		(void) strcat(s, pathleaf(t));
		forced |= copyit(parent, &dst_sb,
				 argv[j], dst, FALSE, tested++);
		*s = EOS;
	    }
	    if (forced)
		RestoreMode(parent, &dst_sb);
	} else if (num != 2) {
	    (void) fflush(stdout);
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
	    (void) fflush(stdout);
	    FPRINTF(stderr, "?? Destination is not a file\n");
	    usage();
	}
    } else {
	if (num != 2) {
	    (void) fflush(stdout);
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
static void
derived(int argc, char **argv)
{
    int j;
    char *s;
    int tested = 0;
    int forced = 0;
    char parent[MAXPATHLEN];
    char dst[BUFSIZ];
    Stat_t parent_sb;

    FindDir(parent, &parent_sb, dst);
    for (j = optind; j < argc; j++) {
	(void) strcpy(dst, argv[j]);
	while ((s = strrchr(dst, '/')) != NULL) {
	    if (s[1]) {
		forced |= copyit(parent, &parent_sb,
				 s + 1,
				 dst,
				 FALSE, tested++);
		break;
	    } else
		*s = EOS;
	}
	if (s == 0) {
	    (void) fflush(stdout);
	    FPRINTF(stderr, "?? No destination directory: \"%s\"\n", argv[j]);
	    (void) fflush(stderr);
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
    int j;

    while ((j = getopt(argc, argv, "adfilmnpsuUvzSDF:")) != EOF)
	switch (j) {
	case 'a':
	    z_opt = FALSE;
	    break;
	case 'd':
	    d_opt = TRUE;
	    break;
	case 'f':
	    f_opt = TRUE;
	    break;
	case 'i':
	    i_opt = TRUE;
	    break;
#ifdef	S_IFLNK
	case 'l':
	    l_opt = TRUE;
	    break;
#endif
	case 'm':
	    m_opt = TRUE;
	    break;
	case 'n':
	    n_opt = TRUE;
	    break;
	case 'p':
	    p_opt = TRUE;
	    break;
	case 's':
	    s_opt = TRUE;
	    break;
	case 'u':
	    u_opt = 1;
	    break;
	case 'U':
	    u_opt = 2;
	    break;
	case 'v':
	    v_opt++;
	    break;
	case 'z':
	    z_opt = TRUE;
	    break;
#if	defined(DOS_VISIBLE)
	case 'S':
	    src_type = MsDos;
	    break;
	case 'D':
	    dst_type = MsDos;
	    break;
	case 'F':		/* patch */
	    break;
#endif
	default:
	    usage();
	}

    if (d_opt)
	derived(argc, argv);
    else
	arg_pairs(argc, argv);

#define	MAY		n_opt ? "would be " : ""
#define	WOULD(n,y,s)	n, n == 1 ? y : s, MAY
#define	SUMMARY(n)	WOULD(n, "", "s")

    if (total_dirs)
	if_Verbose PRINTF("** %ld director%s %scopied\n",
			  WOULD(total_dirs, "y", "ies"));
#ifdef	S_IFLNK
    if (total_links)
	if_Verbose PRINTF("** %ld link%s %scopied\n",
			  SUMMARY(total_links));
#endif
    if (total_files)
	if_Verbose PRINTF("** %ld file%s %scopied, %ld bytes\n",
			  SUMMARY(total_files),
			  total_bytes);
    if (!(total_dirs || total_links || total_files))
	if_Debug PRINTF("** nothing %scopied\n", MAY);

    (void) exit(SUCCESS);
    /*NOTREACHED */
}
