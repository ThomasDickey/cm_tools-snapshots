#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/cm_tools.vcs/src/checkin/src/RCS/checkin.c,v 10.0 1991/10/25 15:20:18 ste_cm Rel $";
#endif

/*
 * Title:	checkin.c (RCS checkin front-end)
 * Author:	T.E.Dickey
 * Created:	19 May 1988, from 'sccsbase'
 * Modified:
 *		25 Oct 1991, 'tmpfile()' resets effective uid/gid (don't use!)
 *		24 Oct 1991, corrected exit-condition in 'GetLock()' - was
 *			     reading past header.  Also, test for "-f" option in
 *			     'GetLock()' to ensure that we pick up lock_date
 *			     even if no locks were made.
 *		22 Oct 1991, corrected initial-access if root-uid (should be the
 *			     owner of the directory, not "root").
 *		22 Oct 1991, corrected recent change which caused working-files
 *			     which had no changes in 'Filtered()' to be written
 *			     as empty (since fpT was closed).
 *		18 Oct 1991, corrected conditions under which mismatch between
 *			     effect/real group-id causes reversion.  Also, fixed
 *			     so that we don't call HackMode for SetAccess.
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
 *		19 Sep 1991, correct spurious introduction of "-x" option into
 *			     command to create first delta.
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
 *		18 Apr 1990, added "-x" option (to assist in makefiles, etc).
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
 */

#define		SIG_PTYPES
#define		STR_PTYPES
#include	<ptypes.h>
#include	<rcsdefs.h>

#include	<ctype.h>
#include	<pwd.h>
#include	<signal.h>
#include	<time.h>
#include	<errno.h>
extern	char	*mktemp();

/* local declarations: */
#define	CI	"ci"
#define	RCS	"rcs"

#define	REV_OPT	"rfluqk"
#define	is_option(s)	(*(s) == '-')
#define	is_t_opt(s)	(s[1] == 't')
#define	is_my_opt(s)	(s[1] == 'B' || s[1] == 'x' || s[1] == 'd')

#define	WARN	FPRINTF(stderr,
#define	TELL	if (!silent) PRINTF
#define	DEBUG(s)	if (debug) PRINTF s;

#if	RCS_VERSION >= 4
#define	if_D_option(s)	if (lock_date < modtime) s;
#else
#define	if_D_option(s)
#endif

static	int	silent	= FALSE,
		debug,			/* set by RCS_DEBUG environment */
		no_op,			/* show, but don't do */
		use_base = TRUE,	/* use RCS_BASE for base-version */
		x_opt,			/* extended pathnames */
		locked	= FALSE,	/* set if file is re-locked */
		from_keys = FALSE,	/* set if we get date from RCS file */
		new_file,		/* per-file, true if no archive */
		TMP_mode,		/* saved protection of RCS-directory */
		HIS_uid,		/* working-file's owner */
		HIS_gid,		/* working-file's group */
		RCS_uid,		/* archive's owner */
		RCS_gid,		/* archive's group */
		RCSprot;		/* protection of RCS-directory */
static	time_t	lock_date,		/* timestamp from prior version */
		modtime,		/* timestamp of working file */
		oldtime;		/* timestamp of archive file */
static	char	*Working,
		*Archive,
		*TMP_file,		/* temp-file used to fix uid/access */
		*t_option,		/* "-t" option used for "ci" or "rcs" */
		RCSdir[MAXPATHLEN],
		RCSbase[20],		/* base+ version number */
		opt_all[BUFSIZ],	/* options for 'ci' */
		opt_rev[BUFSIZ];
static	char	old_date[BUFSIZ],
		new_date[BUFSIZ];

/************************************************************************
 *	local procedures						*
 ************************************************************************/

static
WhoAmI(_AR0)
{
	if (debug) {
		PRINTF("...uid=%d(%s)",		 getuid(), uid2s(getuid()));
		if (geteuid() != getuid())
			PRINTF(", euid=%d(%s)", geteuid(), uid2s(geteuid()));
		PRINTF(", gid=%d(%s)",		 getgid(), gid2s(getgid()));
		if (getegid() != getgid())
			PRINTF(", egid=%d(%s)", getegid(), gid2s(getegid()));
		PRINTF("\n");
	}
}

