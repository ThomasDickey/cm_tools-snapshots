#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/cm_tools.vcs/src/checkout/src/RCS/checkout.c,v 9.2 1991/07/11 14:47:28 dickey Exp $";
#endif

/*
 * Title:	checkout.c (front end for RCS checkout)
 * Author:	T.E.Dickey
 * Created:	20 May 1988 (from 'sccsdate.c')
 * Modified:
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
#include	"ptypes.h"
#include	"rcsdefs.h"

#include	<ctype.h>
#include	<signal.h>
#include	<time.h>
extern	long	packdate();
extern	char	*ctime();
extern	char	*getuser();
extern	char	*pathhead();
extern	time_t	cutoff();

extern	char	*optarg;
extern	int	optind;

/* local definitions */
#define	WARN	FPRINTF(stderr,
#define	TELL	if (!silent) PRINTF
#define	DEBUG(s)	if(debug) PRINTF s;

static	time_t	opt_c	= 0;
static	int	silent;
static	int	debug;			/* set from environment RCS_DEBUG */
static	int	x_opt;			/* extended pathnames */
static	int	locked;			/* TRUE if user is locking file */
static	int	mode;			/* mode with which 'co' sets file */
static	int	Effect, Caller;		/* effective/real uid's	*/
static	char	*Working, *Archive;	/* current names we are using */
static	char	*UidHack;		/* intermediate file for setuid	*/
static	char	options[BUFSIZ];	/* options to pass to 'co' */
static	char	opt_rev[BUFSIZ];	/* revision to find */
static	char	opt_who[BUFSIZ];	/* "-w[login] value	*/
static	char	opt_sta[BUFSIZ];	/* "-s[state] value	*/

/************************************************************************
 *	local procedures						*
 ************************************************************************/

/*
 * Cleanup handler
 */
static
clean_file()
{
	if ((UidHack != 0)
	&&  (Working != 0)
	&&  strcmp(UidHack,Working)) {
		(void)unlink(UidHack);
		UidHack = 0;
	}
}

static
SIG_T
cleanup(sig)
{
	(void)signal(sig, SIG_IGN);
	WARN "checkout: cleaning up\n\n");
	clean_file();
	(void)exit(FAIL);
}

/*
 * Postprocess the checkout by scanning the RCS file to find the delta-date
 * to which the checked-out file corresponds, then touching the checked-out
 * file to make it correspond.
 */
static
PostProcess()
{
	time_t	ok	= 0;		/* must set this to touch file */
	char	key[BUFSIZ],
		tmp[BUFSIZ],
		dft_rev[BUFSIZ],
		tmp_rev[BUFSIZ],
		*s	= 0;
	int	header	= TRUE;
	time_t	tt	= 0;
	int	yd, md, dd, ht, mt, st;

	if (!rcsopen(Archive, -debug))
		return;

	while (header && (s = rcsread(s))) {
		s = rcsparse_id(key, s);

		switch (rcskeys(key)) {
		case S_HEAD:
			s = rcsparse_num(tmp, s);
			(void)strcpy(dft_rev, tmp);
			break;
		case S_LOCKS:
			/* see if this was locked by the user */
			(void)strcpy(tmp, getuser());
			*tmp_rev = EOS;
			s = rcslocks(s, tmp, tmp_rev);
			if (*tmp_rev) {
				TELL("** revision %s is locked\n", tmp_rev);
				mode |= S_IWRITE;
			}
			break;
		case S_COMMENT:
			s = rcsparse_str(s, NULL_FUNC);
			break;
			/* begin a delta description */
		case S_VERS:
			tt = 0;
			(void)strcpy(tmp_rev, key);
			DEBUG(("version = %s\n", tmp_rev))
			break;
		case S_DATE:
			s = rcsparse_num(tmp, s);
			if (sscanf(tmp, FMT_DATE,
				&yd, &md, &dd, &ht, &mt, &st) == 6) {
				newzone(5,0,FALSE);
				tt = packdate(1900+yd, md,dd, ht,mt,st);
				oldzone();
				DEBUG(("date    = %s", ctime(&tt)))
				if (opt_c != 0 && tt > opt_c)
					tt = 0;
			} else
				header = FALSE;
			break;
		case S_AUTHOR:
			s = rcsparse_id(key, s);
			if (*opt_who && strcmp(opt_who,key))	tt = 0;
			break;
		case S_STATE:
			s = rcsparse_id(key, s);
			if (*opt_sta && strcmp(opt_sta,key))	tt = 0;
			break;
			/* 'next' is the last keyword in a delta description */
		case S_NEXT:
			if (tt != 0) {
				char	*rev = *opt_rev ? opt_rev : dft_rev;
				DEBUG(("compare %s %s => %d (for equality)\n",
					tmp_rev, rev,
					vercmp(tmp_rev, rev, TRUE)))
				if (vercmp(tmp_rev, rev, TRUE) == 0) {
					ok = tt;
					header = FALSE;	/* force an exit */
				}
			}
			break;
		case S_DESC:
			header = FALSE;
		}
		if (!s)	break;
	}
	rcsclose();

	if (ok) {
		if (userprot(Working, mode, ok) < 0)
			noPERM(Working);
	}
}

/*
 * Check out the file using the RCS 'co' utility.  If 'co' does something, then
 * it will delete or modify the checked-in file -- return TRUE.  If no action
 * is taken, return false.
 */
