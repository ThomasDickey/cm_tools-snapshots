#ifndef	lint
static	char	sccs_id[] = "$Header: /users/source/archives/cm_tools.vcs/src/checkup/src/RCS/checkup.c,v 3.0 1988/09/26 07:56:39 ste_cm Rel $";
#endif	lint

/*
 * Title:	checkup.c (link/directory tree)
 * Author:	T.E.Dickey
 * Created:	31 Aug 1988
 * $Log: checkup.c,v $
 * Revision 3.0  1988/09/26 07:56:39  ste_cm
 * BASELINE Mon Jun 19 13:17:34 EDT 1989
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
 *		-r REV	reports all working files whose highest checked-in
 *			version is below REV.
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

#include	"ptypes.h"
#include	"rcsdefs.h"
#include	<ctype.h>
extern	char	*pathcat();
extern	char	*pathleaf();
extern	char	*rcs_dir();
extern	char	*sccs_dir();
extern	char	*strcpy();
extern	char	*strrchr();
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
static	int	verbose;		/* "-v" option */
static	int	lines;			/* line-number, for report */
static	char	*revision = "0";	/* required revision level */

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
 * Open the given file, and scan it to see if it consists of entirely
 * "readable" text.  If any exception is found, return TRUE.
 */
static
isbinary(name)
char	*name;
{
	FILE	*fp;
	char	bfr[BUFSIZ];
	int	binfile	= FALSE;

	if (fp = fopen(name, "r")) {
		register char	*s;
		register int	n;
		while ((n = fread(s = bfr, sizeof(char), sizeof(bfr), fp)) > 0)
			while (n-- > 0) {
				register c = *s++;
				if (!(isascii(c)
					&& (isprint(c) || isspace(c)))) {
					binfile = TRUE;
					break;
				}
			}
		FCLOSE(fp);
	} else {
		VERBOSE "?? cannot open %s\n", name);
		binfile = TRUE;	/* cannot open, don't complain? */
	}
	return (binfile);
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
		if (sameleaf(s, sccs_dir()))	return (-1);
		if (sameleaf(s, rcs_dir()))	return (-1);
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
		if (cdate == 0)
			sccslast(path, name, &vers, &cdate, &owner);
		if (cdate != 0) {
			if (cdate == sp->st_mtime) {
				if (dotcmp(vers, revision) < 0)
					change = vers;
			} else if (cdate > sp->st_mtime) {
				change	= "older";
			} else if (cdate < sp->st_mtime) {
				change	= "newer";
			}
		} else if (!isbinary(name)) {
			change	= "not archived";
		}
		if (change || (*owner != EOS && *owner != '?')) {
			if (change == 0)	change	= "no change";
			if (*owner == '?')	*owner	= EOS;
			indent(level);
			TELL "%s (%s%s%s)\n",
				name,
				change,
				*owner ? ", locked by " : "",
				owner);
			if ((verbose < 0) || !isatty(fileno(stdout)))
				PRINTF("%s/%s\n", path, name);
		}
	}
	return(readable);
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
	WARN "usage: checkup [-r rev] [-aqstv] [-x extension] [directory [...]]\n");
	(void)exit(FAIL);
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/

main(argc, argv)
char	*argv[];
{
	register int j;

	/* make stderr line-buffered, since we send our report that way */
#ifdef	IOLBF
	char	bfr[BUFSIZ];
	if (!(stderr->_flag & _IOLBF))
		(void)setvbuf(stderr, bfr, _IOLBF, sizeof(bfr));
#else
	(void)setlinebuf(stderr);
#endif

	while ((j = getopt(argc, argv, "aqr:stvx:")) != EOF)
		switch (j) {
		case 'a':
			allnames++;
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
	if (optind < argc) {
		for (j = optind; j < argc; j++)
			do_arg(argv[j]);
	} else
		do_arg(".");

	(void)exit(SUCCESS);
	/*NOTREACHED*/
}

failed(s)
char	*s;
{
	perror(s);
	(void)exit(FAIL);
}
