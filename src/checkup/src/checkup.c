#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/cm_tools.vcs/src/checkup/src/RCS/checkup.c,v 9.1 1991/07/15 12:59:07 dickey Exp $";
#endif

/*
 * Title:	checkup.c (link/directory tree)
 * Author:	T.E.Dickey
 * Created:	31 Aug 1988
 * $Log: checkup.c,v $
 * Revision 9.1  1991/07/15 12:59:07  dickey
 * distinguish between non-archive/obsolete files.  Corrected
 * the code which prints the names of these files (had wrong
 * leafname).
 *
 *		Revision 9.0  91/05/20  12:40:56  ste_cm
 *		BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
 *		
 *		Revision 8.1  91/05/20  12:40:56  dickey
 *		mods to compile on apollo sr10.3
 *		
 *		Revision 8.0  90/03/30  12:50:15  ste_cm
 *		BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
 *		
 *		Revision 7.0  90/03/30  12:50:15  ste_cm
 *		BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
 *		
 *		Revision 6.1  90/03/30  12:50:15  dickey
 *		corrected code which checks for suppressed extensions (if a
 *		wildcard is given, we don't really know the exact length that
 *		would be matched).
 *		
 *		Revision 6.0  90/03/28  11:21:39  ste_cm
 *		BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
 *		
 *		Revision 5.5  90/03/28  11:21:39  dickey
 *		display the version-number in "-d" listing where I was only
 *		showing "ok".
 *		
 *		Revision 5.4  90/03/28  10:34:24  dickey
 *		added "-c" (configuration-list) option so user can obtain a
 *		list of the working files in a tree together with their
 *		revision codes.
 *		
 *		Revision 5.3  90/03/28  08:08:05  dickey
 *		added "-i" option.  made both "-i" and "-x" options permit
 *		wildcards.  enhanced usage-message.
 *		
 *		Revision 5.2  90/02/07  12:58:53  dickey
 *		added "-p" option to generate relative pathnames
 *
 *		Revision 5.1  90/02/07  11:14:26  dickey
 *		added "-l" option to reopen stderr so we can easily pipe the
 *		report to a file
 *
 *		Revision 5.0  89/10/26  10:20:32  ste_cm
 *		BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods
 *		+ ADA_PITS 4.0
 *
 *		Revision 4.1  89/10/26  10:20:32  dickey
 *		use new procedure 'istextfile()'
 *
 *		Revision 4.0  89/08/17  13:59:45  ste_cm
 *		BASELINE Thu Aug 24 09:31:25 EDT 1989 -- support:navi_011(rel2)
 *
 *		Revision 3.3  89/08/17  13:59:45  dickey
 *		if "-r" and "-o" are both set, use "-r" to limit the set of
 *		files shown as obsolete, rather than to select working files
 *		for date-comparison.
 *
 *		Revision 3.2  89/08/17  13:18:03  dickey
 *		expanded usage-message to multi-line display.  corrected a
 *		place where "?" constant was overwritten.  added "-d" and
 *		"-o" options.
 *
 *		Revision 3.1  89/08/17  09:02:42  dickey
 *		modified "-r" option to support reverse comparison (if user
 *		suffixes a "+" to option-value).
 *
 *		Revision 3.0  88/09/26  07:56:39  ste_cm
 *		BASELINE Mon Jun 19 13:17:34 EDT 1989
 *
 *		Revision 2.0  88/09/26  07:56:39  ste_cm
 *		BASELINE Thu Apr  6 09:32:39 EDT 1989
 *
 *		Revision 1.7  88/09/26  07:56:39  dickey
 *		sccs2rcs keywords
 *
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
#include	"ptypes.h"
#include	"rcsdefs.h"
#include	<ctype.h>
extern	char	*pathcat();
extern	char	*pathleaf();
extern	char	*rcs_dir();
extern	char	*relpath();
extern	char	*sccs_dir();
extern	char	*txtalloc();

extern	int	optind;		/* 'getopt()' index to argv */
extern	char	*optarg;	/* 'getopt()' argument in argv */

/************************************************************************
 *	local definitions						*
 ************************************************************************/

#define	WARN	FPRINTF(stderr,
#define	TELL	if(verbose >= 0) WARN
#define	VERBOSE	if(verbose >  0) WARN

typedef	struct	_exts	{
	struct	_exts	*link;
	char		*if_ext,	/* conditional extension */
			*no_ext;	/* extension to suppress */
	} EXTS;

	/*ARGSUSED*/
#define	def_alloc EXTS_alloc
	def_ALLOC(EXTS)

static	EXTS	*exts;			/* "-x" and "-t" extension list */
static	int	allnames;		/* "-a" option */
static	int	config_rev;		/* "-c" option */
static	int	debug;			/* "-d" option */
static	int	obsolete;		/* "-o" option */
static	int	verbose;		/* "-v" option */
static	int	lines;			/* line-number, for report */
static	char	*revision = "0";	/* required revision level */
static	int	reverse;		/* true if we reverse revision-test */

static	char	*original;		/* original directory, for "-p" */

