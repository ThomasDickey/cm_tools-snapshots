/*
 * Title:	checkin.c (RCS checkin front-end)
 * Author:	T.E.Dickey
 * Created:	19 May 1988, from 'sccsbase'
 * Modified:
 *		05 Dec 2019, use DYN-argv lists.
 *		13 Jan 2012, add -M option to allow check-in message from file.
 *		23 Sep 1993, gcc warnings
 *		11 Nov 1992, initialize access-list of each module to match
 *			     the permit-list.
 *		05 Nov 1992, added "-w" option to simplify apollo setuid code.
 *		22 Oct 1992, mods to accommodate RCS version 5.
 *		17 Jul 1992, if no lock found, don't exit early from GetLock!
 *		10 Feb 1992, change "-d" to "-D".  Make this recognize symbolic
 *			     revisions and branches.
 *		07 Feb 1992, use dynamic-strings for building arg-lists.
 *		05 Feb 1992, revised filename-parsing with 'rcsargpair()',
 *			     made "-x" obsolete.
 *		04 Feb 1992, allow user to pipe "-m" text to this program.
 *		17 Jan 1992, "-k" and "-d" options of 'ci' are conflicting.
 *		25 Oct 1991, 'tmpfile()' resets effective uid/gid (don't use!)
 *		24 Oct 1991, corrected exit-condition in 'GetLock()' - was
 *			     reading past header.  Also, test for "-f" option in
 *			     'GetLock()' to ensure that we pick up lock_date
 *			     even if no locks were made.
 *		22 Oct 1991, corrected initial-access if root-uid (should be the
 *			     owner of the directory, not "root").
 *		18 Oct 1991, corrected conditions under which mismatch between
 *			     effect/real group-id causes reversion.  Also, fixed
 *			     so that we don't call HackMode for set-access.
 *		15 Oct 1991, debug-traces for uid/gid
 *		11 Oct 1991, must not use "-d" option generally, since 'ci' will
 *			     not check in files which have no change in date.
 *			     Converted to ANSI.
 *		07 Oct 1991, use "-d" option available in RCS 4.x to set the
 *			     checkin date w/o having to edit the archive.
 *		01 Oct 1991, if baseline was made since last version, and no
 *			     "-r" option specified, use RCS_BASE.  Also, if
 *			     running as root-set-uid  don't change mode of the
 *			     directories. Added "-B" option.
 *		06 Sep 1991, modified interface to 'rcsopen()'
 *		20 Jun 1991, use 'shoarg()'
 *		19 Apr 1990, Added "-d" option (for want of a better code for
 *			     no-op).  Used this option to suppress invocation
 *			     of ci/rcs/mkdir/chmod and unlink (also changed
 *			     "TELL" for these calls) so user can preview the
 *			     actions which would be attempted.  Note that the
 *			     no-op mode cannot (as yet) do anything interesting
 *			     for the set-uid mode.  Used the no-op mode to find
 *			     that GetLock was not returning proper code (since
 *			     'strict' was set after 'locks'!).  Fixed this.
 *			     Finally, recoded the "rcs -c" stuff so that user
 *			     can add to the default list of comment-prefixes
 *			     by setting the environment variable RCS_COMMENT
 *			     appropriately.
 *		05 Mar 1990, port to sun3 (os3.4) which has bug in tmpfile &
 *			     mktemp.  Cleanup error-exits.
 *		06 Dec 1989, if option "-?" given, don't print warning before
 *			     usage, so we can invoke checkin from rcsput for
 *			     combined-usage.
 *		21 Sep 1989, added "rcs -c" decoding for IMakefile, AMakefile,
 *			     changed the decoding for ".com"
 *		04 Apr 1989, ensure that we call 'rcspermit()' not only to
 *			     check permissions, but also to obtain value for
 *			     RCSbase variable.
 *		31 Mar 1989, only close temp-file if we have opened it!
 *		29 Mar 1989, if working file cannot be found, this may be
 *			     because checkin is running in set-uid mode. 
 *			     Revert to normal rights and try again.
 *		21 Mar 1989, after invoking 'revert()', could no longer write to
 *			     temp-file (fpT); moved 'tmpfile()' call to fix.
 *		15 Mar 1989, if no tip-version found, assume we can create
 *			     initial revision.
 *		08 Mar 1989, use 'revert()' and 'rcspermit()' to implement
 *			     CM-restrictions to set-uid 
 *		27 Feb 1989, Set comment for VMS filetypes .COM, .MMS, as well
 *			     as unix+VMS types ".mk" and ".e".  Also, test for
 *			     the special case of Makefile/makefile.
 *		06 Dec 1988, corrected handling of group-restricted archives.
 *			     corrected setting of RCSprot -- used in HackMode().
 *		28 Sep 1988, use $RCS_DEBUG to control debug-trace.
 *		27 Sep 1988, forgot to make "rcs" utility perform "-t" option.
 *		13 Sep 1988, for ADA-files (".a" or ".ada", added rcs's comment
 *			     header (not in rcs's default table).  Catch signals
 *			     to do cleanup.  Correctly 'rm_work()' for set-uid 
 *			     Added newtime/oldtime logic to cover up case in
 *			     which 'ci' aborts but does not return error.
 *		09 Sep 1988, use 'rcspath()'
 *		30 Aug 1988, make this work as a set-uid process.  If file has
 *			     not been checked-in, initialize its access list
 *			     first.
 *		24 Aug 1988, added 'usage()'; create directory if not found.
 *		15 Aug 1988, If "-k" option is used, assume time+date from the
 *			     RCS file, not from the working file.  Also, supply
 *			     default log-message for "-k" option.
 *		05 Aug 1988, revised interface with 'rcsname.c' module.
 *		08 Jul 1988, set "silent" before first call on GetLock if "-q"
 *			     precedes filename in argv.
 *		01 Jul 1988, added chmod to fix cases in which 'ci' leaves the
 *			     file writeable (Apollo bug?).  Added interpretation
 *			     for environment variable which specifies base
 *			     value for revision if none is given in the options
 *			     list (i.e., RCS_BASE).
 *		27 May 1988, recoded using 'rcsedit' module.
 *		21 May 1988, broke out common routines for 'checkout.c'.
 *
 * Function:	Invoke RCS checkin 'ci', then modify the last delta-date of the
 *		corresponding RCS-file to be the same as the modification date
 *		of the file.  The RCS-files are assumed to be in the standard
 *		location:
 *
 *			name => RCS/name,v
 *
 * patch:	Since 'ci' uses 'access()' to verify that it can put a semaphore
 *		in the RCS directory, I don't see any simple way of handling
 *		this except to make the RCS directory temporarily publicly
 *		writeable ...
 *
 * patch:	on apollo sr10.3, RCS release 4 does not unlock a file when I
 *		create a branch version.
 */

