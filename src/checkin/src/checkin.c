#ifndef	lint
static	char	sccs_id[] = "@(#)checkin.c	1.18 88/08/30 15:55:05";
#endif	lint

/*
 * Title:	checkin.c (RCS checkin front-end)
 * Author:	T.E.Dickey
 * Created:	19 May 1988, from 'sccsbase'
 * Modified:
 *		30 Aug 1988, make this work as a setuid process.  If file has
 *			     not been checked-in, initialize its access list
 *			     first.
 *		24 Aug 1988, added 'usage()'; create directory if not found.
 *		15 Aug 1988, use CI_PATH to control where we install 'ci'.
 *			     If "-k" option is used, assume time+date from the
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
 * patch:
 *		Should check access-list in pre-process part to ensure that
 *		user is permitted to check file in.  Or, Pyster wants to
 *		prohibit users from checkin files in (though they may have the
 *		right to make locks for checking files out).
 *
 *		Want the ability to set the default checkin-version on a global
 *		basis (i.e., for directory) to other than 1.1 (again, pyster).
 *
 * patch:	Since 'ci' uses 'access()' to verify that it can put a semaphore
 *		in the RCS directory, I don't see any simple way of handling
 *		this except to make the RCS directory temporarily publicly
 *		writeable ...
 */

#include	"ptypes.h"
#include	"rcsdefs.h"

#include	<ctype.h>
#include	<pwd.h>
#include	<time.h>
extern	struct	tm *localtime();
extern	FILE	*tmpfile();
extern	long	packdate();
extern	char	*getenv();
extern	char	*getuser();
extern	char	*rcslocks();
extern	char	*rcs2name(), *name2rcs();
extern	char	*rcsread(), *rcsparse_id(), *rcsparse_num(), *rcsparse_str();
extern	char	*rcstemp();
extern	char	*strcat();
extern	char	*strchr(), *strrchr();
extern	char	*strcpy();
extern	char	*strncpy();

/* local declarations: */
#define	REV_OPT	"rfluqk"

#define	WARN	FPRINTF(stderr,
#define	TELL	if (!silent) PRINTF

static	FILE	*fpT;
static	int	silent	= FALSE,
		locked	= FALSE,	/* set if file is re-locked */
		from_keys = FALSE,	/* set if we get date from RCS file */
		RCS_uid,		/* archive's owner */
		RCSprot;		/* protection of RCS-directory */
static	time_t	modtime;
static	char	*Working,
		*Archive,
		RCSdir[BUFSIZ],
		opt_all[BUFSIZ],	/* options for 'ci' */
		opt_rev[BUFSIZ],
		old_date[BUFSIZ],
		new_date[BUFSIZ];

/************************************************************************
 *	local procedures						*
 ************************************************************************/

/*
 * If the given file is still checked-out, touch its time.
 * patch: should do keyword substitution for Header, Date a la 'co'.
 */
