/*
 * Title:	baseline.c (rcs baseline)
 * Author:	T.E.Dickey
 * Created:	24 Oct 1989
 * Modified:
 *		04 Dec 2019, use "executev()" and "show_argv()"
 *		22 Sep 1993, gcc warnings
 *		17 Jan 1992, added "-L" option.  Renamed "-r" to "-R" option
 *			     and "-f" to "-p" for consistency.
 *		22 Oct 1991, use 'shoarg()'
 *		11 Oct 1991, converted to ANSI
 *		20 May 1991, apollo sr10.3 cpp complains about endif-tags
 *		16 Apr 1990, "-f" option must allow 'permit' to run, otherwise
 *			     the RCS,v file won't be updated.  Made "-f" only
 *			     suppress 'permit' "-p" option.
 *		03 Jan 1990, added "-l" option for stluka. added logic (for "-l"
 *			     option) so that we can allow a locked file to be
 *			     baselined (if it is locked, but not changed).
 *			     Corrected code which infers baseline revision (if
 *			     no rcs directory existed, this did not notice!)
 *		07 Nov 1989, added "-f" option, tuned some verboseness.
 *			     Corrected treatment of "-r" option (recur only
 *			     when asked).  Corrected code which infers current-
 *			     baseline version.
 *		01 Nov 1989, walktree passes null-pointer to stat-block if
 *			     no-access.
 *
 * Function:	Use checkin/checkout to force a tree of files to have a common
 *		baseline-version number.
 *
 *		This is a rewrite of the Bourne-shell script 'baseline.sh'.
 *
 * Options:	(see 'usage()')
 */

#define	STR_PTYPES
#include	<ptypes.h>
#include	<rcsdefs.h>
#include	<sccsdefs.h>
#include	<ctype.h>
#include	<time.h>

MODULE_ID("$Id: baseline.c,v 11.10 2019/12/04 09:41:24 tom Exp $")

#define	isDIR(mode)	((mode & S_IFMT) == S_IFDIR)
#define	isFILE(mode)	((mode & S_IFMT) == S_IFREG)
#define	isLINK(mode)	((mode & S_IFMT) == S_IFLNK)

typedef struct _list {
    struct _list *link;
    char *text;
} LIST;

static time_t began;
static int revision = -1;
static char m_option[BUFSIZ];	/* message to show as "-m" */
static char r_option[80];

				/* options */
static int a_opt;		/* all-directory scan */
static int lock_up;		/* leave files locked */
static char *m_text;		/* message-text */
static int no_op;		/* no-op mode */
#ifdef	S_IFLNK
static int L_opt;
#endif
static int purge_opt;
static int recur;
static int verbose;

static void
doit(char **argv, int really)
{
    if (verbose) {
	show_argv(stdout, argv);
    }
    if (really) {
	if (executev(argv) < 0) {
	    FPRINTF(stderr, "?? %s\n", argv[0]);
	    exit(FAIL);
	}
    }
}

static void
purge_rights(const char *path)
{
    int argc;
    char *argv[20];
    char myrev[80];
    static LIST *purged;
    LIST *p;

    for (p = purged; p; p = p->link) {
	if (!strcmp(p->text, path))
	    return;
    }
    p = ALLOC(LIST, 1);
    p->link = purged;
    p->text = txtalloc(path);
    purged = p;

    argc = 0;
    argv[argc++] = txtalloc("permit");
    FORMAT(myrev, "-b%d", revision);
    argv[argc++] = myrev;
    if (!verbose)
	argv[argc++] = txtalloc("-q");
    if (no_op)
	argv[argc++] = txtalloc("-n");
    if (purge_opt)
	argv[argc++] = txtalloc("-p");
    argv[argc++] = m_option;
    argv[argc++] = txtalloc(rcs_dir(NULL, NULL));
    argv[argc] = NULL;
    doit(argv, no_op < 2);
}

static void
baseline(const char *path, const char *name, time_t edited)
{
    int argc;
    char *argv[20];
    char myrev[80];
    const char *version;
    const char *locker;
    time_t date;
    int cmp;
    int pending;		/* true if lock implies change */

    purge_rights(path);
    rcslast(path, name, &version, &date, &locker);
    pending = (!lock_up) || (date != edited);

    if (*version == '?') {
	if (istextfile(name)) {
	    FPRINTF(stderr, "?? %s (not archived)\n", name);
	    if (!recur)
		exit(FAIL);
	} else
	    PRINTF("** %s (ignored)\n", name);
	return;
    } else if ((cmp = vercmp(r_option, version, FALSE)) < 0) {
	FPRINTF(stderr, "?? %s (version mismatch -b%d vs %s)\n",
		name, revision, version);
	exit(FAIL);
	/*NOTREACHED */
    } else if (cmp == 0) {
	PRINTF("** %s (already baselined)\n", name);
	return;
    } else if (*locker != '?' && pending) {
	FPRINTF(stderr, "?? %s (locked by %s)\n", name, locker);
	exit(FAIL);
	/*NOTREACHED */
    }
    PRINTF("** %s\n", name);

    if (*locker == '?') {	/* wasn't locked, must do so now */
	argc = 0;
	argv[argc++] = txtalloc("checkout");
	argv[argc++] = txtalloc("-l");
	if (!verbose)
	    argv[argc++] = txtalloc("-q");
	argv[argc++] = txtalloc(name);
	argv[argc] = NULL;
	doit(argv, !no_op);
    }

    argc = 0;
    argv[argc++] = txtalloc("checkin");
    FORMAT(myrev, "-r%d.0", revision);
    argv[argc++] = myrev;
    argv[argc++] = txtalloc("-f");
    argv[argc++] = txtalloc("-u");
    if (!verbose)
	argv[argc++] = txtalloc("-q");
    if (lock_up)
	argv[argc++] = txtalloc("-l");
    argv[argc++] = m_option;
    argv[argc++] = txtalloc("-sRel");
    argv[argc++] = txtalloc(name);
    argv[argc] = NULL;
    doit(argv, !no_op);
}