#define		ACC_PTYPES
#define		SIG_PTYPES
#define		STR_PTYPES
#include	<ptypes.h>
#include	<rcsdefs.h>
#include	<dyn_str.h>

#include	<ctype.h>
#include	<pwd.h>
#include	<signal.h>
#include	<time.h>
#include	<errno.h>
extern char *mktemp(char *);

MODULE_ID("$Id: checkin.c,v 11.35 2019/12/06 00:18:44 tom Exp $")

/* local declarations: */
#define	CI_TOOL		"ci"
#define	RCS_TOOL	"rcs"

#define	REVSIZ	80
#define	EMPTY(s)	(*(s) == EOS)

#define	REV_OPT	"rfluqk"
#define	is_option(s)	(*(s) == '-')
#define	is_m_opt(s)	(s[1] == 'm')
#define	is_t_opt(s)	(s[1] == 't')
#define	is_my_opt(s)	(s[1] == 'B' || s[1] == 'D' || s[1] == 'M')

#define	TELL	if (!silent || debug) PRINTF
#define	DEBUG(s)	if (debug) PRINTF s

#if RCS_VERSION >= 4
#if RCS_VERSION >= 5
#define	if_D_option(s)	s;	/* "-M" option always needs "-d" */
#else
#define	if_D_option(s)	if (lock_date < modtime) s;
#endif
#else
#define	if_D_option(s)
#endif

static int silent = FALSE, debug;	/* set by RCS_DEBUG environment */
static int no_op;		/* show, but don't do */
static int use_base = TRUE;	/* use RCS_BASE for base-version */
static int u_or_l = FALSE;	/* set if file is re-locked */
static int w_opts;		/* set iff we used "-w" option */
static int from_keys = FALSE;	/* set if we get date from RCS file */
static mode_t TMP_mode;		/* saved protection of RCS-directory */
static uid_t HIS_uid;		/* working-file's owner */
static gid_t HIS_gid;		/* working-file's group */
static uid_t RCS_uid;		/* archive's owner */
static gid_t RCS_gid;		/* archive's group */
static mode_t RCSprot;		/* protection of RCS-directory */
static time_t modtime;		/* timestamp of working file */

#if RCS_VERSION <= 4
static time_t lock_date;
static time_t oldtime;		/* timestamp of archive file */
#endif

static ARGV *opt_all;		/* options for 'ci' */

static char Working[MAXPATHLEN];
static char Archive[MAXPATHLEN];
static char *TMP_file;		/* temp-file used to fix uid/access */
static char *t_option;		/* "-t" option used for "ci" or "rcs" */
static char *cat_input;		/* "-m" text, from pipe, or "-M" from file */
static char RCSdir[MAXPATHLEN];
static char RCSbase[REVSIZ];	/* base+ version number */
static const char *RCSaccess;	/* last permission-list */
static char opt_opt[3];
static char opt_rev[REVSIZ];

#if RCS_VERSION <= 4
static char old_date[BUFSIZ],	/* subprocess variables */
  new_date[BUFSIZ];
