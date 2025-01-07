/*
 * Title:	rcsget.c (rcs get-tree)
 * Author:	T.E.Dickey
 * Created:	19 Oct 1989
 * Modified:
 *		05 Dec 2019, use DYN-argv lists.
 *		04 Sep 2012, pass-thru -c option for 'checkout'.
 *		12 Nov 1994, pass-thru -f, -k options for 'co'.
 *		22 Sep 1993, gcc warnings
 *		06 Feb 1992, revised filename-parsing with 'rcsargpair()',
 *			     obsoleted "-x" option.
 *		11 Oct 1991, converted to ANSI
 *		25 Sep 1991, added options R and L. Ensure that RCS-directory
 *			     exists before trying to extract the file.
 *			     Make this show normal-trace when "-n -q" are set.
 *		20 Jun 1991, use 'shoarg()'
 *		06 Jun 1991, use "-x" option in local name-checking
 *		03 Jun 1991, pass-thru "-x" to 'checkout'
 *		03 Jun 1991, mods to compile on apollo sr10.3
 *		19 Apr 1990, added "-T" option so that 'checkout' isn't
 *			     hard-coded.
 *		18 Apr 1990, changed call on rcs2name/name2rcs to support "-x"
 *			     option in checkin/checkout
 *		16 Apr 1990, interpret "-q" (quiet) option in this program
 *		03 Nov 1989, additional correction: if file does not exist, it
 *			     is ok to ask 'checkout' to get it!  Added a hack
 *			     ("-?" option) to get checkout to show its options
 *			     in this usage-message.
 *		01 Nov 1989, walktree passes null pointer to stat-block if
 *			     no-access.
 *
 * Function:	Use 'checkout' to checkout one or more files from the
 *		RCS-directory which is located in the current working
 *		directory, and then, to set the modification date of the
 *		checked-out files according to the last delta date (rather
 *		than the current date, as RCS assumes).
 *
 * Options:	(see 'usage()')
 */

#define	STR_PTYPES
#include	<ptypes.h>
#include	<dyn_str.h>
#include	<rcsdefs.h>
#include	<sccsdefs.h>
#include	<errno.h>

MODULE_ID("$Id: rcsget.c,v 11.11 2025/01/07 00:52:02 tom Exp $")

#define	VERBOSE	if (!quiet) PRINTF

static char user_wd[BUFSIZ];	/* working-directory for scan_archive */
static const char *verb = "checkout";
static int a_opt;		/* all-directory scan */
static int R_opt;		/* recur/directory-mode */
static int L_opt;		/* follow links */
static int n_opt;		/* no-op mode */
static int quiet;		/* "-q" option */

static void
set_wd(const char *path)
{
    if (!n_opt)
	if (chdir(path) < 0)
	    failed(path);
}

static ARGV *co_dyns;

static void
Checkout(char *working, char *archive)
{
    ARGV *list;

    list = argv_init();
    argv_append(&list, verb);
    argv_merge(&list, co_dyns);
    argv_append(&list, working);
    argv_append(&list, archive);

    if (!quiet || n_opt) {
	show_argv(stdout, argv_values(list));
    }
    if (!n_opt) {
	if (executev(argv_values(list)) < 0)
	    failed(working);
    }
    argv_free(&list);
}

static int
an_archive(const char *name)
{
    size_t len_name = strlen(name);
    size_t len_type = strlen(RCS_SUFFIX);
    return (len_name > len_type
	    && !strcmp(name + len_name - len_type, RCS_SUFFIX));
}

/*
 * Test for directories that we don't try to scan
 */
static int
ignore_dir(char *path)
{
    if (!a_opt && *pathleaf(path) == '.'
	&& sameleaf(path, sccs_dir((char *) 0, path))) {
	if (!quiet)
	    PRINTF("...skip %s\n", path);
	return TRUE;
    }
    return FALSE;
}

static void
Ignore(const char *name, const char *why)
{
    VERBOSE("?? ignored: %s%s\n", name, why);
}

/*ARGSUSED*/
static int
WALK_FUNC(scan_archive)
{
    char tmp[BUFSIZ];

    (void) level;

    if (!strcmp(user_wd, path))	/* account for initial argument */
	return (readable);
    if (!isFILE(sp->st_mode)
	|| !an_archive(name)) {
	Ignore(name, " (not an archive)");
	return (-1);
    }
    if (!strcmp(vcs_file((char *) 0, strcpy(tmp, name), FALSE), name))
	return (readable);

    set_wd(user_wd);
    Checkout(rcs2name(name, FALSE), pathcat(tmp, rcs_dir(NULL, NULL), name));
    set_wd(path);
    return (readable);
}

