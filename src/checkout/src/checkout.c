/*
 * Title:	checkout.c (front end for RCS checkout)
 * Author:	T.E.Dickey
 * Created:	20 May 1988 (from 'sccsdate.c')
 * Modified:
 *		05 Dec 2019, use DYN-argv lists.
 *		14 Dec 2014, coverity warnings
 *		04 Sep 2012, correction to previous -c change.
 *		23 Oct 2005, correct parsing if -c param is not a separate
 *			     option.
 *		28 Dec 2000, restore file-ownership if setuid'd to root.
 *		12 Nov 1994, pass-thru '-f', '-k' options to 'ci'.
 *		22 Sep 1993, gcc warnings
 *		24 Jun 1993, fixes for apollo-setuid for RCS version 5.
 *		02 Nov 1992, mods for RCS version 5.
 *		16 Jul 1992, corrected call on 'cutoff()'
 *		06 Feb 1992, revise filename-parsing with 'rcsargpair()',
 *			     obsoleted "-x" option.
 *		17 Jan 1992, make this interpret "-p" option.
 *		21 Oct 1991, corrected uid-use in pre-processing.  Handle case
 *			     in which user has write-access in the RCS directory
 *			     even if he owns none of the files.
 *		09 Oct 1991, convert to ANSI. Correct non-root setuid in the
 *			     case where real user has proper permissions.
 *			     (i.e., don't do rcstemp-hack).
 *		08 Oct 1991, stifle message about 'revert()' if root-setuid.
 *		06 Sep 1991, modified interface to 'rcsopen()'
 *		11 Jul 1991, make this work properly with suid-root
 *		20 Jun 1990, use 'shoarg()'
 *		20 May 1991, mods to compile on apollo sr10.3
 *		19 Apr 1990, corrected so we don't pass "-x" to 'co'.
 *			     added "-x" option (to help with makefiles, etc)
 *		10 Oct 1989, corrected last change (if no directory name is
 *			     explicit, use ".")
 *		24 Aug 1989, exit with error if 'usercopy()' fails.  Suppress
 *			     'revert()' message if user does not want to lock
 *			     the file.  Also, verify that working file's
 *			     directory exists (otherwise 'usercopy()' silently
 *			     fails...)
 *		22 Aug 1989, corrected format of debug-message
 *		29 Mar 1989, if we cannot find the archive, this may be because
 *			     the user has hidden his directory (and therefore
 *			     we cannot see it in setuid mode).  Revert to normal
 *			     rights in this case and try again.
 *		28 Mar 1989, forgot to invoke 'rcspermit()' to check directory-
 *			     level permit.
 *		21 Mar 1989, strip setuid privilege if caller wants to check out
 *			     a file not owned by the setuid process.
 *		24 Jan 1989, don't insist that archive be owned by user or euid
 *			     if no lock is being made.  Corrected hole which
 *			     caused working file to be deleted if setuid-use
 *			     prompted for overwrite which was rejected.
 *		14 Dec 1988, use 'vercmp()' rather than 'dotcmp()', to make
 *			     retrieval of increments within a baseline get a
 *			     correct timestamp.
 *		06 Dec 1988, added some DEBUG-traces.
 *		13 Sep 1988, added cleanup handler.  Refined permission-checks.
 *		09 Sep 1988, use 'rcspath()'
 *		30 Aug 1988, broke out 'userprot()'.
 *		25 Aug 1988, check for and accommodate 'setuid()' usage.
 *		24 Aug 1988, added 'usage()' message.  Implemented '-c' cutoff,
 *			     and '-w', '-s' options.  If user has locked RCS
 *			     file, make the checked-out file writeable.
 *		13 Jun 1988, use 'newzone()'.
 *		08 Jun 1988, more adjustments to 'chmod()'.
 *		07 Jun 1988, make this set the checked-out file's mode to
 *			     readonly unless it is locked.  (The 'co' utility
 *			     neglects to do this & is needed on apollo).
 *		27 May 1988, recoded using 'rcsedit' module.
 *
 * Function:	Make a package around the RCS utility 'co', added the ability
 *		to manipulate the checkin date, and file ownership.
 *
 * Options:	This passes through to 'co' all options except the '-d' (date)
 *		option -- we interpret a '-c' in SCCS-style to simplify the
 *		implementation.
 *
 * patch:	This does not interpret branches.
 *
 *		If the user has locked a version other than the tip, and then
 *		does a 'co' without the version, 'co' will still give the
 *		tip-version.  Should provide the revision-option to 'co' myself.
 *		For now, simply assume that locks are on the tip version.
 */