/*
 * Define a new leaf-name to ignore.  Piggyback on the "-x" data structure by
 * defining the ".no_ext" member to null.
 */
static
ignore(string)
char	*string;
{
	EXTS	*savep = exts;
	exts = ALLOC(EXTS,1);
	exts->link = savep;
	exts->no_ext = 0;
	exts->if_ext = txtalloc(string);
	if (debug) TELL "ignore leaf \"%s\"\n", string);
}

/*
 * Define a new extension-to-ignore
 */
static
extension(string)
char	*string;
{
	EXTS	*savep = exts;
	char	*s = strrchr(string, *string);
	char	bfr[BUFSIZ];
	int	show = (debug || verbose > 0);

	exts = ALLOC(EXTS,1);
	exts->link = savep;
	exts->no_ext = txtalloc(s);
	exts->if_ext = 0;
	if (s != string) {
		strcpy(bfr, string)[s-string] = EOS;
		exts->if_ext = txtalloc(bfr);
		if (show) TELL "if(%s) ", exts->if_ext);
	}
	if (show) TELL "extension(%s)\n", exts->no_ext);
}

/*
 * Test the given filename to see if it has any of the suffixes which the user
 * wishes to ignore.  If so, return TRUE.
 */
static
suppress(name)
char	*name;
{
	register EXTS	*p;
	register int	len,
			off = strlen(name);
	auto	 char	bfr[BUFSIZ];
	struct	stat	sb;

	for (p = exts; p; p = p->link) {
		if (p->no_ext == 0) {	/* "-i" test? */
			if (!strwcmp(p->if_ext, name))
				return (TRUE);
		} else for (len = 0; len < off; len++) {
			if (!strwcmp(p->no_ext, name + len)) {
				if (p->if_ext) {	/* look for file */
					(void)strcpy(strcpy(bfr, name) + len,
						p->if_ext);
					if (stat(bfr, &sb) >= 0
					&& (sb.st_mode & S_IFMT) == S_IFREG)
						return (TRUE);
				} else {		/* unconditional */
					return (TRUE);
				}
			}
		}
	}
	return (FALSE);
}

/*
 * Indent the report for the given number of directory-levels.
 */
static
indent(level)
{
	++lines;
	if (verbose >= 0) {
		WARN "%4d:\t", lines);
		while (level-- > 0)
			WARN "|--%c", (level > 0) ? '-' : ' ');
	}
}

static
pipes(path, name, vers)
char	*path, *name, *vers;
{
	auto	char	tmp[BUFSIZ];

	if ((verbose < 0) || !isatty(fileno(stdout))) {
		if (config_rev) {
			if (*vers != '?')
				PRINTF("%s ", vers);
			else
				return;
		}
		path = pathcat(tmp, path, name);
		if (original != 0)
			path = relpath(path, original, path);
		PRINTF("%s\n", path);
	}
}

static
char	*
compared(what, rev)
char	*what, *rev;
{
	static	char	buffer[80];
	FORMAT(buffer, "%s than %s", what, rev);
	return (buffer);
}

/*
 * This procedure is invoked from 'walktree()' for each file/directory which
 * is found in the specified tree.  Analyze the files to see if anything should
 * be reported.  Report all directory names, so we can see the context of each
 * filename.
 */
static
do_stat(path, name, sp, readable, level)
char	*path;
char	*name;
struct	stat	*sp;
{
	char	*change	= 0,
		*vers,
		*owner,
		*locked_by = 0;
	time_t	cdate;
	int	mode	= (sp != 0) ? (sp->st_mode & S_IFMT) : 0;
	int	ok_text;

	if (mode == S_IFDIR) {
		auto	char	tmp[BUFSIZ],
				*s = pathcat(tmp, path, name),
				*t;
		abspath(s);		/* get rid of "." and ".." names */
		t = pathleaf(s);	/* obtain leaf-name for "-a" option */
		if (*t == '.' && !allnames)	return (-1);
		if (sameleaf(s, sccs_dir())
		||  sameleaf(s, rcs_dir())) {
			if (obsolete)
				do_obs(path, name, level);
			return (-1);
		}
	}

	if (readable < 0 || sp == 0) {
		VERBOSE "?? %s/%s\n", path, name);
	} else if (mode == S_IFDIR) {
		change = (name[strlen(name)-1] == '/') ? "" : "/";
#ifdef	S_IFLNK
		if ((lstat(name, sp) >= 0)
		&&  (sp->st_mode & S_IFMT) == S_IFLNK) {
			change = " (link)";
			readable = -1;
		}
#endif
		indent(level);
		TELL "%s%s\n", name, change);
	} else if (mode == S_IFREG && !suppress(name)) {
		rcslast(path, name, &vers, &cdate, &owner);
		if ((cdate == 0) && (*vers == '?'))
			sccslast(path, name, &vers, &cdate, &owner);
		if (*owner != EOS && *owner != '?')
			locked_by = owner;
		else
			owner = "";

		if (cdate != 0) {
			ok_text = TRUE;	/* assume this anyway */
			if (cdate == sp->st_mtime) {
				if (obsolete)
					;	/* interpret as obsolete-rev */
				else if (reverse) {
					if (dotcmp(vers, revision) > 0)
						change = vers;
				} else {
					if (dotcmp(vers, revision) < 0)
						change = vers;
				}
			} else if (cdate > sp->st_mtime) {
				change	= compared("older", vers);
			} else if (cdate < sp->st_mtime) {
				change	= compared("newer", vers);
			}
		} else if (ok_text = istextfile(name)) {
			change	= "not archived";
		}
		if ((change != 0) || locked_by != 0) {
			if (change == 0)	change	= "no change";
			indent(level);
			TELL "%s (%s%s%s)\n",
				name,
				change,
				(locked_by != 0) ? ", locked by " : "",
				owner);
			pipes(path, name, vers);
		} else if (debug) {
			indent(level);
			TELL "%s (%s)\n", name, ok_text ? vers : "binary");
			if (ok_text && config_rev)
				pipes(path, name, vers);
		}
	}
	return(readable);
}