/*
 * Remove working-file for set-uid process if the matching temp-file was.
 */
static
rm_work(_AR0) { return (unlink(Working)); }

static
clean_file(_AR0)
{
	if ((Working  != 0)
	&&  (TMP_file != 0)
	&&  strcmp(Working,TMP_file)) {
		if (no_op)
			TELL("%% rm -f %s\n", TMP_file);
		(void)unlink(TMP_file);
	}
	TMP_file = 0;
}

/*
 * If interrupted, clean up and exit
 */
static
SIGNAL_FUNC(cleanup)
{
	static	int	latch;
	if (!latch++) {
		(void)signal(sig, SIG_IGN);
		if (TMP_file)	clean_file();
		if (TMP_mode)	HackMode(FALSE);
		WARN "checkin: cleaned up\n\n");
	}
	(void)exit(FAIL);
	/*NOTREACHED*/
}

/*
 * Restore ownership of a file to the "natural" owner after a check-in
 */
static
FixOwnership(
_ARX(char *,	name)
_ARX(int,	uid)
_AR1(int,	gid)
	)
_DCL(char *,	name)
_DCL(int,	uid)
_DCL(int,	gid)
{
	DEBUG(("...fix ownership of %s\n", name))
	WhoAmI();
	if (!geteuid()) {
		DEBUG(("%% chown %s %s\n", uid2s(uid), name))
		DEBUG(("%% chgrp %s %s\n", gid2s(gid), name))
		if (!no_op)
			(void)chown(name, uid, gid);
	}
}

static
OwnWorking(_AR0)
{
	FixOwnership(Working, HIS_uid, HIS_gid);
}

static
OwnArchive(_AR0)
{
	FixOwnership(Archive, RCS_uid, RCS_gid);
}

/*
 * Decode an rcs archive-date
 */
static
time_t
DecodeArcDate(
_AR1(char *,	from))
_DCL(char *,	from)
{
	time_t	the_time;
	int	year, mon, day, hour, min, sec;

	newzone(5,0,FALSE);	/* format for EST5EDT */

	if (sscanf(from, FMT_DATE, &year, &mon, &day, &hour, &min, &sec) == 6)
		the_time = packdate(1900+year, mon, day, hour, min, sec);

	oldzone();
	return the_time;
}

/*
 * Convert a unix time to an rcs archive-date
 */
static
EncodeArcDate(
_ARX(char *,	to)
_AR1(time_t,	from)
	)
_DCL(char *,	to)
_DCL(time_t,	from)
{
	struct	tm *t;

	newzone(5,0,FALSE);	/* format for EST5EDT */
	t = localtime(&from);
	FORMAT(to, FMT_DATE,
		t->tm_year, t->tm_mon + 1,
		t->tm_mday, t->tm_hour,
		t->tm_min,  t->tm_sec);
	oldzone();
}

/*
 * If we could not persuade 'ci' to check-in the file with "-d", filter a copy
 * of it so that the date-portion of the identifiers is set properly.
 */