static int
WALK_FUNC(scan_tree)
{
    char tmp[BUFSIZ];
    char *s = pathcat(tmp, path, name);
    Stat_t sb;

    if (RCS_DEBUG)
	PRINTF("++ %s%sscan (%s, %s, %s%d)\n",
	       R_opt ? "R " : "",
	       L_opt ? "L " : "",
	       path, name, (sp == NULL) ? "no-stat, " : "", level);

    if (!quiet || n_opt)
	track_wd(path);

    if (sp == NULL) {
	if (R_opt && (level > 0)) {
	    Ignore(name, " (no such file)");
	}
    } else if (isDIR(sp->st_mode)) {
	abspath(s);		/* get rid of "." and ".." names */
	if (ignore_dir(s))
	    readable = -1;
	else if (sameleaf(s, rcs_dir(NULL, NULL))) {
	    if (R_opt) {
		(void) walktree(strcpy(user_wd, path),
				name, scan_archive, "r", level);
	    }
	    readable = -1;
	} else {
#ifdef	S_IFLNK
	    if (!L_opt
		&& (lstat(s, &sb) < 0 || isLINK(sb.st_mode))) {
		Ignore(name, " (is a link)");
		readable = -1;
	    }
#endif
	}
    } else if (!isFILE(sp->st_mode)) {
	Ignore(name, RCS_DEBUG ? " (not a file)" : "");
	readable = -1;
    }
    return (readable);
}

static void
do_arg(const char *name)
{
#ifdef	S_IFLNK
    if (!L_opt) {
	Stat_t sb;
	if (lstat(name, &sb) >= 0 && isLINK(sb.st_mode)) {
	    Ignore(name, " (is a link)");
	    return;
	}
    }
#endif
    (void) walktree((char *) 0, name, scan_tree, "r", 0);
}

static void
usage(int option)
{
    static const char *tbl[] =
    {
	"usage: rcsget [options] files"
	,""
	,"Options include all CHECKOUT options, plus:"
	,"  -a       process all directories, including those beginning with \".\""
	,"  -d       directory-mode (scan based on archives, rather than working files"
	,"  -L       follow symbolic-links to subdirectories when -R is set"
	,"  -n       no-op (show what would be checked-out, but don't do it"
	,"  -q       quiet (also passed to \"checkout\")"
	,"  -R       recur (same as -d)"
	,"  -T TOOL  specify alternate tool to \"checkout\" to invoke per-file"
    };
    unsigned j;
    for (j = 0; j < sizeof(tbl) / sizeof(tbl[0]); j++)
	FPRINTF(stderr, "%s\n", tbl[j]);
    if (option == '?')
	(void) execute(verb, "-?");
    exit(FAIL);
}

/*ARGSUSED*/
_MAIN
{
    int j;
    char *s, *t;

    track_wd((char *) 0);

    /* process options */
    for (j = 1; (j < argc) && (*(s = argv[j]) == '-'); j++) {
	t = s + strlen(s);
	if (strchr("cfklpqrcswj", s[1]) != NULL) {
	    argv_append(&co_dyns, s);
	    if (s[1] == 'q')
		quiet = TRUE;
	} else
	    while (s[1]) {
		switch (s[1]) {
		case 'a':
		    a_opt = TRUE;
		    break;
		case 'R':
		case 'd':
		    R_opt = TRUE;
		    break;
		case 'L':
		    L_opt = TRUE;
		    break;
		case 'n':
		    n_opt = TRUE;
		    break;
		case 'T':
		    verb = s + 2;
		    s = t;
		    break;
		default:
		    usage(s[1]);
		}
		s++;
	    }
    }

    /* process filenames */
    if (j < argc) {
	while (j < argc) {
	    char working[MAXPATHLEN];
	    char archive[MAXPATHLEN];
	    Stat_t sb;

	    j = rcsargpair(j, argc, argv);
	    if (rcs_working(working, &sb) < 0 && errno != EISDIR)
		failed(working);

	    if (isDIR(sb.st_mode)) {
		if (!ignore_dir(working))
		    do_arg(working);
	    } else {
		(void) rcs_archive(archive, (Stat_t *) 0);
		Checkout(working, archive);
	    }
	}
    } else
	do_arg(".");

    exit(SUCCESS);
    /*NOTREACHED */
}
