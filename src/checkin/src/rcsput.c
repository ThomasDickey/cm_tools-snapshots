#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/cm_tools.vcs/src/checkin/src/RCS/rcsput.c,v 11.0 1992/03/04 10:07:40 ste_cm Rel $";
#endif

/*
 * Title:	rcsput.c (rcs put-tree)
 * Author:	T.E.Dickey
 * Created:	19 Oct 1989
 * Modified:
 *		04 Mar 1992, test "-f" after 'different()'
 *		05 Feb 1992, revised filename-parsing with 'rcsargpair()',
 *			     obsoleted "-x".
 *		04 Feb 1992, pass piped-in text via "-m" option.
 *		11 Oct 1991, converted to ANSI
 *		01 Oct 1991, added "-B" option for 'checkin'
 *		13 Sep 1991, moved 'filesize()' to common-lib
 *		20 Jun 1991, use 'shoarg()'
 *		06 Jun 1991, use "-x" option in local name-checking
 *		03 Jun 1991, pass-thru "-x" to 'checkin'
 *		20 May 1991, mods to compile on apollo sr10.3
 *		19 Apr 1990, added "-T" option (to permit non-checkin tool use)
 *		18 Apr 1990, modified call on rcs2name/name2rcs to support "-x"
 *			     option in checkin/checkout
 *		06 Dec 1989, added interpretation of "-f" (force) option to
 *			     override test-for-diffs, test-for-textfile, and
 *			     test-for-existence of arguments.  Also, interpret
 *			     "-?" argument to show not only usage message from
 *			     this utility, but from checkin too.
 *		01 Nov 1989, walktree passes null pointer to stat-block if
 *			     no-access.
 *
 * Function:	Use 'checkin' to checkin one or more files from the
 *		RCS-directory which is located in the current working
 *		directory, and then, to set the delta date of the  checked-in
 *		files according to the last modification date (rather than the
 *		current date, as RCS assumes).
 *
 * Options:	see 'usage()'
 */

#define	STR_PTYPES
#include	<ptypes.h>
#include	<rcsdefs.h>
#include	<sccsdefs.h>
#include	<dyn_string.h>
extern	FILE	*popen(_arx(char *,cmd) _ar1(char *,mode));
extern	char	*tmpnam(_ar1(char *,name));

#define	VERBOSE		if (!quiet) PRINTF

static	DYN *	ci_opts;
static	DYN *	diff_opts;
static	char	*verb = "checkin";
static	FILE	*log_fp;
static	int	a_opt;		/* all-directory scan */
static	int	no_op;		/* no-op mode */
static	char	*pager;		/* nonzero if we don't cat diffs */
static	int	force;
static	int	quiet;

static
cat2fp(
_ARX(FILE *,	fp)
_AR1(char *,	name)
	)
_DCL(FILE *,	fp)
_DCL(char *,	name)
{
	auto	FILE	*ifp;
	auto	char	t[BUFSIZ];
	auto	size_t	n;

	if (ifp = fopen(name, "r")) {
		while ((n = fread(t, sizeof(char), sizeof(t), ifp)) > 0)
			if (fwrite(t, sizeof(char), n, fp) != n)
				break;
		FCLOSE(ifp);
	}
}

static
different(
_AR1(char *,	working))
_DCL(char *,	working)
{
	static	DYN	*cmds, *opts;
	static	char	*prog = "rcsdiff";

	auto	FILE	*ifp, *ofp;
	auto	char	buffer[BUFSIZ],
			out_diff[BUFSIZ];
	auto	int	changed	= FALSE;
	auto	size_t	n;

	dyn_init(&opts, BUFSIZ);
	APPEND(opts, dyn_string(diff_opts));
	CATARG(opts, working);

	if (!quiet) shoarg(stdout, prog, dyn_string(opts));

	/* kludgey, but we don't do many pipes */
	dyn_init(&cmds, dyn_length(opts) + strlen(prog) + 2);
	APPEND(cmds, prog);
	APPEND(cmds, " ");
	(void) bldcmd(dyn_string(cmds) + dyn_length(cmds),
		      dyn_string(opts),  dyn_length(opts));

	if (!tmpnam(out_diff) || !(ofp = fopen(out_diff,"w")))
		failed("tmpnam");

	if (!(ifp = popen(dyn_string(cmds), "r")))
		failed("popen");

	/* copy the result to a file so we can send it two places */
	while ((n = fread(buffer, sizeof(char), sizeof(buffer), ifp)) > 0) {
		if (fwrite(buffer, sizeof(char), n, ofp) != n)
			break;
		changed = TRUE;
	}
	(void)pclose(ifp);
	FCLOSE(ofp);

	if (changed) {
		if (!quiet) {
			if (pager == 0)
				cat2fp(stdout, out_diff);
			else {
				if (execute(pager, out_diff) < 0)
					failed(pager);
			}
		}
		if (log_fp != 0) {
			PRINTF("appending to logfile");
			cat2fp(log_fp, out_diff);
		}
	} else if (!quiet) {
		PRINTF("*** no differences found ***\n");
	}

	(void)unlink(out_diff);
	return (changed);
}

static
checkin(
_ARX(char *,	path)
_ARX(char *,	working)
_AR1(char *,	archive)
	)
