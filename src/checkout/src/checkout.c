#ifndef	lint
static	char	sccs_id[] = "@(#)checkout.c	1.4 88/05/21 13:54:53";
#endif	lint

/*
 * Title:	checkout.c (front end for RCS checkout)
 * Author:	T.E.Dickey
 * Created:	20 May 1988 (from 'sccsdate.c')
 * Modified:
 *
 * Function:	Display the date of a specified SCCS-sid of a given file,
 *		optionally altering the modification date to correspond with
 *		the delta-date.
 *
 * Options:	This passes through to 'co' all options except the '-d' (date)
 *		option -- we interpret a '-c' in SCCS-style to simplify the
 *		implementation.
 *
 * patch:	This interprets now by version number only, with a default for
 *		the most-recent version.  Must make it work ok for cutoff.
 *
 *		Must translate '-c' option to RCS's '-d' option.
 */

#include	<ptypes.h>

#include	<stdio.h>
#include	<ctype.h>
#include	<time.h>
extern	long	packdate();
extern	char	*ctime();
extern	char	*doalloc();
extern	char	*strcat();
extern	char	*strcpy();
extern	char	*strchr();
extern	time_t	adj2est();
extern	time_t	cutoff();

extern	char	*optarg;
extern	int	optind;

/* local definitions */
#define	TRUE	1
#define	FALSE	0
#define	CHECKOUT	"co"

#define	PRINTF	(void) printf
#define	TELL	if (!silent) PRINTF

static	time_t	opt_c	= 0;
static	int	silent;
static	char	options[BUFSIZ];	/* options to pass to 'co' */
static	char	opt_rev[BUFSIZ];	/* revision to find */

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

#define	S_HEAD	0	/* head <version_string>;	*/
#define	S_HEAD2	1	/* <more header lines>		*/
#define	S_SKIP	2	/* <blank lines>		*/
#define	S_VERS	3	/* <version_string>		*/
#define	S_DATE	4	/* date <date>; <some text>	*/
#define	S_COPY	5
#define	S_FAIL	6

/*
 * Postprocess the checkout by scanning the RCS file to find the delta-date
 * to which the checked-out file corresponds, then touching the checked-out
 * file to make it correspond.
 */
PostProcess(name)
char	*name;
{
FILE	*fpS;
time_t	ok	= 0;		/* must set this to touch file */
char	bfr[BUFSIZ],
	tmp[BUFSIZ],
	*s;
int	state	= S_HEAD;

	(void)strcat(strcat(strcpy(bfr, "RCS/"), name), ",v");
	if (!(fpS = fopen(bfr, "r"))) {
		PRINTF("?? Cannot open \"%s\"\n", bfr);
		return;
	}

	while ((state < S_FAIL) && fgets (bfr, sizeof(bfr), fpS)) {

		switch (state) {
		case S_HEAD:
			if (keyRCS(strcpy(tmp, bfr), "head")) {
				(void)strcat(tmp, "\n");
				if (!*opt_rev)
					(void)strcpy(opt_rev, tmp);
				state = S_HEAD2;
				for (s = tmp; s[1]; s++) {
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
			if (!strcmp(bfr, "desc\n"))	state = S_FAIL;
			else if (dotcmp(bfr, opt_rev) >= 0)	state = S_DATE;
			else				state = S_FAIL;
			break;
		case S_DATE:
			if (keyRCS(strcpy(tmp, bfr), "date")) {
			time_t	tt;
			int	yd, md, dd, ht, mt, st;
				if (sscanf(tmp,
					"%02d.%02d.%02d.%02d.%02d.%02d",
					&yd, &md, &dd, &ht, &mt, &st) == 6) {
					tt = packdate(1900+yd, md,dd, ht,mt,st);
					ok = tt;	/* patch: cutoff? */
					state = S_HEAD2;
				} else
					state = S_FAIL;
			} else
				state = S_FAIL;
		}
	}

	if (ok)
		if (setmtime(name, ok + adj2est(TRUE)) < 0)
			PRINTF("?? touch \"%s\" failed\n", name);
}

/*
 * Check out the file using the RCS 'co' utility.  If 'co' does something, then
 * it will delete or modify the checked-in file -- return TRUE.  If no action
 * is taken, return false.
 */
Execute(name, mtime)
char	*name;
time_t	mtime;
{
char	cmds[BUFSIZ];
struct	stat	sb;

	if (execute(CHECKOUT, strcat(strcpy(cmds, options), name)) >= 0) {
		if (stat(name, &sb) >= 0) {
			if (sb.st_mtime == mtime) {
				TELL("** checkout was not performed\n");
				return (FALSE);
			}
			return (TRUE);
		}
	}
	return (FALSE);
}

/*
 * Before checkout, verify that the file exists, and obtain its modification
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
	return (1);	/* file was not already checked out */
}

/************************************************************************
 *	main program							*
 ************************************************************************/
main (argc, argv)
char	*argv[];
{
int	j;
time_t	mtime;

	(void)strcpy(options, " ");

	for (j = 1; j < argc; j++) {
	char	*s = argv[j];
		if (*s == '-') {
			if (*(++s) == 'c') {
				optind = j;
				optarg = ++s;
				opt_c = cutoff(argc, argv);
				j = optind - 1;
				TELL("cutoff: %s", ctime(&opt_c));
			} else {
				(void)strcat(strcat(options, argv[j]), " ");
				if (*s == 'q')
					silent++;
				if (strchr("lqr", *s)) {
					if (s[1])
						(void)strcat(strcpy(opt_rev, s+1),"\n");
				} else if (strchr("swj", *s)) {
					PRINTF("Option not implemented: %s\n",
						argv[j]);
					break;
				} else {
					PRINTF("Illegal option: %s\n", argv[j]);
					break;
				}
			}
		} else if (mtime = PreProcess (s)) {
		char	save_rev[BUFSIZ];
			(void)strcpy(save_rev, opt_rev);
			if (Execute(s, mtime))
				if (PreProcess (s))
					PostProcess (s);
			(void)strcpy(opt_rev, save_rev);
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