#endif

/************************************************************************
 *	local procedures						*
 ************************************************************************/

static void
GiveUp(const char *msg, const char *arg)
{
    TELL("?? %s \"%s\"\n", msg, arg);
    (void) exit(FAIL);
}

static void
WhoAmI(void)
{
    if (debug)
	show_uids(stdout);
}

static void
GiveBack(const char *why)
{
    if (revert(debug ? why : (char *) 0))
	WhoAmI();
}

/*
 * Remove working-file for set-uid process if the matching temp-file was.
 */
static void
rm_work(void)
{
    (void) unlink(Working);
}

static void
clean_file(void)
{
    if ((*Working != 0)
	&& (TMP_file != 0)
	&& strcmp(Working, TMP_file)) {
	TELL("%% rm -f %s\n", TMP_file);
	(void) unlink(TMP_file);
    }
    TMP_file = 0;
}

static void
ChangeProt(char *name, mode_t mode)
{
    TELL("%% chmod %#o %s\n", mode, name);
    if (!no_op)
	if (chmod(name, mode) < 0)
	    GiveUp("Could not change mode", name);
}

/*
 * The RCS utilities (ci and rcs) use the unix 'access()' function to test if
 * the caller has access to the archive-directory.  This is not done properly,
 * (particularly for non-root set-uid  but I cannot easily code around it other
 * than by temporarily changing the directory's protection.
 */
static void
HackMode(int save)
{
    if (!geteuid() || saves_uid()) {
	;
    } else if (save) {
	if ((RCSdir[0] != EOS)
	    && (getuid() != RCS_uid)) {
	    /* patch: check if group is compatible */
	    mode_t need = (getgid() == RCS_gid) ? 0775 : 0777;
	    DEBUG(("...need mode %04o for %s, was %04o\n",
		   need, RCSdir, RCSprot));
	    if (need != RCSprot) {
		TMP_mode = RCSprot | 01000;
		ChangeProt(RCSdir, need);
	    }
	}
    } else if (TMP_mode) {
	TMP_mode &= 0777;
	ChangeProt(RCSdir, TMP_mode & 0777);
	TMP_mode = 0;
    }
}

/*
 * If interrupted, clean up and exit
 */
static
SIGNAL_FUNC(cleanup)
{
    static int latch;
    if (!latch++) {
	(void) signal(sig, SIG_IGN);
	if (TMP_file)
	    clean_file();
	if (TMP_mode)
	    HackMode(FALSE);
	FPRINTF(stderr, "checkin: cleaned up\n\n");
    }
    (void) exit(FAIL);
    /*NOTREACHED */
}

/*
 * Restore ownership of a file to the "natural" owner after a check-in
 */
static void
FixOwnership(char *name, uid_t uid, gid_t gid)
{
    if (!geteuid()) {
	DEBUG(("...fix ownership of %s\n", name));
	WhoAmI();
	DEBUG(("%% chown %s %s\n", uid2s(uid), name));
	DEBUG(("%% chgrp %s %s\n", gid2s(gid), name));
	if (!no_op)
	    (void) chown(name, uid, gid);
    }
}

static void
OwnWorking(void)
{
    FixOwnership(Working, HIS_uid, HIS_gid);
}

static void
OwnArchive(void)
{
    FixOwnership(Archive, RCS_uid, RCS_gid);
}

/*
 * If we could not persuade 'ci' to check-in the file with "-d", filter a copy
 * of it so that the date-portion of the identifiers is set properly.
 */
#if RCS_VERSION <= 4
static int
Filter(void)
{
    FILE *fpS, *fpT;
    size_t len = strlen(old_date);
    int changed = 0;		/* number of substitutions done */
    char *s, *d;
    char name[MAXPATHLEN];
    char bfr[BUFSIZ];
    int lines = 0;

    if_D_option(return)

	if (from_keys || !len || !(fpS = fopen(Working, "r"))) {
	errno = 0;
	return;
    }

    /* Alter the delta-header to match RCS's substitution */
    /* 0123456789.123456789. */
    /* yy.mm.dd.hh.mm.ss */
    new_date[2] = old_date[2] =
	new_date[5] = old_date[5] = '/';
    new_date[8] = old_date[8] = ' ';
    new_date[11] = old_date[11] =
	new_date[14] = old_date[14] = ':';

    if (!(fpT = fopen(TMP_file = strcat(strcpy(name, Working), ",,"), "w")))
	GiveUp("Could not create tmp-file", name);

    DEBUG(("...filtering date-strings to %s\n", new_date));
    while (fgets(bfr, sizeof(bfr), fpS)) {
	char *last = bfr + strlen(bfr) - len;

	lines += 1;
	if ((last > bfr)
	    && (d = strchr(bfr, '$'))) {
	    while (d <= last) {
		if (!strncmp(d, old_date, len)) {
		    DEBUG(("...edit date at line %d\n%s",
			   lines, bfr));
		    for (s = new_date; *s; s++)
			*d++ = *s;
		    changed++;
		}
		d++;
	    }
	}
	(void) fputs(bfr, fpT);
    }
    FCLOSE(fpS);

    if (changed) {
	if (rename(name, Working) < 0)
	    GiveUp("Could not rename", name);;
    } else
	(void) unlink(name);
    TMP_file = 0;
    FCLOSE(fpT);
}