#define		ACC_PTYPES
#define		SIG_PTYPES
#define		STR_PTYPES
#include	<ptypes.h>
#include	<rcsdefs.h>
#include	<dyn_str.h>

#include	<errno.h>
#include	<ctype.h>
#include	<signal.h>
#include	<time.h>

MODULE_ID("$Id: checkout.c,v 11.21 2025/01/07 00:54:05 tom Exp $")

/* local definitions */
#define	TELL	if (!silent) FPRINTF
#define	DEBUG(s)	if(debug) FPRINTF s

#define	EMPTY(s)	(s == NULL || *s == EOS)
#define	MISMATCH(a,b)	((!EMPTY(a)) && strcmp(a,b))

#define	REVSIZ	80

#define	CO_TOOL	"co"

static time_t opt_date;
static int silent;
static int debug;		/* set from environment RCS_DEBUG */
static int no_op;		/* suppress actual "co" invocation */
static int locked;		/* TRUE if user is locking file */
static FILE *log_fp;		/* normally stdout, unless "-p" */
static int to_stdout;		/* TRUE if 'co' writes to stdout */

static uid_t Effect, Caller;	/* effective/real uid's */
static char Working[MAXPATHLEN];	/* current names we are using */
static char Archive[MAXPATHLEN];
static char rev_buffer[REVSIZ];
static const char *UidHack;	/* intermediate file for setuid */
static char *opt_rev;		/* revision to find */
static char *opt_who;		/* "-w[login] value     */
static char *opt_sta;		/* "-s[state] value     */
static char *co_f_opt;		/* "-f" option, if any  */
static char *co_k_opt;		/* "-k" option, if any  */

/************************************************************************
 *	local procedures						*
 ************************************************************************/

static void
noPERM(char *name)
{
    errno = EPERM;
    failed(name);
}

static void
noFILE(char *name)
{
    FPRINTF(stderr, "?? file not found: \"%s\"\n", name);
    exit(FAIL);
}

static void
WhoAmI(void)
{
    if (debug)
	show_uids(stdout);
}

/*
 * Cleanup handler
 */
static void
clean_file(void)
{
    if ((UidHack != NULL && *UidHack)
	&& (*Working != 0)
	&& strcmp(UidHack, Working)) {
	(void) unlink(UidHack);
    }
    UidHack = NULL;
}

static SIG_T
cleanup(int sig)
{
    (void) signal(sig, SIG_IGN);
    FPRINTF(stderr, "checkout: cleaning up\n\n");
    clean_file();
    (void) exit(FAIL);
}

static int
TestAccess(char *name, int flag)
{
    int code = access(name, flag);
    DEBUG((log_fp, ".. access(%s,%s) = %d\n", name, access_mode(flag), code));
    return code;
}

static char *
path_of(char *dst, char *src)
{
    char *s = strrchr(strcpy(dst, src), '/');
    if (s == NULL)
	(void) strcpy(dst, ".");
    else
	*s = EOS;
    return dst;
}

static void
GiveBack(int tell_why, const char *why)
{
    if (revert((tell_why || debug) ? why : (char *) 0)) {
	Effect = geteuid();
	WhoAmI();
    }
}

/*
 * Trim the portion of a version off that 'same_branch()' does not match.
 */
static void
trim_branch(char *s)
{
    char *t = strrchr(s, '.');
    if (t != NULL) {
	*t = EOS;
	if (strrchr(s, '.') != NULL) {
	    ;
	} else
	    *s = EOS;		/* trunk (i.e., "9.1") */
    } else
	*s = EOS;		/* trunk (i.e., "9") */
}

/*
 * Check two versions to see that they are on the same branch.
 */
static int
same_branch(char *a, char *b)
{
    char temp1[BUFSIZ], temp2[BUFSIZ];
    trim_branch(strcpy(temp1, a));
    trim_branch(strcpy(temp2, b));
    return !strcmp(temp1, temp2);
}

/*
 * Find the revision which the user has selected.
 */
