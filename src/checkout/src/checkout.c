#ifndef	lint
static	char	sccs_id[] = "@(#)checkout.c	1.7 88/05/27 12:19:18";
#endif	lint

/*
 * Title:	checkout.c (front end for RCS checkout)
 * Author:	T.E.Dickey
 * Created:	20 May 1988 (from 'sccsdate.c')
 * Modified:
 *		27 May 1988, recoded using 'rcsedit' module.
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

#include	"ptypes.h"
#include	"rcsdefs.h"

#include	<stdio.h>
#include	<ctype.h>
#include	<time.h>
extern	long	packdate();
extern	char	*ctime();
extern	char	*doalloc();
extern	char	*rcsread(), *rcsparse_id(), *rcsparse_num(), *rcsparse_str();
extern	char	*strcat();
extern	char	*strcpy();
extern	char	*strchr();
extern	time_t	adj2est();
extern	time_t	cutoff();

extern	char	*optarg;
extern	int	optind;

/* local definitions */
#define	EOS	'\0'
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
 * Postprocess the checkout by scanning the RCS file to find the delta-date
 * to which the checked-out file corresponds, then touching the checked-out
 * file to make it correspond.
 */
static
PostProcess(name)
char	*name;
{
time_t	ok	= 0;		/* must set this to touch file */
char	key[BUFSIZ],
	tmp[BUFSIZ],
	*s	= 0;
int	header	= TRUE;
time_t	tt;
int	yd, md, dd, ht, mt, st;

	if (!rcsopen(name, !silent))
		return;

	while (header && (s = rcsread(s))) {
		s = rcsparse_id(key, s);

		switch (rcskeys(key)) {
		case S_HEAD:
			s = rcsparse_num(tmp, s);
			if (!*opt_rev)
				(void)strcpy(opt_rev, tmp);
			break;
		case S_COMMENT:
			s = rcsparse_str(s);
			break;
		case S_VERS:
			if (dotcmp(key, opt_rev) < 0)
				header = FALSE;
			break;
		case S_DATE:
			s = rcsparse_num(tmp, s);
			if (sscanf(tmp, FMT_DATE,
				&yd, &md, &dd, &ht, &mt, &st) == 6) {
				tt = packdate(1900+yd, md,dd, ht,mt,st);
				ok = tt;	/* patch: cutoff? */
			} else
				header = FALSE;
			break;
		case S_DESC:
			header = FALSE;
		}
		if (!s)	break;
	}
	rcsclose();

	if (ok)
		if (setmtime(name, ok + adj2est(TRUE)) < 0)
			PRINTF("?? touch \"%s\" failed\n", name);
}

/*
 * Check out the file using the RCS 'co' utility.  If 'co' does something, then
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
		return (FALSE);
	}
	return (TRUE);	/* file was not already checked out */
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
				catarg(options, argv[j]);
				if (*s == 'q')
					silent++;
				if (strchr("lqr", *s)) {
					if (*++s)
						(void)strcpy(opt_rev, s);
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
