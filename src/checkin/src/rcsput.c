#ifndef	lint
static	char	Id[] = "$Id: rcsput.c,v 5.1 1989/11/01 15:18:40 dickey Exp $";
#endif	lint

/*
 * Title:	rcsput.c (rcs put-tree)
 * Author:	T.E.Dickey
 * Created:	19 Oct 1989
 * $Log: rcsput.c,v $
 * Revision 5.1  1989/11/01 15:18:40  dickey
 * walktree passes null pointer to stat-block if no-access.
 *
 *
 * Function:	Use 'checkin' to checkin one or more files from the
 *		RCS-directory which is located in the current working
 *		directory, and then, to set the delta date of the  checked-in
 *		files according to the last modification date (rather than the
 *		current date, as RCS assumes).
 *
 * Options:	all of the rcs 'ci' options, plus:
 *
 *		-a (process all  directories, including those beginning with .)
 *		-b (passed to 'diff' in display of differences)
 *		-c (use 'cat' for paging diffs)
 *		-d (suppress check-in process, do differences only)
 *		-h (passed to 'diff' in display of differences)
 *		-L (specify logfile for differences)
 */

#define	STR_PTYPES
#include	"ptypes.h"
#include	"rcsdefs.h"
extern	FILE	*popen();
extern	char	*dftenv();
extern	char	*pathcat();
extern	char	*pathleaf();
extern	char	*sccs_dir();

#define	isDIR(mode)	((mode & S_IFMT) == S_IFDIR)
#define	isFILE(mode)	((mode & S_IFMT) == S_IFREG)
#define	VERBOSE		if (!quiet) PRINTF

static	char	ci_opts[BUFSIZ];
static	char	diff_opts[BUFSIZ];
static	FILE	*log_fp;
static	int	a_opt;		/* all-directory scan */
static	int	no_op;		/* no-op mode */
static	char	*pager;		/* nonzero if we don't cat diffs */
static	int	quiet;

static
filesize(name)
char	*name;
{
	auto	struct	stat	sb;
	return (stat(name, &sb) >= 0 && (sb.st_mode & S_IFMT) == S_IFREG
		? sb.st_size : -1);
}

static
cat2fp(fp, name)
FILE	*fp;
char	*name;
{
	auto	FILE	*ifp;
	auto	char	t[BUFSIZ];
	auto	int	n;

	if (ifp = fopen(name, "r")) {
		while ((n = fread(t, sizeof(char), sizeof(t), ifp)) > 0)
			if (fwrite(t, sizeof(char), n, fp) != n)
				break;
		FCLOSE(ifp);
	}
}

static
different(working)
char	*working;
{
	auto	FILE	*ifp, *ofp;
	auto	char	buffer[BUFSIZ],
			out_diff[BUFSIZ];
	auto	int	changed	= FALSE,
			n;

	FORMAT(buffer, "rcsdiff %s %s", diff_opts, working);
	VERBOSE("%% %s\n", buffer);
	if (!tmpnam(out_diff) || !(ofp = fopen(out_diff,"w")))
		failed("tmpnam");

	if (!(ifp = popen(buffer, "r")))
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
	} else {
		PRINTF("*** no differences found ***\n");
	}

	(void)unlink(out_diff);
	return (changed);
}

static
checkin(path,name)
char	*path;
char	*name;
{
	auto	char	args[BUFSIZ];
	auto	char	*working = rcs2name(name);
	auto	char	*archive = name2rcs(name);
	auto	int	first;

	if (first = (filesize(archive) < 0)) {
		if (!istextfile(working)) {
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
			if (!different(working))
				return;
			first = FALSE;
		}
	}

	(void)strcpy(args, ci_opts);
	catarg(args, working);

	if (!no_op) {
		PRINTF("*** %s \"%s\"\n",
			first	? "Initial RCS insertion of"
				: "Applying RCS delta to",
			name);
		VERBOSE("%% checkin %s\n", args);
		if (execute("checkin", args) < 0)
			failed(working);
	} else {
		PRINTF("--- %s \"%s\"\n",
			first	? "This would be initial for"
				: "Delta would be applied to",
			name);
	}
}

/*ARGSUSED*/
static
scan_tree(path, name, sp, ok_acc, level)
char	*path;
char	*name;
struct	stat	*sp;
{
	auto	char	tmp[BUFSIZ],
			*s = pathcat(tmp, path, name);

	if (sp == 0)
		ok_acc = -1;
	else if (isDIR(sp->st_mode)) {
		abspath(s);		/* get rid of "." and ".." names */
		if (!a_opt && *pathleaf(s) == '.')
			ok_acc = -1;
		else if (sameleaf(s, sccs_dir())
		    ||	 sameleaf(s, rcs_dir()))
			ok_acc = -1;
		else
			track_wd(path);
	} else if (isFILE(sp->st_mode)) {
		track_wd(path);
		checkin(path,name);
	} else
		ok_acc = -1;

	return(ok_acc);
}

static
do_arg(name)
char	*name;
{
	(void)walktree((char *)0, name, scan_tree, "r", 0);
}

usage()
{
	static	char	*tbl[] = {
 "usage: rcsput [options] files"
,""
,"Options include all CHECKIN-options, plus:"
,"  -a       process all directories, including those beginning with \".\""
,"  -b       (passed to rcsdiff)"
,"  -h       (passed to rcsdiff)"
,"  -c       send differences to terminal without $PAGER filtering"
,"  -d       compute differences only, don't try to CHECKIN"
,"  -L file  write all differences to logfile"
	};
	register int	j;
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	exit(FAIL);
}

main(argc, argv)
char	*argv[];
{
	register int	j;
	register char	*s;
	auto	 int	had_args = FALSE;

	track_wd((char *)0);
	pager = dftenv("more -l", "PAGER");
	for (j = 1; j < argc; j++) {
		if (*(s = argv[j]) == '-') {
			if (strchr("qrfklumnNst", s[1]) != 0) {
				catarg(ci_opts, s);
				if (s[1] == 'q') {
					quiet = TRUE;
					catarg(diff_opts, s);
				}
			} else {
				switch (s[1]) {
				case 'a':	a_opt = TRUE;		break;
				case 'b':
				case 'h':	catarg(diff_opts, s);	break;
				case 'c':	pager = 0;		break;
				case 'd':	no_op = TRUE;		break;
				case 'L':	if (s[2] == EOS)
							s = "logfile";
						if (!(log_fp = fopen(s, "a+")))
							usage();
						break;
				default:	usage();
				}
			}
		} else {
			do_arg(s);
			had_args = TRUE;
		}
	}

	if (!had_args)
		do_arg(".");
	exit(SUCCESS);
	/*NOTREACHED*/
}