static int
PreProcess(time_t *revtime,	/* date with which to touch file */
	   int *co_mode)
{
    int ok_vers = FALSE, ok_date = FALSE;
    char key[BUFSIZ];
    char tmp[BUFSIZ];
    char this_rev[REVSIZ];
    char *s = NULL;
    int header = TRUE, code = S_FAIL;

    if (!rcsopen(Archive, -debug, TRUE)) {
	FPRINTF(stderr, "? cannot open archive %s\n", Archive);
	return FALSE;
    }

    *rev_buffer = EOS;
    if (!EMPTY(opt_rev))
	(void) strcpy(rev_buffer, opt_rev);

    while (header && (s = rcsread(s, code))) {
	s = rcsparse_id(key, s);

	switch (code = rcskeys(key)) {
	    /*
	     * Begin an admin description.  If the user did not specify a
	     * revision, we assume the 'tip' version, unless he had one
	     * locked, in which case we assume that.
	     */
	case S_HEAD:
	    s = rcsparse_num(this_rev, s);
	    if (*rev_buffer == '\0')
		(void) strcpy(rev_buffer, this_rev);
	    break;
	case S_SYMBOLS:
	    s = rcssymbols(s, rev_buffer, rev_buffer);
	    break;
	case S_LOCKS:
	    /* see if this was locked by the user */
	    (void) strcpy(tmp, getuser());
	    *this_rev = EOS;
	    s = rcslocks(s, tmp, this_rev);
	    if (*this_rev && EMPTY(opt_rev)) {
		TELL(log_fp, "** revision %s is locked\n", this_rev);
		*co_mode |= S_IWRITE;
		(void) strcpy(rev_buffer, this_rev);
	    }
	    break;

	    /*
	     * Begin a delta description.  We are looking (like 'co') for
	     * the last version along a branch which matches the cutoff,
	     * state, and who-options.  Since the rcs file stores deltas
	     * in reverse-order, the first one we can match the options
	     * against is the right one.
	     */
	case S_VERS:
	    (void) sprintf(this_rev, "%.*s", REVSIZ - 1, key);
	    DEBUG((log_fp, "version = %s\n", this_rev));
	    ok_vers = same_branch(rev_buffer, this_rev);
	    ok_date = FALSE;
	    break;
	case S_DATE:
	    s = rcsparse_num(tmp, s);
	    if (ok_vers) {
		*revtime = rcs2time(tmp);
		DEBUG((log_fp, "date    = %s", ctime(revtime)));
		ok_date = ((opt_date == 0)
			   || (*revtime <= opt_date));
	    }
	    break;
	case S_AUTHOR:
	    s = rcsparse_id(key, s);
	    if (ok_vers && MISMATCH(opt_who, key))
		ok_vers = FALSE;
	    DEBUG((log_fp, "author  = %s\n", key));
	    break;
	case S_STATE:
	    s = rcsparse_id(key, s);
	    DEBUG((log_fp, "state   = %s\n", key));
	    if (ok_vers && MISMATCH(opt_sta, key))
		ok_vers = FALSE;
	    break;

	    /* 'next' is the last keyword in a delta description */
	case S_NEXT:
	    if (ok_vers && ok_date) {
		if (EMPTY(opt_rev)) {
		    if (strcmp(rev_buffer, this_rev)) {
			*co_mode &= ~S_IWRITE;
		    }
		    (void) strcpy(rev_buffer, this_rev);
		    header = FALSE;
		    break;
		}
		DEBUG((log_fp, "compare %s %s => %d (for equality)\n",
		       this_rev, rev_buffer,
		       vercmp(this_rev, rev_buffer, TRUE)));
		if (vercmp(this_rev, rev_buffer, TRUE) == 0) {
		    (void) strcpy(rev_buffer, this_rev);
		    header = FALSE;	/* force an exit */
		}
	    }
	    break;
	case S_DESC:
	    header = FALSE;
	}
	if (!s)
	    break;
    }
    rcsclose();

    if (!(ok_vers && ok_date)) {
	FPRINTF(stderr, "? cannot match requested revision\n");
	return FALSE;
    }
    return TRUE;
}

/*
 * Do the actual check-out.  For RCS version 5, we must always do this as admin,
 * since the 'ci' program gets confused by the apollo set-uid.
 */
