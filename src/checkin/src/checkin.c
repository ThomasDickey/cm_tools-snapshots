#ifndef	lint
static	char	sccs_id[] = "@(#)checkin.c	1.3 88/05/21 12:30:33";
#endif	lint

/*
 * Title:	checkin.c (RCS checkin front-end)
 * Author:	T.E.Dickey
 * Created:	19 May 1988, from 'sccsbase'
 * Modified:
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
 */

#include	<ptypes.h>

#include	<stdio.h>
#include	<ctype.h>
#include	<time.h>
#include	<signal.h>
extern	struct	tm *localtime();
extern	FILE	*tmpfile();
extern	long	adj2est();
extern	char	*strcat();
extern	char	*strchr();
extern	char	*strcpy();

/* local declarations: */
#define	TRUE	1
#define	FALSE	0
#define	CHECKIN	"ci"

#define	PRINTF	(void) printf
#define	FORMAT	(void) sprintf
#define	SHOW	if (ShowIt(name,TRUE))  PRINTF
#define	TELL	if (ShowIt(name,FALSE)) PRINTF

static	FILE	*fpT;
static	int	copy_opt = TRUE,
		silent   = FALSE, ShowedIt;
static	char	options[BUFSIZ];	/* options for 'ci' */
static	char	old_date[BUFSIZ],
		new_date[BUFSIZ];

/************************************************************************
 *	local procedures						*
 ************************************************************************/

/*
 * Parse for a keyword, converting the buffer to hold the value of the keyword
 * if it is found.
 */
static
keyRCS(string, key)
char	*string, *key;
{
char	*s,
	first[BUFSIZ],
	second[BUFSIZ];

	if (s = strchr(string, ';')) {
		*s = '\0';
		if (sscanf(string, "%s %s;", first, second) == 2)
			if (!strcmp(first, key)) {
				(void)strcpy(string, second);
				return (TRUE);
			}
	}
	return (FALSE);
}

snag (sig)			/* ignore signals we don't want */
{
	(void) signal (sig, snag);
	PRINTF("\007");
}

/*
 * Copy the temporary file back to the specified name
 */
static
copyback (name, mode, lines)
char	*name;
{
FILE	*fpS;
char	bfr[BUFSIZ];

	if (chmod(name, 0644)) {
		TELL ("** cannot write-enable \"%s\"\n", name);
		return (FALSE);
	}
	(void) signal (SIGINT, snag);
	(void) signal (SIGQUIT, snag);
	if (fpS = fopen (name, "w")) {
		(void) rewind (fpT);
		while (lines-- > 0) {
			(void) fgets (bfr, sizeof(bfr), fpT);
			(void) fputs (bfr, fpS);
		}
		(void) fclose (fpS);
		(void) chmod (name, mode);
		(void) signal (SIGINT, SIG_DFL);
		(void) signal (SIGQUIT, SIG_DFL);
		return (TRUE);
	}
	return (FALSE);
}

/*
 * Show the current filename, once before the first message applying to it.
 * If the silent-option is inactive, each filename is shown even if no messages
 * apply to it.
 */
ShowIt (name,doit)
char	*name;
{
	if (!ShowedIt && (doit || !silent)) {
		PRINTF ("File \"%s\"\n", name);
		ShowedIt++;
	}
	return (doit || !silent);
}

/*
 * If the given file is still checked-out, touch its time.
 * patch: should do keyword substitution for Header, Date a la 'co'.
 */
ReProcess (name, mtime)
char	*name;
time_t	mtime;
{
FILE	*fpS;
struct	stat	sb;
int	len	= strlen(old_date),
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
	if (changed) {
		(void)copyback(name, (int)(sb.st_mode & 0777), lines);
	}
	(void)setmtime (name, mtime); /* update the file modification times */
}

/*
 * Post-process a single RCS-file, replacing its check-in date with the file's
 * modification date.
 */
#define	S_HEAD	0	/* head <version_string>;	*/
#define	S_HEAD2	1	/* <more header lines>		*/
#define	S_SKIP	2	/* <blank lines>		*/
#define	S_VERS	3	/* <version_string>		*/
#define	S_DATE	4	/* date <date>; <some text>	*/
#define	S_COPY	5
#define	S_FAIL	6