static
ReProcess ()
{
FILE	*fpS;
struct	stat	sb;
int	len	= strlen(old_date),
	mode,
	lines	= 0,
	changed	= 0;		/* number of substitutions done */
char	*s, *d,
	bfr[BUFSIZ];

	(void) rewind (fpT);

	if (!from_keys) {

		/* Alter the delta-header to match RCS's substitution */
		/* 0123456789.123456789. */
		/* yy.mm.dd.hh.mm.ss */
		new_date[ 2] = old_date[ 2] =
		new_date[ 5] = old_date[ 5] = '/';
		new_date[ 8] = old_date[ 8] = ' ';
		new_date[11] = old_date[11] =
		new_date[14] = old_date[14] = ':';

		if (!(fpS = fopen(Working, "r")))
			return;

		while (fgets(bfr, sizeof(bfr), fpS)) {
		char	*last = bfr + strlen(bfr) - len;

			lines++;
			if ((last > bfr)
			&&  (d = strchr(bfr, '$'))) {
				while (d <= last) {
					if (!strncmp(d, old_date, len)) {
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
	}

	if ((stat(Working, &sb) >= 0)
	&&  ((sb.st_mode & S_IFMT) == S_IFREG)) {
		mode = sb.st_mode & 0777;
		if (changed) {
			char	*UidHack = rcstemp(Working, FALSE);
			(void)copyback(fpT, UidHack, mode, lines);
			if (strcmp(UidHack,Working)) {
				if (usercopy(UidHack, Working) < 0)
					GiveUp("recopy \"%s\"");
			}
		}
		if (!locked)		/* cover up bugs on Apollo acls */
			mode &= ~0222;
		if (userprot(Working, mode, modtime) < 0)
			GiveUp("touch \"%s\" failed");
	}
}

/*
 * Post-process a single RCS-file, replacing its check-in date with the file's
 * modification date.
 */
static
PostProcess ()
{
int	header	= TRUE,
	match	= FALSE;
char	*s	= 0,
	*old,
	revision[BUFSIZ],
	token[BUFSIZ];

	if (!rcsopen(Archive, !silent))
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
			struct	tm *t;
				newzone(5,0,FALSE);	/* format for EST5EDT */
				if (from_keys) {
				int	year, mon, day, hour, min, sec;
					if (sscanf(old_date, FMT_DATE,
						&year, &mon, &day,
						&hour, &min, &sec) == 6) {
						modtime = packdate(1900+year,
							mon, day, hour, min,
							sec);
					}
				} else {
					t = localtime(&modtime);
					FORMAT(new_date, FMT_DATE,
						t->tm_year, t->tm_mon + 1,
						t->tm_mday, t->tm_hour,
						t->tm_min,  t->tm_sec);
					rcsedit(old, old_date, new_date);
				}
				oldzone();
				TELL("** revision %s\n", revision);
				TELL("** modified %s", ctime(&modtime));
			}
			break;
		case S_DESC:
			header = FALSE;
		}
	}
	rcsclose();
}

/*
 * The RCS utilities (ci and rcs) use the unix 'access()' function to test if
 * the caller has access to the archive-directory.  This is not done properly,
 * but I cannot easily code around it other than by temporarily changing the
 * directory's protection.
 */
static
HackMode(newmode)
{
	int	need;

	if (newmode == 0) {
		if ((RCSdir[0] != EOS)
		&&  (getuid()  != geteuid())) {
			need = (getegid() == getgid()) ? 0775 : 0777;
			if (need != RCSprot) {
				newmode = RCSprot;
				if (chmod(RCSdir, need) < 0)
					failed(RCSdir);
			}
		}
	} else
		(void)chmod(RCSdir, newmode);
	return (newmode);
}

/*
 * Check in the file using the RCS 'ci' utility.  If 'ci' does something, then
 * it will delete or modify the checked-in file -- return TRUE.  If no action
 * is taken, return false.
 */
static
Execute()
{
	char	*UidHack;
	char	cmds[BUFSIZ];
	struct	stat	sb;
	int	code,
		mode	= HackMode(0);

	UidHack = rcstemp(Working, TRUE);

	FORMAT(cmds, "%s%s %s", opt_all, Archive, UidHack);
	TELL("** ci %s\n", cmds);
	code = execute(CI_PATH, cmds);
	(void)HackMode(mode);			/* ...restore protection */

	if (code >= 0) {			/* ... check-in file ok */
		if (stat(UidHack, &sb) >= 0) {	/* working file not deleted */
			if (strcmp(UidHack,Working)) {
				if (sb.st_mtime != modtime)
					if (filecopy(UidHack,Working,TRUE) < 0)
						failed(Working);
				(void)unlink(UidHack);
			}
			if (sb.st_mtime == modtime) {
				TELL("** checkin was not performed\n");
				return (FALSE);
			}
		} else if (strcmp(UidHack, Working)
		&&	   unlink(Working) < 0)
			failed(Working);
		return (TRUE);
	}
	return (FALSE);
}

/*
 * This procedure is invoked only if 'SetAccess()' finds an RCS-archive.
 * Ensure that we have a unique lock-value.  If the user did not specify one,
 * we must get it from the archived-file.
 */
static
GetLock()
{
	int	header	= TRUE;
	char	*s	= 0,
		tip[BUFSIZ],
		key[BUFSIZ],
		tmp[BUFSIZ];

	if (*opt_rev == EOS) {

		if (!rcsopen(Archive, -(!silent)))
			return (FALSE);	/* could not open file anyway */

		while (header && (s = rcsread(s))) {
			s = rcsparse_id(key, s);

			switch (rcskeys(key)) {
			case S_HEAD:
				s = rcsparse_num(tip, s);
				break;
			case S_LOCKS:
				*tmp = EOS;
				s = rcslocks(s, strcpy(key, getuser()), tmp);
				if (*tmp) {
					TELL("** revision %s was locked\n",tmp);
					if (!strcmp(tip, tmp)) {
					char	*t = strrchr(tmp, '.');
					int	last;
						(void)sscanf(t, ".%d", &last);
						FORMAT(t, ".%d", last+1);
						(void)strcpy(opt_rev, tmp);
					} else {
						TELL("?? branching not supported\n");
						/* patch:finish this later... */
					}
				} else {
					TELL("?? no lock set by %s\n", key);
				}
				/* fall-thru to force exit */
			case S_VERS:
				header = FALSE;
				break;
			case S_COMMENT:
				s = rcsparse_str(s, NULL_FUNC);
				break;
			}
		}
		rcsclose();
	}
	return (*opt_rev != EOS);
}

/*
 * Before checkin, verify that the file exists, and obtain its modification
 * time/date.
 */
static
time_t
PreProcess(name)
char	*name;
{
struct	stat	sb;

	if (stat(name, &sb) >= 0) {
		if ((S_IFMT & sb.st_mode) == S_IFREG)
			return (sb.st_mtime);
		GiveUp("not a file: %s");
	}
	return (0);
}

/*
 * If the RCS archive does not already exist, make one, with an access list
 * properly initialized (i.e., including the current user and the owner of the
 * RCS directory.  Returns FALSE if the archive already exists.
 */
static
SetAccess()
{
	auto	char	cmds[BUFSIZ];
	static	char	list[BUFSIZ];
	auto	int	mode;

	if (PreProcess(Archive) != 0)
		return (FALSE);

	mode = HackMode(0);		/* ...save protection */
	cmds[0] = EOS;
	catarg(cmds, "-i");
	if (!*list) {
		FORMAT(list, "-a%s", getuser());
		if (getuid() != geteuid()) {
			register struct passwd *p;
			if (p = getpwuid(RCS_uid))
				FORMAT(list + strlen(list), ",%s", p->pw_name);
			else
				GiveUp("owner of RCS directory for %s");
		}
	}
	catarg(cmds, list);
	if (silent) catarg(cmds, "-q");
	catarg(cmds, Archive);
	TELL("** rcs %s\n", cmds);
	if (execute("rcs", cmds) < 0)
		GiveUp("rcs initialization for %s");
	(void)HackMode(mode);		/* ...restore protection */
	return (TRUE);
}

/*
 * If the RCS-directory does not exist, make it.  Save the name in 'RCSdir[]'
 * in case we need it for the setuid-hack in 'Execute()'.
 */
static
MakeDirectory()
{
	struct	stat	sb;
	char	*s;
	int	len;

	if (s = strrchr(Archive, '/')) {
		len = s - Archive;
		if (len == strlen(RCSdir)
		&& !strncmp(RCSdir, Archive, len))
			return (TRUE);	/* same as last directory */

		(strncpy(RCSdir, Archive, len))[len] = EOS;

		if (stat(RCSdir, &sb) >= 0) {
			RCS_uid = sb.st_uid;
			RCSprot = sb.st_mode & 0777;
			if ((sb.st_mode & S_IFMT) == S_IFDIR)
				return (TRUE);
			TELL("?? not a directory: %s\n", RCSdir);
			(void)exit(FAIL);
		} else {
			TELL("** make directory %s\n", RCSdir);
			if (mkdir(RCSdir, 0755) < 0)
				failed(RCSdir);
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
SetOpts(argc,argv,code)
char	*argv[];
{
	int	last	= 0;
	int	logmsg	= 0;
	register int	j;
	char	bfr[BUFSIZ];
	char	*dft	= (code < 0) ? getenv("RCS_BASE") : 0;
	register char *s;

	*opt_rev = EOS;
	*opt_all = EOS;
	for (j = 1; j < argc; j++) {
		s = argv[j];
		if (*s == '-') {
			catarg(opt_all, s);
			if (*++s == 'q')
				silent++;
			if (strchr(REV_OPT, *s)) {
				if (*s == 'l')	locked = TRUE;
				if (*(++s))
					(void)strcpy(opt_rev, s);
				last = j;
			} else if (*s == 'm')
				logmsg = TRUE;
		}
	}

	/*
	 * If no revision was specified, and there is a default in effect,
	 * reprocess the list of options so that it includes the default
	 * revision.  Also, if no log-message was specified, add one so we
	 * don't get prompted unnecessarily.
	 */
	if ((*opt_rev == EOS)
	&&  (!from_keys)
	&&  (dft != 0)) {
		if (last) {
			*opt_all = EOS;
			for (j = 1; j < argc; j++) {
				s = argv[j];
				if (*s == '-') {
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
		if (!logmsg) {
			catarg(opt_all, "-mRCS_BASE");
		}
	} else if (from_keys && (*opt_rev == EOS) && (!logmsg))
		catarg(opt_all, "-mFROM_KEYS");
}

static
GiveUp(fmt)
char	*fmt;
{
	char	msg[BUFSIZ];
	FORMAT(msg, fmt, Working);
	TELL("?? %s\n", msg);
	exit(FAIL);
}

static
usage()
{
	setbuf(stderr, opt_all);
	WARN "Usage: checkin [-options] [working_or_archive [...]]\n\
Options (from \"ci\"):\n\
\t-r[rev]\tassigns the revision number \"rev\" to the checked-in revision\n\
\t-f[rev]\tforces a deposit\n\
\t-k[rev]\tobtains keywords from working file (rev overrides)\n\
\t-l[rev]\tlike \"-r\", but follows with \"co -l\"\n\
\t-u[rev]\tlike \"-l\", but no lock is made\n\
\t-q[rev]\tquiet mode\n\
\t-mmsg\tspecifies log-message \"msg\"\n\
\t-nname\tassigns symbolic name to the checked-in revision\n\
\t-Nname\tlike \"-n\", but overrides previous assignment\n\
\t-sstate\tsets the revision-state (default: \"Exp\")\n\
\t-t[txtfile] writes descriptive text into the RCS file\n");
	exit(FAIL);
}

/************************************************************************
 *	main program							*
 ************************************************************************/

main (argc, argv)
char	*argv[];
{
	register int	j;
	int		code;
	register char	*s;

	fpT = tmpfile();

	/*
	 * Process the argument list
	 */
	for (j = 1; j < argc; j++) {
		s = argv[j];
		if (*s == '-') {
			if (*(++s) == 'q')
				silent++;
			else if (*s == 'k')
				from_keys++;
			if (strchr("rfkluqmnNst", *s) == 0) {
				WARN "unknown option: %s\n", s-1);
				usage();
			}
		} else {

			Working = rcs2name(s),
			Archive = name2rcs(s);
			code = -1;

			if (!(modtime = PreProcess (Working)))
				GiveUp("file not found: %s");

			if (MakeDirectory()
			&&  (SetAccess()
			||   (code = GetLock()))) {
				SetOpts(j,argv,code);
				if (Execute()) {
					PostProcess();
					ReProcess();
				}
			}
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