static int
RcsCheckout(void)
{
    ARGV *args;
    const char *opt = to_stdout ? "-p" : (locked ? "-l" : "-r");
    int code = 0;

    args = argv_init2(CO_TOOL, rcspath(CO_TOOL));
#if RCS_VERSION >= 5
    argv_append(&args, "-M");
#endif
    if (silent)
	argv_append(&args, "-q");
    argv_append2(&args, opt, rev_buffer);
    if (!EMPTY(co_f_opt))
	argv_append(&args, co_f_opt);
    if (!EMPTY(co_k_opt))
	argv_append(&args, co_k_opt);
    argv_append(&args, UidHack);
    argv_append(&args, Archive);

    if (!silent || debug)
	show_argv2(log_fp, CO_TOOL, argv_values(args));
    if (!no_op) {
	Stat_t sb;
	if (Effect == 0) {
	    if (stat(Archive, &sb) < 0)
		sb.st_uid = sb.st_gid = 0;
	}
	code = executev(argv_values(args));
	if (Effect == 0
	    && Effect != sb.st_uid)
	    chown(Archive, sb.st_uid, sb.st_gid);
    }
    argv_free(&args);
    return (code);
}

/*
 * Check out the file using the RCS 'co' utility.  If 'co' does something, then
 * it will delete or modify the checked-in file.
 */
static void
Execute(time_t newtime, time_t oldtime, int co_mode)
{
    int code;
    Stat_t sb;
    long oldctime;
    int copied;

    UidHack = to_stdout ? "" : rcstemp(Working, FALSE);
    copied = strcmp(UidHack, Working);
    oldctime = 0;
    if (!copied && stat(UidHack, &sb) >= 0)
	oldctime = sb.st_ctime;

#if	RCS_VERSION >= 5
    if (saves_uid()) {
	code = RcsCheckout();
    } else {
	code = for_admin(RcsCheckout);
    }
#else
    code = RcsCheckout();
#endif

    if ((code >= 0) && !no_op && !to_stdout) {
	if (stat(UidHack, &sb) >= 0) {
	    DEBUG((log_fp, "=> file \"%s\"\n", UidHack));
	    DEBUG((log_fp, "=> size = %ld\n", (long) (sb.st_size)));
	    DEBUG((log_fp, "=> date = %s", ctime(&sb.st_mtime)));
	    DEBUG((log_fp, "..(comp)  %s", ctime(&oldtime)));
	    if (!copied && (sb.st_ctime == oldctime)) {
		TELL(log_fp, "** checkout was not performed\n");
	    } else {
		if (copied && (usercopy(UidHack, Working) < 0))
		    failed(Working);
		else if (userprot(Working, co_mode, newtime) < 0)
		    noPERM(Working);
	    }
	}
    }
    clean_file();
}

/*
 * Process a single file.
 *
 * Before checkout, verify that the file exists, and obtain its modification
 * time/date.  Also (since this looks at the working/archive files before
 * invoking 'co'), see if we have nominal permissions to work upon the files:
 *	a) If the working file exists, we need rights to recreate it with the
 *	   same ownership.  If it does not exist, we assume that we create it
 *	   with the ownership implied from the directory.
 *	b) We need rights to recreate the archive file with its current
 *	   ownership -- this implies that our effective uid must be the same
 *	   as that of the archive.
 *
 * patch: this does not address the need to down-adjust privilege if we start
 *	out with root-uid.
 */
static void
do_file(void)
{
    Stat_t sb;
    int ok;
    char temp[MAXPATHLEN];
    time_t revtime;
    int co_mode;		/* mode with which 'co' sets file */

    /*
     * Ensure that we can find the RCS-file.  Note that this program may be
     * setuid'd to a user who has no rights in the working directory...
     */
    ok = (rcs_archive(Archive, &sb) >= 0);
    if (!ok && (errno != EISDIR)) {
	if (TestAccess(Archive, R_OK) == 0) {
	    GiveBack(FALSE, "directory access");
	    ok = (rcs_archive(Archive, &sb) >= 0);
	}
    }
    if (!ok)
	noFILE(Archive);

    co_mode = (sb.st_mode & 0777);
    if (locked)
	co_mode |= S_IWRITE;
    else
	co_mode &= 0555;	/* strip writeable mode */
    if (!co_mode)
	co_mode = 0400;		/* leave at least readonly! */

    /*
     * If we have the file, ensure that we have proper access:
     */
    if (Effect != Caller) {
	if (locked
	    && (TestAccess(path_of(temp, Archive), W_OK) >= 0))
	    GiveBack(FALSE, "RCS-directory is writable");
	else if (!rcspermit(temp, (char *) 0, (const char **) 0))
	    GiveBack(Effect && locked, "not listed in permit-file");
    }

    /*
     * Check to see if the working-file exists
     */
    if (rcs_working(Working, &sb) >= 0) {
	DEBUG((log_fp, "=> date = %s", ctime(&sb.st_mtime)));
    } else if (errno == EISDIR) {
	noFILE(Working);
    }

    if (!locked && TestAccess(Archive, R_OK) >= 0)
	GiveBack(FALSE, "normal rights suffice");

    if (PreProcess(&revtime, &co_mode))
	Execute(revtime, sb.st_mtime, co_mode);
}