/*
 * If the given file is still checked-out, touch its time.
 *
 * patch: should do keyword substitution for Header, Date a la 'co'.
 */
static void
ReProcess(void)
{
    Stat_t sb;
    int mode;

    if (stat_file(Working, &sb) >= 0) {

	mode = sb.st_mode & 0777;

	DEBUG(("...reprocessing %s\n", Working));

	OwnWorking();
	if (for_user(Filter) < 0)
	    GiveUp("cannot reprocess", Working);
	if (u_or_l < 0)		/* cover up bugs on Apollo acls */
	    mode &= ~0222;

	DEBUG(("%% chmod %03o %s\n", mode, Working));
	if (userprot(Working, mode, modtime) < 0)
	    GiveUp("touch failed", Working);
    }
}

/*
 * Post-process a single RCS-file, replacing its check-in date with the file's
 * modification date.
 */
static void
PostProcess(void)
{
    int header = TRUE, match = FALSE, changed = FALSE, code = S_FAIL;
    char *s = 0, token[BUFSIZ];

    if_D_option(return)

	* old_date = EOS;

    if (!rcsopen(Archive, debug, FALSE))
	return;

    while (header && (s = rcsread(s, code))) {
	s = rcsparse_id(token, s);

	switch (code = rcskeys(token)) {
	case S_HEAD:
	    s = rcsparse_num(token, s);
	    break;
	case S_VERS:
	    if (dotcmp(token, opt_rev) < 0)
		header = FALSE;
	    match = !strcmp(token, opt_rev);
	    break;
	case S_DATE:
	    if (!match)
		break;
	    s = rcsparse_num(old_date, s);
	    if (*old_date) {
		if (from_keys) {
		    modtime = rcs2time(old_date);
		} else {
		    time2rcs(new_date, modtime);
		    rcsedit(old_date, new_date);
		}
		TELL("** revision %s\n", opt_rev);
		TELL("** modified %s", ctime(&modtime));
		changed++;
	    }
	    break;
	case S_DESC:
	    header = FALSE;
	}
    }
    rcsclose();
    if (changed || no_op)
	OwnArchive();
}
#endif /* RCS_VERSION <= 4 */

/*
 * RCS 4 does not check permissions properly on the RCS directory even when
 * running in set-uid mode (sigh).
 */
static int
DoIt(ARGV *args)
{
    int code;
#if defined(HAVE_SETRUID)
    int fix_id;

    if ((fix_id = (!geteuid() && getuid())) != 0) {
	if (!no_op) {
	    (void) setruid(geteuid());
	    (void) setrgid(getegid());
	}
	WhoAmI();
    }
#endif

    if (!silent || debug)
	show_argv(stdout, argv_values(args));
    code = no_op ? 0 : executev(argv_values(args));

#if defined(HAVE_SETRUID)
    if (!no_op && fix_id) {
	(void) setruid(HIS_uid);
	(void) setrgid(HIS_gid);
    }
#endif
    return code;
}

/*
 * Do the actual check-in.  For RCS version 5, we must always do this as admin,
 * since the 'ci' program gets confused by the apollo set-uid.
 */
static int
RcsCheckin(void)
{
    ARGV *args;
    int result = 0;

    args = argv_init1(CI_TOOL);
#if RCS_VERSION >= 5
    argv_append(&args, "-M");
#endif
    if (!from_keys || !saves_uid()) {
	if (!w_opts) {
	    argv_append2(&args, "-w", uid2s(HIS_uid));
	}
    }
    argv_merge(&args, opt_all);
    if (EMPTY(opt_rev)) {
	argv_append(&args, opt_opt);
    } else {
	argv_append2(&args, opt_opt, opt_rev);
    }
    argv_append(&args, Archive);
    argv_append(&args, TMP_file);

    if (DoIt(args) < 0)
#if RCS_VERSION >= 5
	GiveUp("rcs checkin for", Working);
#else
	result = -1;
#endif
    return result;
}

/*
 * Check in the file using the RCS 'ci' utility.  If 'ci' does something, then
 * it will delete or modify the checked-in file -- return TRUE.  If no action
 * is taken, return false.
 */
