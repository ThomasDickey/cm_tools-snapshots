#ifndef	lint
static	char	sccs_id[] = "@(#)checkin.c	1.8 88/07/08 07:05:46";
#endif	lint

/*
 * Title:	checkin.c (RCS checkin front-end)
 * Author:	T.E.Dickey
 * Created:	19 May 1988, from 'sccsbase'
 * Modified:
 *		08 Jul 1988, set "silent" before first call on GetLock if "-q"
 *			     precedes filename in argv.
 *		01 Jul 1988, added chmod to fix cases in which 'ci' leaves the
 *			     file writeable (Apollo bug?).  Added interpretation
 *			     for environment variable which specifies base
 *			     value for revision if none is given in the options
 *			     list.
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
 * Options:	those recognized by 'ci', as well as '-d' for debugging.
 *
 * patch:
 *		Should check access-list in pre-process part to ensure that
 *		user is permitted to check file in.  Or, Pyster wants to
 *		prohibit users from checkin files in (though they may have the
 *		right to make locks for checking files out).
 *
 *		Want the ability to set the default checkin-version on a global
 *		basis (i.e., for directory) to other than 1.1 (again, pyster).
 */

#include	"ptypes.h"
#include	"rcsdefs.h"

#include	<stdio.h>
#include	<ctype.h>
#include	<time.h>
extern	struct	tm *localtime();
extern	FILE	*tmpfile();
extern	char	*getenv();
extern	char	*getuser();
extern	char	*rcsname();
extern	char	*rcsread(), *rcsparse_id(), *rcsparse_num(), *rcsparse_str();
extern	char	*strcat();
extern	char	*strchr(), *strrchr();
extern	char	*strcpy();

/* local declarations: */
#define	EOS	'\0'
#define	TRUE	1
#define	FALSE	0
#define	CHECKIN	"ci"
#define	REV_OPT	"rfluq"			/* patch: no support for -k */

#define	PRINTF	(void) printf
#define	FORMAT	(void) sprintf
#define	SHOW	if (ShowIt(name,TRUE))  PRINTF
#define	TELL	if (ShowIt(name,FALSE)) PRINTF

static	FILE	*fpT;
static	int	silent	= FALSE,
		locked	= FALSE,	/* set if file is re-locked */
		ShowedIt;
static	char	opt_all[BUFSIZ],	/* options for 'ci' */
		opt_rev[BUFSIZ],
		old_date[BUFSIZ],
		new_date[BUFSIZ];

/************************************************************************
 *	local procedures						*
 ************************************************************************/

/*
 * Show the current filename, once before the first message applying to it.
 * If the silent-option is inactive, each filename is shown even if no messages
 * apply to it.
 */
static
ShowIt (name,doit)
char	*name;
{
int	show	= (doit || !silent);
	if (show && !ShowedIt) {
		PRINTF ("File \"%s\"\n", name);
		ShowedIt++;
	}
	return (show);
}

/*
 * If the given file is still checked-out, touch its time.
 * patch: should do keyword substitution for Header, Date a la 'co'.
 */
static
ReProcess (name, mtime)
char	*name;
time_t	mtime;
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

	/* Alter the delta-header to match RCS's substitution */
	/* 0123456789.123456789. */
	/* yy.mm.dd.hh.mm.ss */
	new_date[ 2] = old_date[ 2] =
	new_date[ 5] = old_date[ 5] = '/';
	new_date[ 8] = old_date[ 8] = ' ';
	new_date[11] = old_date[11] =
	new_date[14] = old_date[14] = ':';

	if (!(fpS = fopen(name, "r")))
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
	(void) fclose(fpS);

	(void)stat(name, &sb);
	mode = sb.st_mode & 0777;
	if (changed) {
		(void)copyback(fpT, name, mode, lines);
	}
	if (!locked) {		/* cover up bugs on Apollo acls */
		mode &= ~0222;
		if (chmod(name, mode) < 0)
			SHOW("?? \"chmod %03o %s\" failed\n", mode, name);
	}
	if (setmtime (name, mtime) < 0) /* update the file modification times */
		SHOW("?? touch \"%s\" failed\n", name);
}

/*
 * Post-process a single RCS-file, replacing its check-in date with the file's
 * modification date.
 */
