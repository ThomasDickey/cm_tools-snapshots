#ifndef	lint
static	char	sccs_id[] = "@(#)checkout.c	1.23 88/09/28 09:35:18";
#endif	lint

/*
 * Title:	checkout.c (front end for RCS checkout)
 * Author:	T.E.Dickey
 * Created:	20 May 1988 (from 'sccsdate.c')
 * Modified:
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
#include	"ptypes.h"
#include	"rcsdefs.h"

#include	<ctype.h>
#include	<signal.h>
#include	<time.h>
extern	long	packdate();
extern	char	*ctime();
extern	char	*getuser();
extern	char	*pathhead();
extern	char	*strcat();
extern	char	*strcpy();
extern	char	*strchr();
extern	time_t	cutoff();

extern	char	*optarg;
extern	int	optind;

/* local definitions */
#define	WARN	FPRINTF(stderr,
#define	TELL	if (!silent) PRINTF

static	time_t	opt_c	= 0;
static	int	silent;
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

cleanup(sig)
{
	(void)signal(sig, SIG_IGN);
	WARN "checkout: cleaning up\n\n");
	clean_file();
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

	if (!rcsopen(Archive, -RCS_DEBUG))
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
			break;
		case S_DATE:
			s = rcsparse_num(tmp, s);
			if (sscanf(tmp, FMT_DATE,
				&yd, &md, &dd, &ht, &mt, &st) == 6) {
				newzone(5,0,FALSE);
				tt = packdate(1900+yd, md,dd, ht,mt,st);
				oldzone();
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
				if (dotcmp(tmp_rev, rev) <=0) {
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
	TELL("** co %s\n", cmds);
	if (execute(rcspath("co"), cmds) >= 0) {
		if (stat(UidHack, &sb) >= 0) {
			if (strcmp(UidHack,Working)) {
				if (usercopy(UidHack, Working) < 0)
					return (FALSE);
				clean_file();
			}
			if (sb.st_mtime == mtime) {
				TELL("** checkout was not performed\n");
				return (FALSE);
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

	if (stat(name, &sb) >= 0) {
		if ((S_IFMT & sb.st_mode) == S_IFREG) {
			if (owner) {	/* setup for archive: effective */
				uid = Effect;
			} else {	/* setup for working: real */
				uid = Caller;
			}
			permit(name, &sb, uid);
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
	if (uid != sb_->st_uid)
		noPERM(name);
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
	time_t		mtime;

	Working = rcs2name(name);
	Archive = name2rcs(name);

	if (PreProcess(Archive,TRUE) == TRUE) {
		WARN "?? can't find archive \"%s\"\n", Archive);
		(void)exit(FAIL);
	}

	if (mtime = PreProcess (Working,FALSE)) {
		if (Execute(mtime))
			PostProcess ();
	}
}

static
usage()
{
	setbuf(stderr, options);
	WARN "Usage: checkout [options] [working_or_archive [...]]\n\
Options (from \"co\"):\n\
\t-l[rev]\tlocks the checked-out revision for the caller.\n\
\t-q[rev]\tquiet mode\n\
\t-r[rev]\tretrieves the latest revision whose number is\n\
\t\tless than or equal to \"rev\".\n\
\t-cdate\tretrieves the latest revision on the selected\n\
\t\tbranch whose checkin date/time is less than or\n\
\t\tequal to \"date\", in the format\n\
\t\t\tyy/mm/dd hh:mm:ss\n\
\t-sstate\tretrieves the latest revision on the selected\n\
\t\tbranch whose state is set to state.\n\
\t-w[login] retrieves the latest revision on the selected\n\
\t\tbranch which was checked in by user \"login\".\n\
Unimplemented \"co\" options:\n\
\t-jjoinlist generates a new revision which is the join of\n\
\t\tthe revisions on joinlist\n");
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
				catarg(options, argv[j]);
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
				} else {
					WARN "?? Unknown option: %s\n", s-1);
					usage();
				}
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

failed(s)
char	*s;
{
	perror(s);
	(void)exit(FAIL);
}
