/*
 * Title:	rcsput.c (rcs put-tree)
 * Author:	T.E.Dickey
 * Created:	19 Oct 1989
 * Modified:
 *		05 Dec 2019, use DYN-argv lists.
 *		13 Jan 2013, pass "-M" option to checkin.
 *		10 May 1997, pass-thru -w option to checkin.
 *		09 May 1994, port to Linux
 *		22 Sep 1993, gcc warnings
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

#define	CUR_PTYPES
#define	STR_PTYPES
#include	<ptypes.h>
#include	<rcsdefs.h>
#include	<sccsdefs.h>
#include	<dyn_str.h>
extern char *tmpnam(char *);

MODULE_ID("$Id: rcsput.c,v 11.14 2025/01/07 00:49:12 tom Exp $")

#define	VERBOSE		if (!quiet) PRINTF

static ARGV *ci_opts;
static ARGV *diff_opts;
static const char *verb = "checkin";
static FILE *log_fp;
static int a_opt;		/* all-directory scan */
static int no_op;		/* no-op mode */
static const char *pager;	/* nonzero if we don't cat diffs */
static int force;
static int quiet;

static void
cat2fp(FILE *fp, char *name)
{
    FILE *ifp;
    char t[BUFSIZ];
    size_t n;

    if ((ifp = fopen(name, "r")) != NULL) {
	while ((n = fread(t, sizeof(char), sizeof(t), ifp)) > 0)
	    if (fwrite(t, sizeof(char), n, fp) != n)
		  break;
	FCLOSE(ifp);
    }
}

static int
different(const char *working)
{
    ARGV *cmds;
    ARGV *opts;
    DYN *flat;
    static const char *prog = "rcsdiff";

    FILE *ifp, *ofp;
    char buffer[BUFSIZ], out_diff[BUFSIZ];
    int changed = FALSE;
    size_t n;

    opts = argv_init();
    argv_merge(&opts, diff_opts);
    argv_append(&opts, working);

    if (!quiet)
	show_argv2(stdout, prog, argv_values(opts));

    /* kludgey, but we don't do many pipes */
    cmds = argv_init1(prog);
    argv_merge(&cmds, opts);

    if (!tmpnam(out_diff))
	failed("tmpnam");
    if (!(ofp = fopen(out_diff, "w")))
	failed("open-tmpnam");

    flat = argv_flatten(cmds);
    if (!(ifp = popen(dyn_string(flat), "r")))
	failed("popen");
    dyn_free(flat);

    /* copy the result to a file so we can send it two places */
    while ((n = fread(buffer, sizeof(char), sizeof(buffer), ifp)) > 0) {
	if (fwrite(buffer, sizeof(char), n, ofp) != n)
	      break;
	changed = TRUE;
    }
    (void) pclose(ifp);
    FCLOSE(ofp);

    if (changed) {
	if (!quiet) {
	    if (pager == NULL)
		cat2fp(stdout, out_diff);
	    else {
		if (execute(pager, out_diff) < 0)
		    failed(pager);
	    }
	}
	if (log_fp != NULL) {
	    PRINTF("appending to logfile");
	    cat2fp(log_fp, out_diff);
	}
    } else if (!quiet) {
	PRINTF("*** no differences found ***\n");
    }

    (void) unlink(out_diff);
    argv_free(&opts);
    argv_free(&cmds);
    return (changed);
}

static void
checkin(const char *path, const char *working, const char *archive)
{
    ARGV *args;
    int first;

    if ((first = (filesize(archive) < 0)) != 0) {
	if (!force && !istextfile(working)) {
	    PRINTF("*** \"%s\" does not seem to be a text file\n",
		   working);
	    return;
	}
	first = TRUE;
    } else {
	time_t date;
	const char *vers, *locker;
	rcslast(path, working, &vers, &date, &locker);
	if (*vers == '?')
	    first = TRUE;	/* no revisions present */
	else {
	    if (!different(working) && !force)
		return;
	    first = FALSE;
	}
    }

    args = argv_init1(verb);
    argv_merge(&args, ci_opts);
    argv_append(&args, working);
    argv_append(&args, archive);

    if (!no_op) {
	PRINTF("*** %s \"%s\"\n",
	       first ? "Initial RCS insertion of"
	       : "Applying RCS delta to",
	       working);
    } else {
	PRINTF("--- %s \"%s\"\n",
	       first ? "This would be initial for"
	       : "Delta would be applied to",
	       working);
    }

    if (!quiet)
	show_argv2(stdout, verb, argv_values(args));
    if (!no_op) {
	if (executev(argv_values(args)) < 0)
	    failed(working);
    }
    argv_free(&args);
}