static
PostProcess (name, mtime)
char	*name;
time_t	mtime;
{
int	header	= TRUE,
	match	= FALSE;
char	*s	= 0,
	*old,
	revision[BUFSIZ],
	token[BUFSIZ];

	if (!rcsopen(name, !silent))
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
			s = rcsparse_str(s);
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
				t = localtime(&mtime);
				FORMAT(new_date, FMT_DATE,
					t->tm_year, t->tm_mon + 1,
					t->tm_mday, t->tm_hour,
					t->tm_min,  t->tm_sec);
				rcsedit(old, old_date, new_date);
				oldzone();
				TELL("** revision %s\n", revision);
				TELL("** modified %s", ctime(&mtime));
			}
			break;
		case S_DESC:
			header = FALSE;
		}
	}
	rcsclose();
}

/*
 * Check in the file using the RCS 'ci' utility.  If 'ci' does something, then
 * it will delete or modify the checked-in file -- return TRUE.  If no action
 * is taken, return false.
 */
static
Execute(name, mtime)
char	*name;
time_t	mtime;
{
char	cmds[BUFSIZ];
struct	stat	sb;

	if (execute(CHECKIN, strcat(strcpy(cmds, opt_all), name)) >= 0) {
		if (stat(name, &sb) >= 0) {
			if (sb.st_mtime == mtime) {
				SHOW("** checkin was not performed\n");
				return (FALSE);
			}
		}
		return (TRUE);
	}
	return (FALSE);
}

/*
 * Ensure that we have a unique lock-value.  If the user did not specify one,
 * we must get it from the archived-file.
 */
static
GetLock(name)
char	*name;
{
struct	stat	sb;
int	header	= TRUE;
char	*s	= 0,
	*user	= getuser(),
	tip[BUFSIZ],
	key[BUFSIZ],
	tmp[BUFSIZ];

	if (*opt_rev == EOS) {
		if (stat(rcsname(name), &sb) < 0)
			return (-1);	/* initial checkin */

		if (!rcsopen(name, !silent))
			return (FALSE);	/* could not open file anyway */

		while (header && (s = rcsread(s))) {
			s = rcsparse_id(key, s);

			switch (rcskeys(key)) {
			case S_HEAD:
				s = rcsparse_num(tip, s);
				break;
			case S_LOCKS:
				do {
					s = rcsparse_id(key, s);
					if (*s == ':')	s++;
					s = rcsparse_num(tmp, s);
					if (!strcmp(key, user))
						header = FALSE;
				} while (*key && header);
				if (header) {
					SHOW("?? no lock set by %s\n", user);
					header = FALSE;
				} else {
					TELL("** revision %s was locked\n",tmp);
					if (!strcmp(tip, tmp)) {
					char	*t = strrchr(tmp, '.');
					int	last;
						(void)sscanf(t, ".%d", &last);
						FORMAT(t, ".%d", last+1);
						(void)strcpy(opt_rev, tmp);
					} else {
						SHOW("?? branching not supported\n");
						/* patch:finish this later... */
					}
				}
				break;
			case S_COMMENT:
				s = rcsparse_str(s);
				break;
			case S_VERS:
				header = FALSE;
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
		TELL ("** ignored (not a file)\n");
		return (0);
	}
	TELL ("** file not found\n");
	return (0);
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
	(void)strcpy(opt_all, " ");
	for (j = 1; j < argc; j++) {
		s = argv[j];
		if (*s == '-') {
			catarg(opt_all, s);
			if (*++s == 'q')
				silent++;
			if (strchr(REV_OPT, *s)) {
				if (*s == 'l')	locked = TRUE;
				(void)strcpy(opt_rev, ++s);
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
	&&  (dft != 0)) {
		if (last) {
			(void)strcpy(opt_all, " ");
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
	}
}

/************************************************************************
 *	main program							*
 ************************************************************************/

main (argc, argv)
char	*argv[];
{
	register int	j;
	time_t		mtime;
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
		} else {
			ShowedIt = FALSE;
			if (mtime = PreProcess (s)) {
				if (code = GetLock(s)) {
					SetOpts(j,argv,code);
					if (Execute(s, mtime)) {
						PostProcess (s, mtime);
						ReProcess(s, mtime);
					}
				}
			}
		}
	}
	(void)exit(0);
	/*NOTREACHED*/
}

failed(s)
char	*s;
{
	perror(s);
	(void)exit(1);
}