static int
WALK_FUNC(scan_tree)
{
    char tmp[BUFSIZ];
    char *s = pathcat(tmp, path, name);

    if (sp == 0 || level > recur)
	readable = -1;
    else if (isDIR(sp->st_mode)) {
	abspath(s);		/* get rid of "." and ".." names */
	if (!a_opt && *pathleaf(s) == '.')
	    readable = -1;
	else if (sameleaf(s, sccs_dir(path, name))
		 || sameleaf(s, rcs_dir(path, name))) {
	    readable = -1;
	} else {
	    if (level > recur) {
		PRINTF("** %s (ignored)\n", name);
		readable = -1;
	    } else {
#ifdef	S_IFLNK
		if (!L_opt) {
		    struct stat sb;
		    if (lstat(name, &sb) < 0
			|| isLINK(sb.st_mode)) {
			PRINTF("** %s (link)\n", name);
			readable = -1;
		    }
		} else
#endif
		    track_wd(s);
	    }
	}
    } else if (isFILE(sp->st_mode)) {
	if (level > 0)
	    track_wd(path);
	baseline(path, name, sp->st_mtime);
    } else
	readable = -1;

    return (readable);
}

static void
do_arg(const char *name)
{
    int infer_rev = FALSE;

    FORMAT(m_option, "-mBASELINE %s", ctime(&began));
    m_option[strlen(m_option) - 1] = EOS;
    if (m_text)
	(void) strcat(strcat(m_option, " -- "), m_text);

    /*
     * If no revision was given, infer it, assuming that we are going to
     * add forgotten stuff to the current baseline.
     */
    if (revision < 0) {
	char vname[BUFSIZ];
	time_t date;
	const char *locker, *version;
	rcslast(".",
		vcs_file(rcs_dir(NULL, NULL), vname, FALSE),
		&version, &date, &locker);
	if (*version == '?'
	    || sscanf(version, "%d.", &revision) <= 0)
	    revision = 2;
	infer_rev = TRUE;
    }
    FORMAT(r_option, "%d.0", revision);

    (void) walktree((char *) 0, name, scan_tree, "r", 0);

    if (infer_rev)
	revision = -1;
}

static void
usage(void)
{
    static const char *tbl[] =
    {
	"Usage: baseline [options] [files]"
	,""
	,"Bumps versions of rcs files to the specified integer value (i.e., 2.0)."
	,"If no files are specified, all files in the current directory are assumed."
	,""
	,"Options are:"
	,"  -{integer}  baseline version (must be higher than last baseline)"
	,"  -a          process directories beginning with \".\""
	,"  -l          leave files locked after baselining"
	,"  -m TEXT     append reason for baseline to date-message"
#ifdef	S_IFLNK
	,"  -L          traverse symbolic links to directories"
#endif
	,"  -n          no-op (show what would be done, but don't do it)"
	,"  -p          run \"permit\" first to purge access lists"
	,"  -R          recur when lower-level directories are found"
	,"  -v          verbose (shows details)"
    };
    unsigned j;
    for (j = 0; j < sizeof(tbl) / sizeof(tbl[0]); j++)
	FPRINTF(stderr, "%s\n", tbl[j]);
    FFLUSH(stderr);
    exit(FAIL);
}

/*ARGSUSED*/
_MAIN
{
    int j;
    char *s;
    char *d;
    int had_args = FALSE;

    track_wd((char *) 0);
    began = time((time_t *) 0);

    for (j = 1; j < argc; j++) {
	if (*(s = argv[j]) == '-') {
	    while (*(++s)) {
		if (isdigit(UCH(*s))) {
		    if ((revision = (int) strtol(s, &d, 10)) < 2)
			usage();
		    s = d - 1;
		} else if (*s == 'm') {
		    if (*(++s)) {
			m_text = s;
			break;
		    } else
			usage();
		} else {
		    switch (*s) {
		    case 'a':
			a_opt++;
			break;
		    case 'l':
			lock_up++;
			break;
		    case 'n':
			no_op++;
			break;
#ifdef	S_IFLNK
		    case 'L':
			L_opt++;
			break;
#endif
		    case 'p':
			purge_opt++;
			break;
		    case 'R':
			recur = 999;
			break;
		    case 'v':
			verbose++;
			break;
		    default:
			usage();
		    }
		}
	    }
	} else {
	    do_arg(s);
	    had_args = TRUE;
	}
    }

    if (!had_args) {
	recur++;
	do_arg(".");
    }
    exit(SUCCESS);
    /*NOTREACHED */
}
