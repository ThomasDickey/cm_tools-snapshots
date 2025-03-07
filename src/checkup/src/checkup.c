/*
 * Title:	checkup.c (link/directory tree)
 * Author:	T.E.Dickey
 * Created:	31 Aug 1988
 * Modified:
 *		10 May 2002, use the workaround more generally (a 1-second
 *			     difference still shows up on mounted VFAT volumes.
 *		01 May 2002, workaround for cygwin timestamps...
 *		21 Jul 2000, support checkup -o with $SCCS_VAULT
 *		24 Jan 2000, revised directory macros.
 *		05 Jul 1995, show names of non-text files for -a option
 *		08 Nov 1994, refined interaction between stdout/stderr.
 *		03 Aug 1994, use 'lastrev()'; related interface changes.
 *		22 Sep 1992, gcc warnings
 *		30 Oct 1992, added checks for RCS version 5 (GMT dates).
 *		01 May 1992, added "-L" option.
 *		11 Oct 1991, converted to ANSI
 *		15 Jul 1991, distinguish between non-archive/obsolete files.
 *			     Corrected the code which prints the names of these
 *			     files (had wrong leafname).
 *		20 May 1991, mods to compile on apollo sr10.3
 *		30 Mar 1990, corrected code which checks for suppressed
 *			     extensions (if a wildcard is given, we don't really
 *			     know the exact length that would be matched).
 *		28 Mar 1990, Added "-i" option.  Made both "-i" and "-x" options
 *			     permit wildcards.  Enhanced usage-message.  Added
 *			     "-c" (configuration-list) option so user can obtain
 *			     a list of the working files in a tree together with
 *			     their revision codes.  Display the version-number
 *			     in "-d" listing where I was only showing "ok".
 *		07 Feb 1990, Added "-l" option to reopen stderr so we can easily
 *			     pipe the report to a file.  Added "-p" option to
 *			     generate relative pathnames
 *		26 Oct 1989, use new procedure 'istextfile()'
 *		17 Aug 1989, modified "-r" option to support reverse comparison
 *			     (if user suffixes a "+" to option-value).  Expanded
 *			     usage-message to multi-line display.  corrected a
 *			     place where "?" constant was overwritten.  added
 *			     "-d" and "-o" options.  If "-r" and "-o" are both
 *			     set, use "-r" to limit the set of files shown as
 *			     obsolete, rather than to select working files
 *			     for date-comparison.
 *		26 Sep 1988, corrected use of 'sameleaf()' -- had wrong args!
 *		19 Sep 1988, added '-a' option.
 *		02 Sep 1988, use 'rcs_dir()', 'sccs_dir()'
 *		01 Sep 1988, added '-r', '-q, -s' options.
 *
 * Function:	Scan the specified directories, looking for files which may
 *		or may not have been checked into either RCS or SCCS.
 *
 *		The check-in date is compared with the file modification
 *		times, for compatibility with 'checkin' and 'checkout'.
 *
 *		The directory-display is written to the standard-error.
 *		If standard output is piped (e.g., to a file), the list of
 *		offending filenames will be written there as well.
 *
 * Options:	(see 'usage()')
 */

#define	DIR_PTYPES
#define	STR_PTYPES
#include	<ptypes.h>
#include	<cmv_defs.h>	/* for 'lastrev()' */
#include	<rcsdefs.h>
#include	<sccsdefs.h>
#include	<ctype.h>

MODULE_ID("$Id: checkup.c,v 11.21 2025/01/07 00:53:07 tom Exp $")

/************************************************************************
 *	local definitions						*
 ************************************************************************/

typedef struct _exts {
    struct _exts *link;
    char *if_ext;		/* conditional extension */
    char *no_ext;		/* extension to suppress */
} EXTS;

static EXTS *exts;		/* "-x" and "-t" extension list */
static int allnames;		/* "-a" option */
static int config_rev;		/* "-c" option */
static int debug;		/* "-d" option */
static int obsolete;		/* "-o" option */
static int verbose;		/* "-v" option */
#ifdef	S_IFLNK
static int show_links;		/* "-L" option */
#endif
static int lines;		/* line-number, for report */
static const char *revision = "0";	/* required revision level */
static int reverse;		/* true if we reverse revision-test */

static int redir_out;		/* true if stdout isn't tty */
static int redir_err;		/* true if stderr isn't tty */

static char *original;		/* original directory, for "-p" */

/*
 * Define a new leaf-name to ignore.  Piggyback on the "-x" data structure by
 * defining the ".no_ext" member to null.
 */
static void
ignore(char *string)
{
    EXTS *savep = exts;
    exts = ALLOC(EXTS, 1);
    exts->link = savep;
    exts->no_ext = NULL;
    exts->if_ext = txtalloc(string);
    if (debug) {
	if (verbose >= 0)
	    FPRINTF(stderr, "ignore leaf \"%s\"\n", string);
    }
}

