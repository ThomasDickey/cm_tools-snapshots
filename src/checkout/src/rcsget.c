#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/cm_tools.vcs/src/checkout/src/RCS/rcsget.c,v 11.0 1992/02/06 12:57:07 ste_cm Rel $";
#endif

/*
 * Title:	rcsget.c (rcs get-tree)
 * Author:	T.E.Dickey
 * Created:	19 Oct 1989
 * Modified:
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
#include	<rcsdefs.h>
#include	<sccsdefs.h>
#include	<errno.h>

#define	VERBOSE	if (!quiet) PRINTF

static	char	user_wd[BUFSIZ];/* working-directory for scan_archive */
static	char	co_opts[BUFSIZ];
static	char	*verb	= "checkout";
static	int	a_opt;		/* all-directory scan */
static	int	R_opt;		/* recur/directory-mode */
static	int	L_opt;		/* follow links */
static	int	n_opt;		/* no-op mode */
static	int	quiet;		/* "-q" option */

static
set_wd(
_AR1(char *,	path))
_DCL(char *,	path)
{
	if (!n_opt)
		if (chdir(path) < 0)
			failed(path);
}

static
void
Checkout(
_ARX(char *,	working)
_AR1(char *,	archive)
	)
_DCL(char *,	working)
_DCL(char *,	archive)
{
	auto	char	args[BUFSIZ];

	(void)strcpy(args, co_opts);
	catarg(args, working);
	catarg(args, archive);

	if (!quiet || n_opt) shoarg(stdout, verb, args);
	if (!n_opt) {
		if (execute(verb, args) < 0)
			failed(working);
	}
}

static
int
an_archive(
_AR1(char *,	name))
_DCL(char *,	name)
{
	register int	len_name = strlen(name),
			len_type = strlen(RCS_SUFFIX);
	return (len_name > len_type
	&&  !strcmp(name + len_name - len_type, RCS_SUFFIX));
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
	if (!a_opt && *pathleaf(path) == '.'
	 && sameleaf(path, sccs_dir())) {
		if (!quiet) PRINTF("...skip %s\n", path);
		return TRUE;
	}
	return FALSE;
}

static
void
Ignore(
_ARX(char *,	name)
_AR1(char *,	why)
	)
_DCL(char *,	name)
_DCL(char *,	why)
{
	VERBOSE("?? ignored: %s%s\n", name, why);
}

static
/*ARGSUSED*/
WALK_FUNC(scan_archive)
{
	auto	char	tmp[BUFSIZ];

	if (!strcmp(user_wd,path))	/* account for initial argument */
		return (readable);
	if (!isFILE(sp->st_mode)
	||  !an_archive(name)) {
		Ignore(name, " (not an archive)");
		return (-1);
	}
	if (!strcmp(vcs_file((char *)0, strcpy(tmp,name),FALSE), name))
		return (readable);

	set_wd(user_wd);
	Checkout(name, pathcat(tmp, rcs_dir(), name));
	set_wd(path);
	return(readable);
}

static
WALK_FUNC(scan_tree)
{
	auto	char	tmp[BUFSIZ],
			*s = pathcat(tmp, path, name);
	auto	STAT	sb;

	if (RCS_DEBUG)
		PRINTF("++ %s%sscan (%s, %s, %s%d)\n",
			R_opt ? "R " : "",
			L_opt ? "L " : "",
			path, name, (sp == 0) ? "no-stat, " : "", level);

	if (!quiet || n_opt)
		track_wd(path);

	if (sp == 0) {
		if (R_opt && (level > 0)) {
			Ignore(name, " (no such file)");
		}
	} else if (isDIR(sp->st_mode)) {
		abspath(s);		/* get rid of "." and ".." names */
		if (ignore_dir(s))
			readable = -1;
		else if (sameleaf(s, rcs_dir())) {
			if (R_opt) {
				(void)walktree(strcpy(user_wd,path),
					name, scan_archive, "r", level);
			}
			readable = -1;
		} else {
#ifdef	S_IFLNK
			if (!L_opt
			&&  (lstat(s, &sb) < 0 || isLINK(sb.st_mode))) {
				Ignore(name, " (is a link)");
				readable = -1;
			}
#endif
		}
	} else if (!isFILE(sp->st_mode)) {
		Ignore(name, RCS_DEBUG ? " (not a file)" : "");
		readable = -1;
	}
	return(readable);
}

static
void
do_arg(
_AR1(char *,	name))
_DCL(char *,	name)
{
#ifdef	S_IFLNK
	if (!L_opt) {
		STAT	sb;
		if (lstat(name, &sb) >= 0 && isLINK(sb.st_mode)) {
			Ignore(name, " (is a link)");
			return;
		}
	}
#endif
	(void)walktree((char *)0, name, scan_tree, "r", 0);
}

static
void
usage(
_AR1(int,	option))
_DCL(int,	option)
{
	static	char	*tbl[] = {
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
	register int	j;
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	if (option == '?')
		(void)execute(verb, "-?");
	exit(FAIL);
}

/*ARGSUSED*/
_MAIN
{
	register int	j;
	register char	*s, *t;

	track_wd((char *)0);

	/* process options */
	for (j = 1; (j < argc) && (*(s = argv[j]) == '-'); j++) {
		t = s + strlen(s);
		if (strchr("lpqrcswj", s[1]) != 0) {
			catarg(co_opts, s);
			if (s[1] == 'q')
				quiet = TRUE;
		} else while (s[1]) {
			switch (s[1]) {
			case 'a':	a_opt = TRUE;		break;
			case 'R':
			case 'd':	R_opt = TRUE;		break;
			case 'L':	L_opt = TRUE;		break;
			case 'n':	n_opt = TRUE;		break;
			case 'T':	verb  = s+2;	s = t;	break;
			default:	usage(s[1]);
			}
			s++;
		}
	}

	/* process filenames */
	if (j < argc) {
		while (j < argc) {
			char	working[MAXPATHLEN];
			char	archive[MAXPATHLEN];
			STAT	sb;

			j = rcsargpair(j, argc, argv);
			if (rcs_working(working, &sb) < 0 && errno != EISDIR)
				failed(working);

			if (isDIR(sb.st_mode)) {
				if (!ignore_dir(working))
					do_arg(working);
			} else {
				(void)rcs_archive(archive, (STAT *)0);
				Checkout(working, archive);
			}
		}
	} else
		do_arg(".");

	exit(SUCCESS);
	/*NOTREACHED*/
}