PostProcess (name, mtime)
char	*name;
time_t	mtime;
{
FILE	*fpS;
int	lines	= 0;
unsigned
int	j,
	state	= S_HEAD,
	changed = FALSE;
char	*s,
	path[BUFSIZ],
	vstring[BUFSIZ],
	bfr[BUFSIZ];

	ShowedIt = FALSE;
	FORMAT (path, "RCS/%s,v", name);

	(void) rewind (fpT);
	if ((fpS = fopen (path, "r")) == 0) {
		TELL ("** cannot open: %s\n", path);
		return;
	}

	while ((state < S_FAIL) && fgets (bfr, sizeof(bfr), fpS)) {
		lines++;
		switch (state) {
		case S_HEAD:
			if (keyRCS(strcpy(vstring, bfr), "head")) {
				(void)strcat(vstring, "\n");
				state = S_HEAD2;
				for (s = vstring; s[1]; s++) {
					if (!isdigit(*s) && *s != '.') {
						state = S_FAIL;
						break;
					}
				}
			} else
				state = S_FAIL;
			break;
		case S_HEAD2:
			if (!strcmp(bfr,"\n"))		state = S_SKIP;
			break;
		case S_SKIP:
			if (!strcmp(bfr,"\n"))		break;
			/* fall-thru */
		case S_VERS:
			if (!strcmp(bfr, vstring))	state = S_DATE;
			else				state = S_FAIL;
			break;
		case S_DATE:
			if (keyRCS(strcpy(old_date, bfr), "date")) {
				for (s = bfr; !isdigit(*s) && *s; s++);
				if (*s) {
				time_t	xtime	= mtime + adj2est(FALSE);
				struct	tm *t	= localtime(&xtime);
				TELL ("   old: %s", bfr);
					FORMAT(new_date,
						"%02d.%02d.%02d.%02d.%02d.%02d",
						t->tm_year, t->tm_mon + 1,
						t->tm_mday, t->tm_hour,
						t->tm_min,  t->tm_sec);
					for (j = 0; new_date[j]; j++)
						s[j] = new_date[j];
				}
				state = S_COPY;
				TELL ("   new: %s", bfr);
				changed++;
			} else
				state = S_FAIL;
		}
		(void) fputs (bfr, fpT);
	}
	(void) fclose (fpS);

	/*
	 * If we wrote an altered delta-date to the temp-file, recopy it
	 * back over the original RCS-file:
	 */
	if (changed && copy_opt) {
		if (copyback(path, 0444, lines)) {
			SHOW ("** %d lines processed\n", lines);
			(void) fflush (stdout);
		}
	} else if (copy_opt) {
		TELL ("** no changes made to \"%s\"\n", name);
	} else if (!changed) {
		TELL ("** no changes would be made\n");
	} else {
		SHOW ("** change would be made\n");
	}
}

/*
 * Check in the file using the RCS 'ci' utility.  If 'ci' does something, then
 * it will delete or modify the checked-in file -- return TRUE.  If no action
 * is taken, return false.
 */
Execute(name, mtime)
char	*name;
time_t	mtime;
{
char	cmds[BUFSIZ];
struct	stat	sb;

	if (execute(CHECKIN, strcat(strcpy(cmds, options), name)) >= 0) {
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
 * Before checkin, verify that the file exists, and obtain its modification
 * time/date.
 */
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

/************************************************************************
 *	main program							*
 ************************************************************************/

main (argc, argv)
char	*argv[];
{
int	j;
time_t	mtime;

	fpT = tmpfile();

	(void)strcpy(options, " ");

	for (j = 1; j < argc; j++) {
	char	*s = argv[j];
		if (*s == '-') {
			if (*(++s) == 'd')
				copy_opt = FALSE;
			else {
				(void)strcat(strcat(options, argv[j]), " ");
				if (*s == 'q')
					silent++;
			}
		} else if (mtime = PreProcess (s)) {
			if (Execute(s, mtime)) {
				PostProcess (s, mtime);
				ReProcess(s, mtime);
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