static
Execute(mtime)
time_t	mtime;
{
	char	cmds[BUFSIZ];
	struct	stat	sb;

	UidHack = rcstemp(Working, FALSE);
	FORMAT(cmds, "%s%s %s", options, UidHack, Archive);
	if (!silent) shoarg(stdout, "co", cmds);
	if (execute(rcspath("co"), cmds) >= 0) {
		if (stat(UidHack, &sb) >= 0) {
			DEBUG(("=> file \"%s\"\n", UidHack))
			DEBUG(("=> size = %d\n", sb.st_size))
			DEBUG(("=> date = %s", ctime(&sb.st_mtime)))
			if (sb.st_mtime == mtime) {
				TELL("** checkout was not performed\n");
				return (FALSE);
			}
			if (strcmp(UidHack,Working)) {
				if (usercopy(UidHack, Working) < 0)
					failed(Working);
				clean_file();
			}
			mode = (sb.st_mode & 0777);
			if (!locked)
				mode &= 0555;	/* strip writeable mode */
			if (!mode)
				mode = 0400;	/* leave at least readonly! */
			return (TRUE);
		}
	}
	clean_file();
	return (FALSE);
}

/*
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
static
time_t
PreProcess(name,owner)
char	*name;
int	owner;			/* TRUE if euid, FALSE if uid */
{
	struct	stat	sb;
	int		uid;

	DEBUG(("...PreProcess(%s,%d)\n", name, owner))
	if (stat(name, &sb) >= 0) {
		if ((S_IFMT & sb.st_mode) == S_IFREG) {
			if (owner) {	/* setup for archive: effective */
				char	RCSdir[BUFSIZ],
					*s = strrchr(strcpy(RCSdir, name),'/');
				if (s != 0)
					*s = EOS;
				else
					(void)strcpy(RCSdir, "./");
					DEBUG(("++ RCSdir='%s'\n", RCSdir))

				if (Effect != sb.st_uid && Effect != 0) {
					revert(debug ? "non-CM use":(char *)0);
					Effect = geteuid();
				} else if (!rcspermit(RCSdir,(char *)0))
					revert((locked || debug) ?
						"not listed in permit-file" :
						(char *)0);
				if (locked) {
					permit(name, &sb, uid = Effect);
				}
			} else {	/* setup for working: real */
				permit(name, &sb, uid = Caller);
			}
			DEBUG(("=> date = %s", ctime(&sb.st_mtime)))
			return (sb.st_mtime);
		}
		TELL ("** ignored (not a file)\n");
		return (FALSE);
	}
	if (!owner) {
		name = pathhead(name, &sb);
		if (access(name, W_OK) < 0)
			permit(name, &sb, uid);
	}
	return (TRUE);	/* file was not already checked out */
}

static
permit(name, sb_, uid)
char		*name;
struct	stat	*sb_;
{
	if (uid != sb_->st_uid && Effect != 0) {
		DEBUG(("=> uid  = %d, file = %d\n", uid, sb_->st_uid))
		noPERM(name);
	}
}

#include <errno.h>
static
noPERM(name)
char	*name;
{
	extern	int	errno;
	errno	= EPERM;
	failed(name);
}

/*
 * Process a single file.
 */
static
do_file(name)
char	*name;
{
	char		*s, dirname[BUFSIZ];
	time_t		mtime;

	Working = rcs2name(name,x_opt);
	Archive = name2rcs(name,x_opt);

	if (s = strrchr(strcpy(dirname, Working),'/'))
		s[1] = EOS;
	else
		(void)strcpy(dirname, ".");
	if (access(dirname, W_OK) < 0)
		failed(dirname);

	DEBUG(("...do_file(%s) => %s %s\n", name, Working, Archive))

	if (PreProcess(Archive,TRUE) == TRUE) {
		revert(debug ? "directory access" : (char *)0);
		if (PreProcess(Archive,TRUE) == TRUE) {
			WARN "?? can't find archive \"%s\"\n", Archive);
			(void)exit(FAIL);
		}
	}

	if (mtime = PreProcess (Working,FALSE)) {
		if (Execute(mtime))
			PostProcess ();
	}
}

static
usage()
{
	static	char	*tbl[] = {
 "Usage: checkout [options] [working_or_archive [...]]"
,""
,"Options (from \"co\"):"
,"  -l[rev]  locks the checked-out revision for the caller."
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
,"  -p[rev]  prints the revision on standout-output"
,""
,"Non-\"co\" options:"
,"  -x       assume archive and working file are in path2/RCS and path2"
,"           (normally assumes ./RCS and .)"
	};
	register int	j;
	setbuf(stderr, options);
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	(void)fflush(stderr);
	(void)exit(FAIL);
}

/************************************************************************
 *	main program							*
 ************************************************************************/
main (argc, argv)
char	*argv[];
{
	register int	j;
	char		tmp[BUFSIZ];
	register char	*s, *d;

	debug  = RCS_DEBUG;
	*options = EOS;
	Caller = getuid();
	Effect = geteuid();
	if (Caller != Effect)
		catchall(cleanup);

	for (j = 1; j < argc; j++) {
		s = argv[j];
		d = 0;
		if (*s == '-') {
			if (*(++s) == 'c') {
				optind = j;
				optarg = ++s;
				opt_c = cutoff(argc, argv);
				j = optind;
				FORMAT(tmp, "-d%s", ctime(&opt_c));
				tmp[strlen(tmp)-1] = EOS;
				TELL("++ cutoff: %s", ctime(&opt_c));
				catarg(options, tmp);
			} else {
				if (*s == 'q')
					silent++;
				if (strchr("lqr", *s)) {
					if (*s == 'l')
						locked++;
					d = opt_rev;
				} else if (*s == 's') {
					d = opt_sta;
				} else if (*s == 'w') {
					d = opt_who;
				} else if (*s == 'x') {
					x_opt++;
					continue;
				} else {
					WARN "?? Unknown option: %s\n", s-1);
					usage();
				}
				catarg(options, argv[j]);
				if (d)
					(void)strcpy(d,++s);
			}
		} else {
			do_file(s);
		}
	}
	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