/*
 * Define a new extension-to-ignore
 */
static void
extension(const char *string)
{
    EXTS *savep = exts;
    char *s = strrchr(string, *string);
    char bfr[BUFSIZ];
    int show = (debug || verbose > 0);

    exts = ALLOC(EXTS, 1);
    exts->link = savep;
    exts->no_ext = txtalloc(s);
    exts->if_ext = NULL;
    if (s != string) {
	strcpy(bfr, string)[s - string] = EOS;
	exts->if_ext = txtalloc(bfr);
	if (show) {
	    if (verbose >= 0)
		FPRINTF(stderr, "if(%s) ", exts->if_ext);
	}
    }
    if (show) {
	if (verbose >= 0)
	    FPRINTF(stderr, "extension(%s)\n", exts->no_ext);
    }
}

/*
 * Test the given filename to see if it has any of the suffixes which the user
 * wishes to ignore.  If so, return TRUE.
 */
static int
suppress(const char *name)
{
    EXTS *p;
    int len;
    int off = (int) strlen(name);
    char bfr[BUFSIZ];
    struct stat sb;

    for (p = exts; p; p = p->link) {
	if (p->no_ext == NULL) {	/* "-i" test? */
	    if (!strwcmp(p->if_ext, name))
		return (TRUE);
	} else {
	    for (len = 0; len < off; len++) {
		if (!strwcmp(p->no_ext, name + len)) {
		    if (p->if_ext) {	/* look for file */
			(void) strcpy(strcpy(bfr, name) + len,
				      p->if_ext);
			if (stat(bfr, &sb) >= 0
			    && isFILE(sb.st_mode))
			    return (TRUE);
		    } else {	/* unconditional */
			return (TRUE);
		    }
		}
	    }
	}
    }
    return (FALSE);
}

/*
 * Indent the report for the given number of directory-levels.
 */
static void
indent(int level)
{
    ++lines;
    if (verbose >= 0) {
	FPRINTF(stderr, "%4d:\t", lines);
	while (level-- > 0)
	    FPRINTF(stderr, "|--%c", (level > 0) ? '-' : ' ');
    }
}

/*
 * If we're logging the tree-display, or if we're redirecting standard output,
 * send to the standard output a list of pathnames (with version numbers
 * prepended if the "-c" option is specified).
 */
static void
pipes(const char *path,
      const char *name,
      const char *vers)
{
    char tmp[BUFSIZ];
    char tmp2[BUFSIZ];
    int fake_tee = (redir_out && (redir_out != redir_err));

    if ((verbose < 0) || fake_tee) {
	if (!fake_tee)
	    fflush(stderr);
	if (config_rev) {
	    if (*vers != '?')
		PRINTF("%s ", vers);
	    else
		return;
	}
	path = pathcat(tmp, path, name);
	if (original != NULL)
	    path = relpath(tmp2, original, path);
	PRINTF("%s\n", path);
	if (!fake_tee)
	    fflush(stdout);
    }
}

static char *
compared(const char *what, const char *rev)
{
    static char buffer[80];
    FORMAT(buffer, "%s than %s", what, rev);
    return (buffer);
}

/*
 * Scan a directory looking for obsolete archives.  This requires special
 * handling, since the directory-name may be a symbolic link; thus we have to
 * be careful where we look for the working file!
 */
static void
do_obs(const char *working,	/* current working directory, from 'do_stat()' */
       const char *archive,	/* name of directory (may be symbolic link) */
       int level)
{
    DIR *dp;
    DirentT *de;
    char tpath[BUFSIZ], tname[BUFSIZ];
    Stat_t sb;
    const char *tag = "?", *vers, *owner;
    time_t cdate;

    if (!(dp = opendir(pathcat(tpath, working, archive)))) {
	perror(tpath);
	return;
    }

    while ((de = readdir(dp)) != NULL) {
	if (dotname(de->d_name))
	    continue;
	if (stat(pathcat(tname, archive, de->d_name), &sb) >= 0) {
	    int show = FALSE;

	    lastrev(working, tname, &vers, &cdate, &owner);

	    /*
	     * If 'cdate' is zero, then we could not (for whatever
	     * reason) find a working file.  If 'vers' is "?", then
	     * the file was not an archive, so we report this
	     * always.  Filter the remaining files according to
	     * revision codes.
	     */
	    if (cdate == 0) {
		show = TRUE;
		tag = "obsolete";
		if (*vers == '?') {
		    tag = "non-archive";
		}
	    } else {
		if (reverse) {
		    if (dotcmp(vers, revision) > 0)
			show = TRUE;
		} else {
		    if (dotcmp(vers, revision) < 0)
			show = TRUE;
		}
	    }

	    if (show) {
		indent(level);
		if (verbose >= 0)
		    FPRINTF(stderr, "%s (%s:%s)\n", tname, tag, vers);
		pipes(working, tname, vers);
	    } else if (debug) {
		indent(level);
		if (verbose >= 0)
		    FPRINTF(stderr, "%s (%s)\n", tname, vers);
	    }
	}
    }
    (void) closedir(dp);
}