static int
Execute(void)
{
    Stat_t sb;
    int code;

    HackMode(TRUE);
    TMP_file = rcstemp(Working, TRUE);

#if RCS_VERSION >= 5
    if (saves_uid())
	code = RcsCheckin();
    else
	code = for_admin(RcsCheckin);
#else
    code = RcsCheckin();
#endif
    if (code >= 0 || no_op)
	OwnArchive();

    HackMode(FALSE);		/* ...restore protection */

    if (no_op) ;
    else if (code >= 0) {	/* ... check-in file ok */
	if (stat(TMP_file, &sb) >= 0) {		/* working file not deleted */
#if RCS_VERSION >= 5
	    static int modified = TRUE;
#else
	    int modified = (sb.st_mtime != modtime);
#endif
	    if (strcmp(TMP_file, Working)) {
		if (modified) {
		    if (usercopy(TMP_file, Working) < 0)
			GiveUp("Copy to userfile", Working);
		    DEBUG(("...copied %s to %s\n",
			   TMP_file, Working));
		}
	    }
	    if (!modified) {
		clean_file();
		DEBUG(("...working file was not modified\n"));
		return (FALSE);
	    }
	} else if (strcmp(TMP_file, Working)
		   && for_user(rm_work) < 0)
	    GiveUp("Deletion of working-file", Working);

	clean_file();
	return (TRUE);
    }
    clean_file();
    return (FALSE);
}

/*
 * This procedure is invoked only if we find an RCS-archive.
 * Ensure that we have a unique lock-value.  If the user did not specify one,
 * we must get it from the archived-file.
 *
 * returns:
 *	+1 - a lock is present
 *	 0 - no lock was set (or can be forced)
 */
#if RCS_VERSION <= 4
static int
GetLock(void)
{
    int done = FALSE;
    int strict = FALSE;
    int implied = EMPTY(opt_rev);
    int match;
    int code = S_FAIL;

    char *s = 0;
    char lock_rev[REVSIZ];
    char head_rev[REVSIZ];
    char next_rev[REVSIZ];
    char lock_by[BUFSIZ];
    char key[BUFSIZ];

    (void) strcpy(lock_by, getuser());

    *head_rev =
	*next_rev =
	*lock_rev = EOS;

    if (!rcsopen(Archive, -debug, TRUE))
	return (FALSE);		/* could not open file anyway */

    while (!done && (s = rcsread(s, code))) {
	s = rcsparse_id(key, s);

	switch (code = rcskeys(key)) {
	    /*
	     * Begin the admin-node
	     */
	case S_HEAD:
	    s = rcsparse_num(head_rev, s);
	    if (EMPTY(head_rev))
		done = TRUE;

	    DEBUG(("...GetLock tip = %s\n", head_rev));
	    DEBUG(("...(use-base:%d, compare to base \"%s\":%d)\n",
		   use_base,
		   RCSbase,
		   vercmp(RCSbase, head_rev, FALSE)));

	    if (vercmp(RCSbase, head_rev, FALSE) < 0)
		next_version(next_rev, head_rev);
	    else
		(void) strcpy(next_rev, RCSbase);
	    DEBUG(("=>%s (next)\n", next_rev));
	    break;

	case S_STRICT:
	    strict = TRUE;
	    break;

	case S_SYMBOLS:
	    s = rcssymbols(s, opt_rev, opt_rev);
	    break;

	case S_LOCKS:
	    s = rcslocks(s, lock_by, lock_rev);

	    if (EMPTY(lock_rev)) {
		if (implied && !from_keys)
		    (void) strcpy(opt_rev, next_rev);
		break;
	    }

	    TELL("** revision %s was locked\n", lock_rev);
	    DEBUG(("...(use-base:%d, compare to base \"%s\":%d)\n",
		   use_base,
		   RCSbase,
		   vercmp(RCSbase, lock_rev, FALSE)));

	    if (!strcmp(head_rev, lock_rev)) {	/* trunk */
		if (!implied) ;
		else if (use_base
			 && (vercmp(RCSbase, lock_rev, FALSE) > 0))
		    (void) strcpy(opt_rev, RCSbase);
		else
		    next_version(opt_rev, lock_rev);
	    } else if (implied) {
		next_version(opt_rev, lock_rev);
	    } else {		/* branch */
		int count = strcount(opt_rev, '.');
		while (count < 2) {
		    count++;
		    (void) strcat(opt_rev, ".1");
		}
	    }
	    if (!(strcount(opt_rev, '.') % 2))
		(void) strcat(opt_rev, ".1");
	    DEBUG(("=>%s (resulting version)\n", opt_rev));
	    break;

	    /*
	     * Begin a delta-description
	     */
	case S_VERS:
	    match = EMPTY(lock_rev)
		? TRUE
		: !strcmp(key, lock_rev);
	    break;
	case S_DATE:
	    if (match) {
		time_t at;
		s = rcsparse_num(key, s);
		at = rcs2time(key);
		/* if no lock, choose highest date */
		if (EMPTY(lock_rev)) {
		    if (lock_date < at)
			lock_date = at;
		} else
		    lock_date = at;
		done = TRUE;
	    }
	    break;

	    /*
	     * Begin the delta-contents
	     */
	case S_AUTHOR:
	case S_STATE:
	case S_BRANCHES:
	case S_DESC:
	case S_LOG:
	case S_TEXT:
	    done = TRUE;
	    break;

	default:
	    DEBUG(("undecoded case %s in %s, line %d\n",
		   key, __FILE__, __LINE__));
	case S_BRANCH:
	case S_ACCESS:
	case S_COMMENT:
	    break;
	}
    }
    rcsclose();

    if (strict && EMPTY(lock_rev))
	TELL("?? no lock set by %s\n", lock_by);
    DEBUG(("=> lock_date = %s",
	   lock_date != 0 ? ctime(&lock_date) : "(none)\n"));

    return (!strict || !EMPTY(opt_rev));
}
#endif /* RCS_VERSION <= 4 */