static
Filter(_AR0)
{
	auto	FILE	*fpS, *fpT;
	auto	size_t	len	= strlen(old_date);
	auto	int	changed	= 0;	/* number of substitutions done */
	register char	*s, *d;
	auto	char	name[MAXPATHLEN];
	auto	char	bfr[BUFSIZ];
	auto	int	lines = 0;

	if_D_option(return)

	if (from_keys || !(fpS = fopen(Working, "r"))) {
		errno = 0;
		return;
	}

	/* Alter the delta-header to match RCS's substitution */
	/* 0123456789.123456789. */
	/* yy.mm.dd.hh.mm.ss */
	new_date[ 2] = old_date[ 2] =
	new_date[ 5] = old_date[ 5] = '/';
	new_date[ 8] = old_date[ 8] = ' ';
	new_date[11] = old_date[11] =
	new_date[14] = old_date[14] = ':';

	if (!(fpT = fopen(TMP_file = strcat(strcpy(name, Working), ",,"), "w")))
		GiveUp("Could not create tmp-file", name);

	DEBUG(("...filtering date-strings to %s\n", new_date))
	while (fgets(bfr, sizeof(bfr), fpS)) {
		char	*last = bfr + strlen(bfr) - len;

		lines += 1;
		if ((last > bfr)
		&&  (d = strchr(bfr, '$'))) {
			while (d <= last) {
				if (!strncmp(d, old_date, len)) {
					DEBUG(("...edit date at line %d\n%s",
						lines, bfr))
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
		(void)unlink(name);
	TMP_file = 0;
	FCLOSE(fpT);
}

/*
 * If the given file is still checked-out, touch its time.
 * patch: should do keyword substitution for Header, Date a la 'co'.
 */
static
ReProcess (_AR0)
{
	auto	struct	stat	sb;
	auto	int	mode;

	if ((stat(Working, &sb) >= 0)
	&&  ((sb.st_mode & S_IFMT) == S_IFREG)) {
		mode = sb.st_mode & 0777;

		OwnWorking();
		if (for_user(Filter) < 0)
			GiveUp("cannot reprocess", Working);
		if (!locked)		/* cover up bugs on Apollo acls */
			mode &= ~0222;

		DEBUG(("%% chmod %03o %s\n", mode, Working))
		if (userprot(Working, mode, modtime) < 0)
			GiveUp("touch failed", Working);
	}
}

/*
 * Post-process a single RCS-file, replacing its check-in date with the file's
 * modification date.
 */
static
PostProcess (_AR0)
{
int	header	= TRUE,
	match	= FALSE,
	changed	= FALSE;
char	*s	= 0,
	*old,
	revision[BUFSIZ],
	token[BUFSIZ];

	if_D_option(return)

	if (!rcsopen(Archive, debug, FALSE))
		return;
	(void)strcpy(revision, opt_rev);

	while (header && (s = rcsread(s))) {
		s = rcsparse_id(token, s);

		switch (rcskeys(token)) {
		case S_HEAD:
			s = rcsparse_num(token, s);
			if (!*revision)
				(void)strcpy(revision, token);
			break;
		case S_COMMENT:
			s = rcsparse_str(s, NULL_FUNC);
			break;
		case S_VERS:
			if (dotcmp(token, revision) < 0)
				header = FALSE;
			match = !strcmp(token, revision);
			break;
		case S_DATE:
			if (!match)
				break;
			s = rcsparse_num(old_date, old = s);
			if (*old_date) {
				if (from_keys) {
					modtime = DecodeArcDate(old_date);
				} else {
					EncodeArcDate(new_date, modtime);
					rcsedit(old, old_date, new_date);
				}
				TELL("** revision %s\n", revision);
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

/*
 * The RCS utilities (ci and rcs) use the unix 'access()' function to test if
 * the caller has access to the archive-directory.  This is not done properly,
 * (particularly for non-root set-uid  but I cannot easily code around it other
 * than by temporarily changing the directory's protection.
 */
static
HackMode(
_AR1(int,	save))
_DCL(int,	save)
{
	int	need;

	if (!geteuid()) {
		;
	} else if (save) {
		if ((RCSdir[0] != EOS)
		&&  (getuid()  != RCS_uid)) {
			need = (getgid() == RCS_gid) ? 0775 : 0777;
			DEBUG(("...chmod %04o %s, was %04o\n",
				need, RCSdir, RCSprot))
			if (need != RCSprot) {
				TMP_mode = RCSprot | 01000;
				if (no_op) {
					TELL("%% chmod %#o %s\n", need, RCSdir);
				} else if (chmod(RCSdir, need) < 0)
					GiveUp("Could not change mode",RCSdir);
			}
		}
	} else if (TMP_mode) {
		TMP_mode &= 0777;
		if (no_op)
			TELL("%% chmod %#o %s\n", TMP_mode, RCSdir);
		DEBUG(("...chmod %04o %s, to restore\n", TMP_mode, RCSdir))
		(void)chmod(RCSdir, TMP_mode);
		TMP_mode = 0;
	}
}

/*
 * RCS 4 does not check permissions properly on the RCS directory even when
 * running in set-uid mode (sigh).
 */
DoIt (
_ARX(char *,	verb)
_ARX(char *,	args)
_AR1(int,	if_noop)
	)
_DCL(char *,	verb)
_DCL(char *,	args)
_DCL(int,	if_noop)
{
	int	code;
	int	fix_id;

	if (fix_id = (!geteuid() && getuid())) {
		if (!no_op) {
			(void)setruid(geteuid());
			(void)setrgid(getegid());
		}
		WhoAmI();
	}

	if (!silent) shoarg(stdout, verb, args);
	code = no_op ? if_noop : execute(rcspath(verb), args);

	if (!no_op && fix_id) {
		(void)setruid(HIS_uid);
		(void)setrgid(HIS_gid);
	}
	return code;
}

/*
 * Check in the file using the RCS 'ci' utility.  If 'ci' does something, then
 * it will delete or modify the checked-in file -- return TRUE.  If no action
 * is taken, return false.
 */
static
Execute(_AR0)
{
	char	cmds[BUFSIZ];
	struct	stat	sb;
	int	code;

	HackMode(TRUE);
	TMP_file = rcstemp(Working, TRUE);

	FORMAT(cmds, "%s%s %s", opt_all, Archive, TMP_file);
	code = DoIt(CI, cmds, -1);

	if (code >= 0 || no_op)
		OwnArchive();

	HackMode(FALSE);		/* ...restore protection */

	if (code >= 0) {			/* ... check-in file ok */
		if (stat(TMP_file, &sb) >= 0) {	/* working file not deleted */
			if (strcmp(TMP_file,Working)) {
				if (sb.st_mtime != modtime) {
					if (usercopy(TMP_file,Working) < 0)
						GiveUp("Copy to userfile",Working);
				}
			}
			clean_file();
			if (sb.st_mtime == modtime) {
				TELL("** checkin was not performed\n");
				return (FALSE);
			}
		} else if (strcmp(TMP_file, Working)
		&&	   for_user(rm_work) < 0)
			GiveUp("Deletion of working-file",Working);
		clean_file();
		return (TRUE);
	}
	clean_file();
	return (FALSE);
}

/*
 * This procedure is invoked only if 'SetAccess()' finds an RCS-archive.
 * Ensure that we have a unique lock-value.  If the user did not specify one,
 * we must get it from the archived-file.
 *
 * returns:
 *	+1 - a lock is present
 *	 0 - no lock was set (or can be forced)
 */
static
GetLock(_AR0)
{
	int	header	= TRUE,
		strict	= FALSE,
		match;

	char	*s	= 0,
		lock_rev[80],
		lock_by[80],
		tip[BUFSIZ],
		key[BUFSIZ];

	(void)strcpy(lock_by, getuser());
	*lock_rev = EOS;
	lock_date = 0;

	if (*opt_rev == EOS) {	/* patch: do I need this level ? */

		if (!rcsopen(Archive, -debug, TRUE))
			return (FALSE);	/* could not open file anyway */

		while (header && (s = rcsread(s))) {
			s = rcsparse_id(key, s);

			switch (rcskeys(key)) {
			case S_DESC:
			case S_LOG:
			case S_TEXT:
				header = FALSE;
				break;
			case S_HEAD:
				s = rcsparse_num(tip, s);
				if (!*tip)
					header = FALSE;
				DEBUG(("...GetLock tip = %s %s\n",
					tip,
					vercmp(RCSbase, tip, FALSE) < 0
						? "base"
						: ""))
				if (vercmp(RCSbase, tip, FALSE) < 0) {
					next_version(RCSbase, tip);
					DEBUG(("=>%s (next)\n", RCSbase))
				}
				break;
			case S_STRICT:
				strict = TRUE;
				break;

			case S_LOCKS:
				s = rcslocks(s, lock_by, lock_rev);
				if (!*lock_rev)
					break;

				TELL("** revision %s was locked\n",lock_rev);
				DEBUG(("...%s (%d:%d)\n",
					RCSbase,
					use_base,
					vercmp(RCSbase, lock_rev, FALSE)))
				if (!strcmp(tip, lock_rev)) {
					if (use_base
					&& (vercmp(RCSbase, lock_rev, FALSE) > 0))
						(void)strcpy(opt_rev, RCSbase);
					else
						next_version(opt_rev, lock_rev);
					DEBUG(("=>%s (locked)\n", opt_rev))
				} else {
					TELL("?? branching not supported\n");
					/* patch:finish this later... */
				}
				break;

			case S_VERS:
				if (*lock_rev)
					match = !strcmp(key, lock_rev);
				else
					match = TRUE;
				break;
			case S_DATE:
				if (match) {
					time_t	at;
					s = rcsparse_num(key, s);
					at = DecodeArcDate(key);
					/* if no lock, choose highest date */
					if (!*lock_rev) {
						if (lock_date < at)
							lock_date = at;
					} else
						lock_date = at;
					header = FALSE;
				}
				break;
			case S_COMMENT:
				s = rcsparse_str(s, NULL_FUNC);
				break;
			}
		}
		rcsclose();
	}
	if (strict && !*lock_rev)
		TELL("?? no lock set by %s\n", lock_by);
	DEBUG(("=> lock = %s", lock_date != 0 ? ctime(&lock_date) : "(none)\n"))
	return (!strict || (*opt_rev != EOS));
}

/*
 * Before checkin, verify that the file exists, and obtain its modification
 * time/date.
 */
static
time_t
DateOf(
_AR1(char *,	name))
_DCL(char *,	name)
{
	auto	struct	stat	sb;

	DEBUG(("...DateOf(%s)\n", name))
	errno = 0;
	if (stat(name, &sb) >= 0) {
		if ((S_IFMT & sb.st_mode) == S_IFREG) {
			DEBUG(("=> date = %s", ctime(&sb.st_mtime)))
			return (sb.st_mtime);
		}
		GiveUp("not a file:", Working);
	}
	DEBUG(("=> not found, errno=%d\n", errno))
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
typedef	struct	_prefix	{
	struct	_prefix	*link;
	char	*suffix,
		*prefix;
	} PREFIX;

static	PREFIX	*clist;

	/*ARGSUSED*/
	def_ALLOC(PREFIX)

static
define_prefix(
_ARX(char *,	suffix)
_AR1(char *,	prefix)
	)
_DCL(char *,	suffix)
_DCL(char *,	prefix)
{
	auto	char	tmp[BUFSIZ];
	register PREFIX	*new = ALLOC(PREFIX,1);
	new->link = clist;
	new->suffix = stralloc(suffix);
	new->prefix = stralloc(strcat(strcpy(tmp, "-c"), prefix));
	clist = new;
}

static
char	*
copy_to(
_ARX(char *,	dst)
_AR1(char *,	src)
	)
_DCL(char *,	dst)
_DCL(char *,	src)
{
	char	delim;
	if (delim = *src) {
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

static
char	*
get_prefix(
_AR1(char *,	suffix))
_DCL(char *,	suffix)
{
	PREFIX	*new;
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
static
RcsInitialize(_AR0)
{
	static	char	list[BUFSIZ];	/* static so we do list once */
	auto	char	cmds[BUFSIZ],
			*s, *t;

	if (clist == 0) {
		define_prefix(".a",	"--  ");
		define_prefix(".ada",	"--  ");
		define_prefix(".com",	"$!\t");
		define_prefix(".e",	" * ");
		define_prefix(".mms",	"#\t");
		define_prefix(".mk",	"#\t");
		if (s = getenv("RCS_COMMENT")) {
			char	suffix[BUFSIZ], prefix[BUFSIZ];
			while (*s) {
				s = copy_to(suffix, s);
				s = copy_to(prefix, s);
				if (*s)
					s++;
				define_prefix(suffix, prefix);
				while (*s == ',' || isspace(*s))
					s++;
			}
		}
	}

	*cmds = EOS;
	if (t_option != 0)
		catarg(cmds, t_option);
	catarg(cmds, "-i");
	if (!*list) {
		register struct passwd *p;

		if (p = getpwuid(geteuid() ? geteuid() : RCS_uid))
			FORMAT(list, "-a%s", p->pw_name);
		else
			GiveUp("owner of RCS directory for", Working);

		if (getuid() != HIS_uid) {
			if (p = getpwuid(HIS_uid))
				FORMAT(list + strlen(list), ",%s", p->pw_name);
			else
				GiveUp("owner of directory for", Working);
		}
	}
	catarg(cmds, list);

	s = ftype(t = pathleaf(Working));
	if (!*s && (	!strcmp(t, "Makefile")
		||	!strcmp(t, "IMakefile")
		||	!strcmp(t, "AMakefile")
		||	!strcmp(t, "makefile")) )
		catarg(cmds, "-c#\t");
	else if (s = get_prefix(s))
		catarg(cmds, s);

	if (silent) catarg(cmds, "-q");
	catarg(cmds, Archive);
	if (DoIt(RCS, cmds, 0) < 0)
		GiveUp("rcs initialization for", Working);
}

static
int
SetAccess(_AR0)
{
	if ((oldtime = DateOf(Archive)) != 0)
		return (FALSE);

	if (for_admin(RcsInitialize) < 0)
		failed(Archive);
	return (TRUE);
}

/*
 * If the RCS-directory does not exist, make it.  Save the name in 'RCSdir[]'
 * in case we need it for the set-uid hack in 'Execute()'.
 */
static
MakeDirectory(_AR0)
{
	struct	stat	sb;
	char	*s;
	size_t	len;

	if (s = strrchr(Archive, '/')) {
		len = s - Archive;
		if (len == strlen(RCSdir)
		&& !strncmp(RCSdir, Archive, len))
			return (TRUE);	/* same as last directory */

		(strncpy(RCSdir, Archive, len))[len] = EOS;

		if (stat(RCSdir, &sb) >= 0) {
			if (getegid() != sb.st_gid) {
				(void)setegid(sb.st_gid);
				WhoAmI();
			}
			if (geteuid() != 0
			&&  sb.st_uid != geteuid())
				revert(debug ? "non-CM use" : (char *)0);
			if (!rcspermit(RCSdir,RCSbase))
				revert(geteuid() || debug
					? "not listed in permit-file"
					: (char *)0);
			DEBUG((".. RCSbase='%s'\n", RCSbase))
			RCS_uid = sb.st_uid;
			RCS_gid = sb.st_gid;
			RCSprot = sb.st_mode & 0777;
			if ((sb.st_mode & S_IFMT) == S_IFDIR)
				return (TRUE);
			GiveUp("not a directory:", RCSdir);
		} else {
			revert(debug ? "new user directory" : (char *)0);
			RCS_uid = getuid();
			RCS_gid = getgid();
			RCSprot = 0755;
			TELL("%% mkdir %s\n", RCSdir);
			if (!no_op) {
				if (mkdir(RCSdir, RCSprot) < 0)
					GiveUp("directory-create",RCSdir);
			}
		}
	} else		/* ...else... user is putting files in "." */
		RCSdir[0] = EOS;
	return (TRUE);
}

/*
 * Generate the string 'opt_all[]' which we will pass to 'ci' for options.
 * We may have to generate it before each file because some are initial
 * checkins.
 *
 * For initial checkins we have a special case: if the environment variable
 * RCS_BASE is set, we use this for the revision code rather than "1.1".
 */
static
SetOpts(
_ARX(int,	argc)
_ARX(char **,	argv)
_AR1(int,	code)
	)
_DCL(int,	argc)
_DCL(char **,	argv)
_DCL(int,	code)
{
	int	last	= 0;
	int	logmsg	= FALSE;
	int	got_rev	= FALSE;
	register int	j;
	char	bfr[BUFSIZ];
	char	*dft;
	register char *s;

	*opt_all = EOS;
	for (j = 1; j < argc; j++) {
		if (is_option(s = argv[j])) {
			if (is_my_opt(s))
				continue;
			if (is_t_opt(s) && !new_file)
				continue;
			catarg(opt_all, s);
			if (*++s == 'q')
				silent++;
			if (strchr(REV_OPT, *s)) {
				if (*s == 'l')	locked = TRUE;
				if (*(++s)) {
					got_rev = TRUE;
					s = strcpy(opt_rev, s);
					s += strlen(opt_rev) - 2;
					if (s > opt_rev && !strcmp(s, ".0"))
						revert("baseline version");
				}
				last = j;
			} else if (*s == 'm')
				logmsg = TRUE;
		}
	}

	if (!*opt_rev)
		(void)strcpy(opt_rev, RCSbase);

	/*
	 * If no revision was specified, and there is a default in effect,
	 * reprocess the list of options so that it includes the default
	 * revision.  Also, if no log-message was specified, add one so we
	 * don't get prompted unnecessarily.
	 */
	if (!got_rev && !from_keys && (code < 0 || use_base))
		dft = opt_rev;
	else
		dft = 0;	/* don't supply a default-version */

	if ((dft != 0) && (*dft != EOS)) {
		if (last != 0) {
			*opt_all = EOS;
			for (j = 1; j < argc; j++) {
				if (is_option(s = argv[j])) {
					if (is_my_opt(s))
						continue;
					if (is_t_opt(s) && !new_file)
						continue;
					if (j == last) {
						FORMAT(bfr, "%s%s", s, dft);
						s = bfr;
					}
					catarg(opt_all, s);
				}
			}
		} else {	/* no revision-related option was given */
			FORMAT(bfr, "-r%s", dft);
			catarg(opt_all, bfr);
		}
		if (!logmsg && code < 0) {
			catarg(opt_all, "-mRCS_BASE");
		}
	} else if (from_keys && !got_rev && !logmsg)
		catarg(opt_all, "-mFROM_KEYS");

	if_D_option(catarg(opt_all, "-d"))
}

static
GiveUp(
_ARX(char *,	msg)
_AR1(char *,	arg)
	)
_DCL(char *,	msg)
_DCL(char *,	arg)
{
	TELL("?? %s \"%s\"\n", msg, arg);
	(void)exit(FAIL);
}

static
usage(_AR0)
{
	static	char	*tbl[] = {
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
"  -nname   assigns symbolic name to the checked-in revision",
"  -Nname   like \"-n\", but overrides previous assignment",
"  -sstate  sets the revision-state (default: \"Exp\")",
"  -t[txtfile] writes descriptive text into the RCS file",
"",
"Non-\"ci\" options:",
"  -B       ignore existing baseline version when defaulting revision",
"  -d       debug/no-op (show actions, but don't do)",
"  -x       assume archive and working file are in path2/RCS and path2",
"           (normally assumes ./RCS and .)"
	};
	register int	j;
	setbuf(stderr, opt_all);
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	(void)exit(FAIL);
}

/************************************************************************
 *	main program							*
 ************************************************************************/

/*ARGSUSED*/
_MAIN
{
	register int	j;
	register char	*s;

	debug = RCS_DEBUG;
	if (s = getenv("RCS_BASE"))
		(void)strcpy(RCSbase,s);
	catchall(cleanup);
	WhoAmI();

	HIS_uid = getuid();
	HIS_gid = getgid();

	/*
	 * Process the argument list
	 */
	for (j = 1; j < argc; j++) {
		if (is_option(s = argv[j])) {
			if (*(++s) == 'q')
				silent++;
			else if (*s == 'k')
				from_keys++;
			if (is_t_opt(argv[j]))
				t_option = argv[j];
			if (strchr("rfkluqmnNst", *s) == 0) {
				if (*s == 'B') {
					use_base = FALSE;
					continue;
				} else if (*s == 'd') {
					no_op++;
					continue;
				} else if (*s == 'x') {
					x_opt++;
					continue;
				} else if (*s != '?')
					WARN "unknown option: %s\n", s-1);
				usage();
			}
		} else {
			int	code = -1;

			Working = rcs2name(s,x_opt),
			Archive = name2rcs(s,x_opt);

			if (!(modtime = DateOf (Working))) {
				revert(debug ? "directory access" : (char *)0);
				if (!(modtime = DateOf (Working))) {
					GiveUp("file not found:", Working);
				}
			}

			oldtime = 0;	/* assume we don't find archive */
			*opt_rev = EOS;	/* assume we don't find version */
			if (MakeDirectory()
			&&  (new_file = SetAccess() || (code = GetLock()))) {
				SetOpts(j,argv,code);
				if (Execute()) {
					time_t	newtime = DateOf(Archive);
					if (newtime != 0
					&&  newtime != oldtime) {
						PostProcess();
						ReProcess();
					}
				} else if (no_op)
					OwnWorking();
			}
		}
	}
	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