/*
 * Test for directories that we don't try to scan
 */
static int
ignore_dir(char *path)
{
    if (sameleaf(path, sccs_dir((char *) 0, path))
	|| sameleaf(path, rcs_dir((char *) 0, path))) {
	if (!quiet)
	    PRINTF("...skip %s\n", path);
	return TRUE;
    }
    return FALSE;
}

/*ARGSUSED*/
static int
WALK_FUNC(scan_tree)
{
    char tmp[BUFSIZ], *s = pathcat(tmp, path, name);

    (void) level;

    if (RCS_DEBUG)
	PRINTF("++ scan %s / %s\n", path, name);

    if (sp == NULL || readable < 0) {
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
	checkin(path, name, name2rcs(name, FALSE));
    } else
	readable = -1;

    return (readable);
}

static void
do_arg(const char *name)
{
    (void) walktree((char *) 0, name, scan_tree, "r", 0);
}

static void
usage(int option)
{
    static const char *tbl[] =
    {
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
    unsigned j;
    for (j = 0; j < sizeof(tbl) / sizeof(tbl[0]); j++)
	FPRINTF(stderr, "%s\n", tbl[j]);
    if (option == '?')
	(void) system("checkin -?");
    exit(FAIL);
}

/*ARGSUSED*/
_MAIN
{
    int j;
    const char *s;
    char *cat_input = NULL;
    int m_opt = FALSE;
    char original[MAXPATHLEN];

    if (!isatty(fileno(stdin)) && interactive())
	cat_input = strtrim(file2mem("-"));

    track_wd((char *) 0);
    pager = dftenv("more -l", "PAGER");

    if (!getwd(original))
	failed("getwd");

    ci_opts = argv_init();
    diff_opts = argv_init();

    /* process options */
    for (j = 1; (j < argc) && (*(s = argv[j]) == '-'); j++) {
	if (strchr("BqrfklumMnNstw", s[1]) != NULL) {
	    argv_append(&ci_opts, s);
	    switch (s[1]) {
	    case 'f':
		force = TRUE;
		break;
	    case 'm':
		m_opt = TRUE;
		break;
	    case 'q':
		quiet = TRUE;
		argv_append(&diff_opts, s);
		break;
	    }
	} else {
	    switch (s[1]) {
	    case 'a':
		a_opt = TRUE;
		break;
	    case 'b':
	    case 'h':
		argv_append(&diff_opts, s);
		break;
	    case 'c':
		pager = NULL;
		break;
	    case 'd':
		no_op = TRUE;
		break;
	    case 'L':
		if (s[2] == EOS)
		    s = "logfile";
		if (!(log_fp = fopen(s, "a+")))
		    usage(0);
		break;
	    case 'T':
		verb = s + 2;
		break;
	    default:
		usage(s[1]);
	    }
	}
    }

    if (cat_input && !m_opt)
	argv_append2(&ci_opts, "-m", cat_input);

    /* process list of filenames */
    if (j < argc) {
	while (j < argc) {
	    char working[MAXPATHLEN];
	    char archive[MAXPATHLEN];
	    Stat_t sb;

	    j = rcsargpair(j, argc, argv);
	    if (rcs_working(working, &sb) < 0)
		failed(working);

	    if (isDIR(sb.st_mode)) {
		if (!ignore_dir(working))
		    do_arg(working);
	    } else {
		(void) rcs_archive(archive, (Stat_t *) 0);
		checkin(original, working, archive);
	    }
	}
    } else
	do_arg(".");

    exit(SUCCESS);
    /*NOTREACHED */
}