/*
 * Before checkin, verify that the file exists, and obtain its modification
 * time/date.
 */
static time_t
DateOf(char *name)
{
    Stat_t sb;

    DEBUG(("...DateOf(%s)\n", name));
    if (stat_file(name, &sb) >= 0) {
	DEBUG(("=> date = %s", ctime(&sb.st_mtime)));
	return (sb.st_mtime);
    } else if (errno == EISDIR) {
	GiveUp("not a file:", Working);
    }
    DEBUG(("=> not found, errno=%d\n", errno));
    return (0);
}

/*
 * For "rcs -i" command, define/override 'ci' table for comment-prefix for the
 * Log-keyword.  Use the environment variable RCS_COMMENT to permit user-
 * defined conversions (note that RCS_COMMENT is used in our local mod to 'ci'
 * as well, for prefixing the prompt-lines).  We expect an auto-delimited list
 * such as
 *	"/.c/ * -- /,/.d/>>/"
 * in which the delimiter can change for each item in the list.
 */
typedef struct _prefix {
    struct _prefix *link;
    char *suffix, *prefix;
} PREFIX;

static PREFIX *clist;

static void
define_prefix(const char *suffix, const char *prefix)
{
    char tmp[BUFSIZ];
    PREFIX *new = ALLOC(PREFIX, 1);

    new->link = clist;
    new->suffix = stralloc(suffix);
    new->prefix = stralloc(strcat(strcpy(tmp, "-c"), prefix));
    clist = new;
} static

const char *
copy_to(char *dst, const char *src)
{
    char delim;
    if ((delim = *src) != EOS) {
	src++;
	while (*src != delim) {
	    if (!(*dst++ = *src))	/* look out for EOS ! */
		break;
	    src++;
	}
    }
    *dst = EOS;
    return (src);
}

static const char *
get_prefix(const char *suffix)
{
    PREFIX *new;
    for (new = clist; new != 0; new = new->link) {
	if (!strcmp(new->suffix, suffix))
	    return (new->prefix);
    }
    return (0);
}

/*
 * If the RCS archive does not already exist, make one, with an access list
 * properly initialized (i.e., including the current user and the owner of the
 * RCS directory.  Returns FALSE if the archive already exists.
 */
static int
RcsInitialize(void)
{
    ARGV *args;
    static char list[BUFSIZ];	/* static so we do list once */
    const char *s;
    char *t;

    if (clist == 0) {
	define_prefix(".a", "--  ");
	define_prefix(".ada", "--  ");
	define_prefix(".com", "$!\t");
	define_prefix(".e", " * ");
	define_prefix(".mms", "#\t");
	define_prefix(".mk", "#\t");
	if ((s = getenv("RCS_COMMENT")) != NULL) {
	    char suffix[BUFSIZ], prefix[BUFSIZ];
	    while (*s) {
		s = copy_to(suffix, s);
		s = copy_to(prefix, s);
		if (*s)
		    s++;
		define_prefix(suffix, prefix);
		while (*s == ',' || isspace(UCH(*s)))
		    s++;
	    }
	}
    }

    args = argv_init1(RCS_TOOL);

    argv_append(&args, t_option);
    argv_append(&args, "-i");

    if ((s = RCSaccess) && !EMPTY(s))
	argv_append2(&args, "-a", s);
    else {
	if (EMPTY(list)) {
	    struct passwd *p;

	    if ((p = getpwuid(geteuid()? geteuid() : RCS_uid)) != 0)
		FORMAT(list, "-a%s", p->pw_name);
	    else
		GiveUp("owner of RCS directory for", Working);

	    if (getuid() != HIS_uid) {
		if ((p = getpwuid(HIS_uid)) != 0)
		    FORMAT(list + strlen(list), ",%s", p->pw_name);
		else
		    GiveUp("owner of directory for", Working);
	    }
	}
	argv_append(&args, list);
    }

    s = ftype(t = pathleaf(Working));
    if (!*s && (!strcmp(t, "Makefile")
		|| !strcmp(t, "IMakefile")
		|| !strcmp(t, "AMakefile")
		|| !strcmp(t, "makefile")))
	argv_append(&args, "-c#\t");
    else if ((s = get_prefix(s)) != NULL)
	argv_append(&args, s);

    if (silent)
	argv_append(&args, "-q");
    argv_append(&args, Archive);
    if (DoIt(args) < 0)
	GiveUp("rcs initialization for", Working);
    return 0;
}