_DCL(char *,	path)
_DCL(char *,	working)
_DCL(char *,	archive)
{
	static	DYN	*args;
	auto	int	first;

	if (first = (filesize(archive) < 0)) {
		if (!force && !istextfile(working)) {
			PRINTF("*** \"%s\" does not seem to be a text file\n",
				working);
			return;
		}
		first = TRUE;
	} else {
		auto	time_t	date;
		auto	char	*vers,
				*locker;
		rcslast(path, working, &vers, &date, &locker);
		if (*vers == '?')
			first = TRUE;	/* no revisions present */
		else {
			if (!different(working) && !force)
				return;
			first = FALSE;
		}
	}

	dyn_init(&args, BUFSIZ);
	(void) dyn_append(args, dyn_string(ci_opts));
	CATARG(args, working);
	CATARG(args, archive);

	if (!no_op) {
		PRINTF("*** %s \"%s\"\n",
			first	? "Initial RCS insertion of"
				: "Applying RCS delta to",
			working);
	} else {
		PRINTF("--- %s \"%s\"\n",
			first	? "This would be initial for"
				: "Delta would be applied to",
			working);
	}

	if (!quiet) shoarg(stdout, verb, dyn_string(args));
	if (!no_op) {
		if (execute(verb, dyn_string(args)) < 0)
			failed(working);
	}
}

/*
 * Test for directories that we don't try to scan
 */
static
int
ignore_dir(
_AR1(char *,	path))
_DCL(char *,	path)
{
	if (sameleaf(path, sccs_dir())
	||  sameleaf(path, rcs_dir())) {
		if (!quiet) PRINTF("...skip %s\n", path);
		return TRUE;
	}
	return FALSE;
}

/*ARGSUSED*/
static
WALK_FUNC(scan_tree)
{
	auto	char	tmp[BUFSIZ],
			*s = pathcat(tmp, path, name);

	if (RCS_DEBUG)
		PRINTF("++ scan %s / %s\n", path, name);

	if (sp == 0 || readable < 0) {
		readable = -1;
		if (!ignore_dir(s)) {	/* could be RCS-dir we cannot scan */
			perror(name);
			if (!force)
				exit(FAIL);
		}
	} else if (isDIR(sp->st_mode)) {
		abspath(s);		/* get rid of "." and ".." names */
		if (!a_opt && *pathleaf(s) == '.')
			readable = -1;
		else if (ignore_dir(s))
			readable = -1;
		else
			track_wd(path);
	} else if (isFILE(sp->st_mode)) {
		track_wd(path);
		checkin(path, name, name2rcs(name,FALSE));
	} else
		readable = -1;

	return(readable);
}

static
do_arg(
_AR1(char *,	name))
_DCL(char *,	name)
{
	(void)walktree((char *)0, name, scan_tree, "r", 0);
}

usage(
_AR1(int,	option))
_DCL(int,	option)
{
	static	char	*tbl[] = {
 "Usage: rcsput [options] files_or_directories"
,""
,"Options include all CHECKIN-options, plus:"
,"  -a       process all directories, including those beginning with \".\""
,"  -b       (passed to rcsdiff)"
,"  -h       (passed to rcsdiff)"
,"  -c       send differences to terminal without $PAGER filtering"
,"  -d       compute differences only, don't try to CHECKIN"
,"  -L file  write all differences to logfile"
,"  -T TOOL  specify alternate tool to \"checkin\" to invoke per-file"
,""
	};
	register int	j;
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	if (option == '?')
		(void)system("checkin -?");
	exit(FAIL);
}

/*ARGSUSED*/
_MAIN
{
	register int	j;
	register char	*s;
	auto	 char	*cat_input = 0;
	auto	 int	m_opt	 = FALSE;
	auto	 char	original[MAXPATHLEN];

	if (!isatty(fileno(stdin)) && interactive())
		cat_input = strtrim(file2mem("-"));

	track_wd((char *)0);
	pager = dftenv("more -l", "PAGER");

	if (!getwd(original))
		failed("getwd");

	dyn_init(&ci_opts, BUFSIZ);
	dyn_init(&diff_opts, BUFSIZ);

	/* process options */
	for (j = 1; (j < argc) && (*(s = argv[j]) == '-'); j++) {
		if (strchr("BqrfklumnNst", s[1]) != 0) {
			CATARG(ci_opts, s);
			switch (s[1]) {
			case 'f':	force = TRUE;		break;
			case 'm':	m_opt = TRUE;		break;
			case 'q':	quiet = TRUE;
					CATARG(diff_opts, s);	break;
			}
		} else {
			switch (s[1]) {
			case 'a':	a_opt = TRUE;		break;
			case 'b':
			case 'h':	CATARG(diff_opts, s);	break;
			case 'c':	pager = 0;		break;
			case 'd':	no_op = TRUE;		break;
			case 'L':	if (s[2] == EOS)
						s = "logfile";
					if (!(log_fp = fopen(s, "a+")))
						usage(0);
					break;
			case 'T':	verb = s+2;		break;
			default:	usage(s[1]);
			}
		}
	}

	if (cat_input && !m_opt)
		CATARG2(ci_opts, "-m", cat_input);

	/* process list of filenames */
	if (j < argc) {
		while (j < argc) {
			char	working[MAXPATHLEN];
			char	archive[MAXPATHLEN];
			STAT	sb;

			j = rcsargpair(j, argc, argv);
			if (rcs_working(working, &sb) < 0)
				failed(working);

			if (isDIR(sb.st_mode)) {
				if (!ignore_dir(working))
					do_arg(working);
			} else {
				(void)rcs_archive(archive, (STAT *)0);
				checkin(original, working, archive);
			}
		}
	} else
		do_arg(".");

	exit(SUCCESS);
	/*NOTREACHED*/
}