static int
cmp_date(time_t a, time_t b)
{
    int result = 0;
#define FIX_DATE(n) if ((n) & 1) ++n
    FIX_DATE(a);
    FIX_DATE(b);
    if (a > b)
	result = 1;
    else if (a < b)
	result = -1;
    return result;
}

/*
 * This procedure is invoked from 'walktree()' for each file/directory which
 * is found in the specified tree.  Analyze the files to see if anything should
 * be reported.  Report all directory names, so we can see the context of each
 * filename.
 */
static int
WALK_FUNC(do_stat)
{
    const char *change = NULL;
    const char *vers;
    const char *owner;
    const char *locked_by = NULL;
    time_t cdate;
    mode_t mode = (sp != NULL) ? sp->st_mode : 0;
    int ok_text = TRUE;

    if (isDIR(mode)) {
	char tmp[BUFSIZ], *s = pathcat(tmp, path, name), *t;
	abspath(s);		/* get rid of "." and ".." names */
	t = pathleaf(s);	/* obtain leaf-name for "-a" option */
	if (*t == '.' && !allnames)
	    return (-1);
	if (sameleaf(s, sccs_dir(path, name))
	    || sameleaf(s, rcs_dir(path, name))) {
	    if (obsolete)
		do_obs(path, name, level);
	    return (-1);
	} else if (strncmp(s, sccs_dir(path, name), strlen(s))
		   && obsolete)
	    do_obs(s, sccs_dir(path, name), level);
    }

    if (readable < 0 || sp == NULL) {
	if (verbose > 0)
	    FPRINTF(stderr, "?? %s/%s\n", path, name);
    } else if (isDIR(mode)) {
	change = (name[strlen(name) - 1] == '/') ? "" : "/";
#ifdef	S_IFLNK
	if ((lstat(name, sp) >= 0)
	    && isLINK(sp->st_mode)) {
	    change = " (link)";
	    readable = -1;
	}
#endif
	indent(level);
	if (verbose >= 0)
	    FPRINTF(stderr, "%s%s\n", name, change);
    } else if (isFILE(mode) && !suppress(name)) {
#ifdef	S_IFLNK
	if (!show_links
	    && (lstat(name, sp) >= 0)
	    && isLINK(sp->st_mode)) {
	    change = NULL;
	    vers = "link";
	    readable = -1;
	} else {
#endif
	    lastrev(path, name, &vers, &cdate, &owner);
	    if (*owner != EOS && *owner != '?')
		locked_by = owner;
	    else
		owner = "";

	    if (cdate != 0) {
		int cmp = cmp_date(cdate, sp->st_mtime);
		if (cmp == 0) {
		    if (obsolete) ;	/* interpret as obsolete-rev */
		    else if (reverse) {
			if (dotcmp(vers, revision) > 0)
			    change = vers;
		    } else {
			if (dotcmp(vers, revision) < 0)
			    change = vers;
		    }
		} else if (cmp > 0) {
		    change = compared("older", vers);
		} else if (cmp < 0) {
		    if (cmp_date(sp->st_mtime, cdate + gmt_offset(cdate)) != 0)
			change = compared("newer", vers);
		}
	    } else if ((ok_text = istextfile(name)) != 0) {
		change = "not archived";
	    } else if (allnames) {
		change = "not a text-file";
	    }
#ifdef	S_IFLNK
	}
#endif
	if ((change != NULL) || locked_by != NULL) {
	    if (change == NULL)
		change = "no change";
	    indent(level);
	    if (verbose >= 0)
		FPRINTF(stderr, "%s (%s%s%s)\n",
			name,
			change,
			(locked_by != NULL) ? ", locked by " : "",
			owner);
	    pipes(path, name, vers);
	} else if (debug) {
	    indent(level);
	    if (verbose >= 0)
		FPRINTF(stderr, "%s (%s)\n", name, ok_text ? vers : "binary");
	    if (ok_text && config_rev && isdigit(UCH(*vers)))
		pipes(path, name, vers);
	}
    }
    return (readable);
}

/*
 * Process a single argument: a directory name.
 */
static void
do_arg(const char *name)
{
    FPRINTF(stderr, "** path = %s\n", name);
    lines = 0;
    (void) walktree((char *) 0, name, do_stat, "r", 0);
}