/*
 * If the RCS-directory does not exist, make it.  Save the name in 'RCSdir[]'
 * in case we need it for the set-uid hack in 'Execute()'.
 */
static void
MakeDirectory(void)
{
    Stat_t sb;
    size_t len = strlen(RCSdir);

    if (len != 0
	&& len < strlen(Archive)
	&& Archive[len] == '/'
	&& !strncmp(RCSdir, Archive, len - 1)) {

	DEBUG(("...same %s-directory\n", rcs_dir(NULL, NULL)));
	return;

    } else if (rcs_located(RCSdir, &sb) < 0) {

	if (access(RCSdir, W_OK) >= 0) {
	    GiveBack("RCS-dir is accessible by user");
	    if (rcs_located(RCSdir, &sb) < 0)
		failed(RCSdir);
	} else {

	    GiveBack("new user directory");
	    RCS_uid = getuid();
	    RCS_gid = getgid();
	    RCSprot = 0755;
	    TELL("%% mkdir %s\n", RCSdir);

	    if (!no_op) {
		mode_t omask = umask(0);
		if (mkdir(RCSdir, RCSprot) < 0)
		    GiveUp("directory-create", RCSdir);
		(void) umask(omask);
	    }
	    return;
	}

    } else {

#if defined(HAVE_GETEUID) && defined(HAVE_SETEGID)
	if (getegid() != sb.st_gid) {
	    (void) setegid(sb.st_gid);
	    WhoAmI();
	}

	if (geteuid() != 0
	    && sb.st_uid != geteuid())
	    GiveBack("non-CM use");
#endif

    }

    /*
     * The RCS-directory already exists, and is not the same as one which
     * was previously processed.  Check the derived permissions.
     */
    if (!rcspermit(RCSdir, RCSbase, &RCSaccess))
	GiveBack(geteuid()
		 ? "not listed in permit-file"
		 : (char *) 0);

    DEBUG((".. RCSbase='%s'\n", RCSbase));
    RCS_uid = sb.st_uid;
    RCS_gid = sb.st_gid;
    RCSprot = sb.st_mode & 0777;
}

/*
 * Generate the argv-list 'opt_all[]' which we will pass to 'ci' for options.
 * We may have to generate it before each file because some are initial
 * checkins.
 *
 * For initial checkins we have a special case: if the environment variable
 * RCS_BASE is set, we use this for the revision code rather than "1.1".
 */
static void
SetOpts(int argc, char **argv, int new_file)
{
    const char *m_option = ((cat_input != 0)
			    ? cat_input
			    : (from_keys
			       ? "FROM_KEYS"
			       : (new_file
				  ? "RCS_BASE"
				  : (char *) 0)));
    char last_rev;
    int j;
    char *s;

    opt_all = argv_init();

    w_opts = FALSE;
    u_or_l = FALSE;

    last_rev =
	*opt_opt =
	*opt_rev = EOS;		/* assume we don't find version */

    for (j = 1; j < argc; j++) {
	if (is_option(s = argv[j])) {
	    if (is_my_opt(s))
		continue;
	    if (is_t_opt(s) && new_file)
		continue;
	    if (is_m_opt(s)) {
		m_option = s + 2;
		continue;
	    }

	    /*
	     * Retain the last-specified revision value in opt_rev,
	     * allowing us to put "-q" and other options in any
	     * order.
	     */ if (strchr(REV_OPT, *++s)) {
		static char fast[] = "-?";
		int code = *s++;

		switch (code) {
		case 'q':
		    silent++;
		    break;
		case 'k':
		case 'r':
		    u_or_l = FALSE;
		    break;
		case 'l':
		    u_or_l = TRUE;
		    break;
		case 'u':
		    u_or_l = -TRUE;
		    break;
		}
		if (!EMPTY(s)) {
		    s = strcpy(opt_rev, s);
		    s += strlen(opt_rev) - 2;
		    if (s > opt_rev && !strcmp(s, ".0"))
			(void) revert("baseline version");
		}
		if (last_rev != EOS)
		    argv_append(&opt_all, fast);
		fast[1] = (char) code;
		last_rev = (char) code;
		(void) strcpy(opt_opt, fast);
	    } else {
		if (*s == 'w')
		    w_opts = TRUE;
		argv_append(&opt_all, argv[j]);
	    }
	}
    }

    if (EMPTY(opt_opt))
	(void) strcpy(opt_opt, "-r");

    if (new_file) {
	if (EMPTY(opt_rev)) {
	    static const char *fmt = "%d.1";
	    char last_base[REVSIZ];
	    int value;

	    if (EMPTY(RCSbase) || from_keys)
		argv_append(&opt_all, opt_opt);
	    else if (use_base)
		argv_append2(&opt_all, opt_opt, RCSbase);
	    else if (sscanf(RCSbase, fmt, &value) == 1) {
		FORMAT(last_base, fmt, value - 1);
		argv_append2(&opt_all, opt_opt, last_base);
	    }
	}
    }
    argv_append2(&opt_all, "-m", m_option);
}