/*
 * Scan a directory looking for obsolete archives.  This requires special
 * handling, since the directory-name may be a symbolic link; thus we have to
 * be careful where we look for the working file!
 */
static
do_obs(path, name, level)
char	*path;		/* current working directory, from 'do_stat()' */
char	*name;		/* name of directory (may be symbolic link) */
int	level;
{
	auto	DIR		*dp;
	auto	struct	direct	*de;
	auto	char		tpath[BUFSIZ],
				tname[BUFSIZ];
	auto	struct	stat	sb;
	auto	char	*tag,
			*vers,
			*owner;
	auto	time_t	cdate;

	if (!(dp = opendir(pathcat(tpath, path, name)))) {
		perror(tpath);
		return;
	}

	while (de = readdir(dp)) {
		if (dotname(de->d_name))	continue;
		if (stat(pathcat(tname, name, de->d_name), &sb) >= 0) {
			auto	int	show	= FALSE;

			if ((sb.st_mode & S_IFMT) != S_IFREG) {
				indent(level);
				TELL "%s (non-file)\n", tname);
				continue;
			}
			rcslast(path, tname, &vers, &cdate, &owner);
			if ((cdate == 0) && (*vers == '?'))
				sccslast(path, tname, &vers, &cdate, &owner);

			/*
			 * If 'cdate' is zero, then we could not (for whatever
			 * reason) find a working file.  If 'vers' is "?", then
			 * the file was not an archive, so we report this
			 * always.  Filter the remaining files according to
			 * revision codes.
			 */
			if (cdate == 0) {
				tag = "obsolete";
				if (*vers == '?') {
					tag = "non-archive";
					show = TRUE;
				} else if (reverse) {
					if (dotcmp(vers, revision) > 0)
						show = TRUE;
				} else {
					if (dotcmp(vers, revision) < 0)
						show = TRUE;
				}
			}

			if (show) {
				indent(level);
				TELL "%s (%s:%s)\n", tname, tag, vers);
				pipes(path, tname, vers);
			} else if (debug) {
				indent(level);
				TELL "%s (%s)\n", tname, vers);
			}
		}
	}
	(void)closedir(dp);
}

/*
 * Process a single argument: a directory name.
 */
static
do_arg(name)
char	*name;
{
	WARN "** path = %s\n", name);
	lines	= 0;
	(void)walktree((char *)0, name, do_stat, "r", 0);
}

usage()
{
	static	char	*msg[] = {
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
	register int	j;
	for (j = 0; j < sizeof(msg)/sizeof(msg[0]); j++)
		WARN "%s\n", msg[j]);
	(void)fflush(stderr);
	(void)exit(FAIL);
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/

main(argc, argv)
char	*argv[];
{
	auto	char	tmp[BUFSIZ];
	register int j;

	/* make stderr line-buffered, since we send our report that way */
#ifdef	IOLBF
	char	bfr[BUFSIZ];
	if (!(stderr->_flag & _IOLBF))
		(void)setvbuf(stderr, bfr, _IOLBF, sizeof(bfr));
#else
	(void)setlinebuf(stderr);
#endif

	while ((j = getopt(argc, argv, "acdi:l:opqr:stvx:")) != EOF)
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
			if (!fopen(optarg, "a+"))
				failed(optarg);
			(void)freopen(optarg, "a+", stderr);
			break;
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
			/*NOTREACHED*/
		}

	/*
	 * If we are asked to do a reverse-comparison, we must juggle the
	 * revision-level so that 'dotcmp()' can give us a useful return value.
	 */
	if (revision[j = strlen(revision)-1] == '+') {
		register char	*s;

		revision = strcpy(tmp, revision);
		revision[j] = EOS;
		if (s = strrchr(revision, '.')) {
			if (s[1] == EOS)
				(void)strcat(revision, "0");
		} else
			(void)strcat(revision, ".0");
		revision = txtalloc(revision);
		reverse  = TRUE;
	}

	/*
	 * Process the list of file-specifications:
	 */
	if (optind < argc) {
		for (j = optind; j < argc; j++)
			do_arg(argv[j]);
	} else
		do_arg(".");

	(void)fflush(stderr);
	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