static void
usage(void)
{
    static const char *tbl[] =
    {
	"Usage: checkout [options] [working_or_archive [...]]"
	,""
	,"Options (from \"co\"):"
	,"  -l[rev]  locks the checked-out revision for the caller."
	,"  -p[rev]  prints the revision on standout-output"
	,"  -q[rev]  quiet mode"
	,"  -r[rev]  retrieves the latest revision whose number is less than or equal"
	,"           to \"rev\"."
	,"  -cdate   retrieves the latest revision on the selected branch whose checkin"
	,"           date/time is less than or equal to \"date\", in the format"
	,"                   yy/mm/dd hh:mm:ss"
	,"  -sstate  retrieves the latest revision on the selected branch whose state"
	,"           is set to state."
	,"  -w[login] retrieves the latest revision on the selected branch which was"
	,"           checked in by user \"login\"."
	,""
	,"Unimplemented \"co\" options:"
	,"  -jjoinlist generates a new revision which is the join of the revisions on"
	,"           joinlist"
	,""
	,"Special:"
	,"  -D       debug/no-op"
    };
    unsigned j;
    for (j = 0; j < sizeof(tbl) / sizeof(tbl[0]); j++)
	FPRINTF(stderr, "%s\n", tbl[j]);
    FFLUSH(stderr);
    (void) exit(FAIL);
}

/************************************************************************
 *	main program							*
 ************************************************************************/
/*ARGSUSED*/
_MAIN
{
    int j;
    char *s;

    char who[BUFSIZ], tmp[BUFSIZ];
    int code;

    debug = RCS_DEBUG;
    Caller = getuid();
    Effect = geteuid();
    if (Caller != Effect)
	catchall(cleanup);

    log_fp = stdout;
    WhoAmI();

    /* process options */
    for (j = 1; (j < argc) && (*(s = argv[j]) == '-'); j++) {

	(void) strcpy(tmp, s++);
	code = *s++;

	switch (code) {
	case 'D':
	    no_op++;
	    break;

	case 'c':
	    if (*s != EOS) {
		optind = j;
		argv[j] = s;
		optarg = s;
	    } else {
		optind = j + 1;
		optarg = argv[optind];
	    }
	    opt_date = cutoff(argc, argv);
	    j = optind - 1;
	    FORMAT(tmp, "-d%s", ctime(&opt_date));
	    (void) strtrim(tmp);
	    TELL(log_fp, "++ cutoff: %s", ctime(&opt_date));
	    break;

	case 'f':
	    co_f_opt = argv[j];
	    break;

	case 'k':
	    co_k_opt = argv[j];
	    break;

	case 'q':
	    silent++;
	    if (!EMPTY(s))
		opt_rev = s;
	    break;

	case 'l':
	    locked++;
	    /* FALL-THROUGH */
	case 'r':
	    if (!EMPTY(s))
		opt_rev = s;
	    break;

	case 'p':
	    to_stdout = TRUE;
	    log_fp = stderr;
	    if (!EMPTY(s))
		opt_rev = s;
	    break;

	case 's':
	    if (!EMPTY(s))
		opt_sta = s;
	    else
		usage();
	    DEBUG((log_fp, ">state:%s\n", opt_sta));
	    break;

	case 'w':
	    if (!EMPTY(s))
		opt_who = s;
	    else
		opt_who = strcpy(who, getuser());
	    DEBUG((log_fp, ">who:%s\n", opt_who));
	    break;

	default:
	    FPRINTF(stderr, "?? Unknown option: %s\n", argv[j]);
	    usage();
	}
    }
    DEBUG((log_fp, ">rev:%s\n", opt_rev));

    /* process files */
    if (j < argc) {
	while (j < argc) {
	    j = rcsargpair(j, argc, argv);
	    do_file();
	}
    } else {
	FPRINTF(stderr, "? expected a filename\n");
	usage();
    }

    (void) exit(SUCCESS);
    /*NOTREACHED */
}