static void
usage(void)
{
    static const char *tbl[] =
    {
	"Usage: checkin [-options] [working_or_archive [...]]",
	"",
	"Options (from \"ci\"):",
	"  -r[rev]  assigns the revision number \"rev\" to the checked-in revision",
	"  -f[rev]  forces a deposit",
	"  -k[rev]  obtains keywords from working file (rev overrides)",
	"  -l[rev]  like \"-r\", but follows with \"co -l\"",
	"  -u[rev]  like \"-l\", but no lock is made",
	"  -q[rev]  quiet mode",
	"  -mmsg    specifies log-message \"msg\"",
	"  -Mfile   specifies log-message \"msg\"",
	"  -nname   assigns symbolic name to the checked-in revision",
	"  -Nname   like \"-n\", but overrides previous assignment",
	"  -sstate  sets the revision-state (default: \"Exp\")",
	"  -t[txtfile] writes descriptive text into the RCS file",
	"  -wlogin  overrides the username",
	"",
	"Non-\"ci\" options:",
	"  -B       ignore existing baseline version when defaulting revision",
	"  -D       debug/no-op (show actions, but don't do)"
    };
    unsigned j;
    for (j = 0; j < sizeof(tbl) / sizeof(tbl[0]); j++)
	FPRINTF(stderr, "%s\n", tbl[j]);
    (void) exit(FAIL);
}

/************************************************************************
 *	main program							*
 ************************************************************************/

/*ARGSUSED*/
_MAIN
{
    Stat_t sb;
    int new_file;		/* per-file, true if no archive */
    int j;
    char *s;

    debug = RCS_DEBUG;
    catchall(cleanup);
    WhoAmI();

    HIS_uid = getuid();
    HIS_gid = getgid();

    if (!isatty(fileno(stdin)) && interactive())
	cat_input = strtrim(file2mem("-"));

    /* Process the options */
    for (j = 1; (j < argc) && is_option(s = argv[j]); j++) {
	if (*(++s) == 'q')
	    silent++;
	else if (*s == 'k')
	    from_keys++;
	if (is_t_opt(argv[j]))
	    t_option = argv[j];
	if (strchr("rfkluqmnNstw", *s) == 0) {
	    if (*s == 'B') {
		use_base = FALSE;
		continue;
	    } else if (*s == 'D') {
		no_op++;
		continue;
	    } else if (*s == 'M') {
		cat_input = strtrim(file2mem(s + 1));
		continue;
	    } else if (*s != '?')
		FPRINTF(stderr, "unknown option: %s\n", s - 1);
	    usage();
	}
    }

    /* process the filenames */
    if (j < argc) {
	while (j < argc) {
	    j = rcsargpair(j, argc, argv);

	    if (rcs_working(Working, &sb) >= 0)
		modtime = sb.st_mtime;
	    else {
		GiveBack("directory access");
		if (!(modtime = DateOf(Working)))
		    GiveUp("file not found:", Working);
	    }

#if RCS_VERSION <= 4
	    if (new_file = (rcs_archive(Archive, &sb) < 0)) {
		oldtime = 0;	/* did not find archive */
		MakeDirectory();
		if (for_admin(RcsInitialize) < 0)
		    failed(Archive);
	    } else {
		MakeDirectory();
		oldtime = sb.st_mtime;
	    }

	    SetOpts(j, argv, new_file);
	    if (new_file || GetLock()) {

		if_D_option(
			       if (!from_keys && modtime != 0)
			       argv_append(&opt_all, "-d")
		    )

		    if (Execute()) {
			time_t newtime = DateOf(Archive);
			if (newtime != 0
			    && newtime != oldtime) {
			    PostProcess();
			    ReProcess();
			}
		    } else if (no_op)
			OwnWorking();
	    }
#else /* RCS_VERSION >= 5 */
	    if ((new_file = (rcs_archive(Archive, &sb) < 0)) != 0) {
		MakeDirectory();
		if (for_admin(RcsInitialize) < 0)
		    failed(Archive);
	    } else {
		MakeDirectory();
	    }

	    SetOpts(j, argv, new_file);

	    if (!from_keys && modtime != 0)
		argv_append(&opt_all, "-d");

	    if (Execute())
		OwnWorking();
#endif /* RCS_VERSION */
	}
    } else {
	FPRINTF(stderr, "? expected filename\n");
	usage();
    }
    DEBUG(("...normal exit\n"));
    (void) exit(SUCCESS);
    /*NOTREACHED */
}
