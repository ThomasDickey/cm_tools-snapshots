#ifndef	lint
static	char	Id[] = "$Id: checkup.c,v 5.2 1990/02/07 12:58:53 dickey Exp $";
#endif	lint

/*
 * Title:	checkup.c (link/directory tree)
 * Author:	T.E.Dickey
 * Created:	31 Aug 1988
 * $Log: checkup.c,v $
 * Revision 5.2  1990/02/07 12:58:53  dickey
 * added "-p" option to generate relative pathnames
 *
 *		Revision 5.1  90/02/07  11:14:26  dickey
 *		added "-l" option to reopen stderr so we can easily pipe the
 *		report to a file
 *		
 *		Revision 5.0  89/10/26  10:20:32  ste_cm
 *		BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
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
 * Options:	-a	scans directory-names beginning with '.' (normally
 *			ignored).
 *		-d	debug, shows all names and dates
 *		-o	reports "obsolete" files, i.e., those archives for
 *			which there is no corresponding working-file.
 *		-p	sends pathnames in relative-format.  This is useful
 *			when a set of full pathnames would be too long for the
 *			prevailing shell.
 *		-r REV	reports all working files whose highest checked-in
 *			version is below REV.  (A "+" suffixed causes the
 *			report to reverse, showing files above REV).
 *		-q (-s)	suppress report, used when only a list of names is
 *			wanted.
 *		-t	sets up a default list of extensions to be ignored.
 *
 *		-x XXX	specifies an extension to be ignored.  The first
 *			character in the option-value is used as the delimiter.
 *			If repeated, it also marks off a second extension,
 *			which must correspond to an existing file before the
 *			file with the end-extension is ignored.  For example,
 *
 *				-x .e.c
 *
 *			causes all ".c" files which have a corresponding
 *			".e" file to be ignored.
 *
 *			If more than one "-x" option is given, the most recent
 *			is processed first.
 *
 *		-v	verbose option directs this program to print the names
 *			which cannot be processed (i.e., sockets, and stuff that
 *			cannot be 'stat'ed.
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
static	int	debug;			/* "-d" option */
static	int	obsolete;		/* "-o" option */
static	int	verbose;		/* "-v" option */
static	int	lines;			/* line-number, for report */
static	char	*revision = "0";	/* required revision level */
static	int	reverse;		/* true if we reverse revision-test */

static	char	*original;		/* original directory, for "-p" */

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

	exts = ALLOC(EXTS,1);
	exts->link = savep;
	exts->no_ext = txtalloc(s);
	exts->if_ext = 0;
	if (s != string) {
		strcpy(bfr, string)[s-string] = EOS;
		exts->if_ext = txtalloc(bfr);
		VERBOSE "if(%s) ", exts->if_ext);
	}
	VERBOSE "extension(%s)\n", exts->no_ext);
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
		if ((len = strlen(p->no_ext)) <= off) {
			if (!strcmp(p->no_ext, &name[off-len])) {
				if (p->if_ext) {	/* look for file */
					(void)strcpy(strcpy(bfr, name)
							+ off-len,
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
pipes(path, name)
char	*path, *name;
{
	auto	char	tmp[BUFSIZ];

	if ((verbose < 0) || !isatty(fileno(stdout))) {
		path = pathcat(tmp, path, name);
		if (original != 0)
			path = relpath(path, original, path);
		PRINTF("%s\n", path);
	}
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
		*owner;
	time_t	cdate;
	int	mode	= (sp != 0) ? (sp->st_mode & S_IFMT) : 0;

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
#endif	S_IFLNK
		indent(level);
		TELL "%s%s\n", name, change);
	} else if (mode == S_IFREG && !suppress(name)) {
		rcslast(path, name, &vers, &cdate, &owner);
		if ((cdate == 0) && (*vers == '?'))
			sccslast(path, name, &vers, &cdate, &owner);
		if (cdate != 0) {
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
				change	= "older";
			} else if (cdate < sp->st_mtime) {
				change	= "newer";
			}
		} else if (istextfile(name)) {
			change	= "not archived";
		}
		if ((change != 0) || (*owner != EOS && *owner != '?')) {
			if (change == 0)	change	= "no change";
			if (*owner == '?')	owner	= "";
			indent(level);
			TELL "%s (%s%s%s)\n",
				name,
				change,
				*owner ? ", locked by " : "",
				owner);
			pipes(path, name);
		} else if (debug) {
			indent(level);
			TELL "%s (ok)\n", name);
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
	auto	char	*vers,
			*owner;
	auto	time_t	cdate;

	if (dp = opendir(pathcat(tpath, path, name))) {
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
				 * If 'cdate' is zero, then we could not (for
				 * whatever reason) find a working file.  If
				 * 'vers' is "?", then the file was not an
				 * archive, so we report this always.  Filter
				 * the remaining files according to revision
				 * codes.
				 */
				if (cdate == 0) {
					if (*vers == '?')
						show = TRUE;
					else if (reverse) {
						if (dotcmp(vers, revision) > 0)
							show = TRUE;
					} else {
						if (dotcmp(vers, revision) < 0)
							show = TRUE;
					}
				}

				if (show) {
					indent(level);
					TELL "%s (obsolete:%s)\n", tname, vers);
					pipes(path, name);
				} else if (debug) {
					indent(level);
					TELL "%s (ok) %s\n",
						tname,
						(cdate == 0) ? vers : "");
				}
			}
		}
		(void)closedir(dp);
	}
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
	"usage: checkup [options] [directory [...]]",
	"",
	"options:",
	"  -a      report files and directories beginning with \".\"",
	"  -d      debug (show all names)",
	"  -l FILE write logfile of tree-of-files",
	"  -o      reports obsolete files (no working file exists)",
	"  -p      generate pathnames relative to current directory",
	"  -q      quiet",
	"  -r REV  specifies revision-level to check against, REV+ to reverse",
	"  -s      (same as -q)",
	"  -t      suppresses standard extensions: .bak .i .log .out .tmp",
	"  -v      verbose",
	"  -x EXT  extension rules: .EXT to suppress, .IF.THEN conditionally"
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

	while ((j = getopt(argc, argv, "adl:opqr:stvx:")) != EOF)
		switch (j) {
		case 'a':
			allnames++;
			break;
		case 'd':
			debug++;
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