static void
usage(void)
{
    static const char *msg[] =
    {
	"Usage: checkup [options] [directory [...]]",
	"",
	"A report is written to standard-error showing the directory trees which are",
	"scanned, and the check-in status of each source-file.  If standard-output is",
	"not a terminal (i.e., redirected to a file) it receives a list of the file",
	"names reported.",
	"",
	"Options:",
	"  -a      report files and directories beginning with \".\"",
	"  -c      display configuration-revisions of each file.  This causes the list",
	"          of files written to standard-output to include only checked-in",
	"          names, with their revision codes.",
	"  -d      debug (show all names)",
	"  -i TEXT suppress leaves matching \"TEXT\"",
	"  -l FILE write logfile of tree-of-files",
#ifdef	S_IFLNK
	"  -L      process symbolic-link targets",
#endif
	"  -o      reports obsolete files (no working file exists)",
	"  -p      generate pathnames relative to current directory",
	"  -q      quiet: suppresses directory-tree report",
	"  -r REV  specifies revision-level to check against, reporting all working",
	"          files whose highest checked-in version is below REV.",
	"          Use REV+ to reverse, showing files above REV.",
	"  -s      (same as -q)",
	"  -t      suppresses standard extensions: .bak .i .log .out .tmp",
	"  -v      verbose: prints names (to stderr) which cannot be processed (e.g.,",
	"          sockets and special devices).",
	"  -x TEXT specifies an extension to be ignored.  The first character in the",
	"          option-value is the delimiter.  If repeated, it marks off a second",
	"          extension which must correspond to an existing file before the",
	"          matching file is ignored. The cases are:",
	"          .EXT to suppress unconditionally,",
	"          .IF.THEN conditionally",
	"",
	"The -i \"TEXT\" and -x \".EXT\" and \".THEN\" strings permit the use of the",
	"wildcards \"*\" and \"?\".  The -i and -x options are processed in LIFO order."
    };
    unsigned j;
    for (j = 0; j < sizeof(msg) / sizeof(msg[0]); j++)
	FPRINTF(stderr, "%s\n", msg[j]);
    (void) exit(FAIL);
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/

/*ARGSUSED*/
_MAIN
{
    char tmp[BUFSIZ];
    int j;

    /* make stderr line-buffered, since we send our report that way */
#if defined(_IOLBF) && defined(IOLBF) && HAVE_SETVBUF
    char bfr[BUFSIZ];
    if (!(stderr->_flag & _IOLBF))
	(void) setvbuf(stderr, bfr, _IOLBF, sizeof(bfr));
#else
#  if defined(HAVE_SETLINEBUF)
    (void) setlinebuf(stderr);
#  endif
#endif

    redir_out = !isatty(fileno(stdout));
    redir_err = !isatty(fileno(stderr));

    while ((j = getopt(argc, argv, "acdi:l:Lopqr:stvx:")) != EOF)
	switch (j) {
	case 'a':
	    allnames++;
	    break;
	case 'c':
	    config_rev++;
	    break;
	case 'd':
	    debug++;
	    break;
	case 'i':
	    ignore(optarg);
	    break;
	case 'l':
	    if (fopen(optarg, "a+") == NULL
		|| freopen(optarg, "a+", stderr) == NULL)
		failed(optarg);
	    redir_err = -TRUE;
	    break;
#ifdef	S_IFLNK
	case 'L':
	    show_links++;
	    break;
#endif
	case 'o':
	    obsolete++;
	    break;
	case 'p':
	    original = txtalloc(getwd(tmp));
	    break;
	case 'r':
	    revision = optarg;
	    break;
	case 'q':
	case 's':
	    verbose--;
	    break;
	case 'v':
	    verbose++;
	    break;
	case 't':
	    extension(".bak");
	    extension(".i");
	    extension(".log");
	    extension(".out");
	    extension(".tmp");
	    break;
	case 'x':
	    extension(optarg);
	    break;
	default:
	    usage();
	    /*NOTREACHED */
	}

    /*
     * If we are asked to do a reverse-comparison, we must juggle the
     * revision-level so that 'dotcmp()' can give us a useful return value.
     */
    if (revision[j = (int) strlen(revision) - 1] == '+') {
	char *s;

	strcpy(tmp, revision);
	tmp[j] = EOS;
	if ((s = strrchr(tmp, '.')) != NULL) {
	    if (s[1] == EOS)
		(void) strcat(tmp, "0");
	} else
	    (void) strcat(tmp, ".0");
	revision = txtalloc(tmp);
	reverse = TRUE;
    }

    /*
     * Process the list of file-specifications:
     */
    if (optind < argc) {
	for (j = optind; j < argc; j++)
	    do_arg(argv[j]);
    } else
	do_arg(".");

    (void) exit(SUCCESS);
    /*NOTREACHED */
}
