#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/cm_tools.vcs/src/checkout/src/RCS/rcsget.c,v 9.4 1991/09/25 13:48:08 dickey Exp $";
#endif

/*
 * Title:	rcsget.c (rcs get-tree)
 * Author:	T.E.Dickey
 * Created:	19 Oct 1989
 * Modified:
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
#include	"ptypes.h"
#include	"rcsdefs.h"
extern	char	*pathcat();
extern	char	*pathleaf();
extern	char	*sccs_dir();

#define	isDIR(mode)	((mode & S_IFMT) == S_IFDIR)
#define	isFILE(mode)	((mode & S_IFMT) == S_IFREG)

#ifdef	S_IFLNK
#define	isLINK(mode)	((mode & S_IFMT) == S_IFLNK)
#endif

#define	VERBOSE	if (!quiet) PRINTF

static	char	working[BUFSIZ];	/* working-directory for scan_archive */
static	char	co_opts[BUFSIZ];
static	char	*verb	= "checkout";
static	int	a_opt;		/* all-directory scan */
static	int	R_opt;		/* recur/directory-mode */
static	int	L_opt;		/* follow links */
static	int	n_opt;		/* no-op mode */
static	int	quiet;		/* "-q" option */
static	int	x_opt;		/* "-x" option */

static
set_wd(path)
char	*path;
{
	if (!n_opt)
		if (chdir(path) < 0)
			failed(path);
}

static
checkout(name)
char	*name;
{
	auto	char	args[BUFSIZ];

	catarg(strcpy(args, co_opts), name);

	if (!quiet || n_opt) shoarg(stdout, verb, args);
	if (!n_opt) {
		if (execute(verb, args) < 0)
			failed(name);
	}
}

static
an_archive(name)
char	*name;
{
	register int	len_name = strlen(name),
			len_type = strlen(RCS_SUFFIX);
	return (len_name > len_type
	&&  !strcmp(name + len_name - len_type, RCS_SUFFIX));
}

static
Ignore(name, why)
char	*name, *why;
{
	VERBOSE("?? ignored: %s%s\n", name, why);
}

static
/*ARGSUSED*/
scan_archive(path, name, sp, ok_acc, level)
char	*path;
char	*name;
struct	stat	*sp;
{
	auto	char	tmp[BUFSIZ];

	if (!strcmp(working,path))	/* account for initial argument */
		return (ok_acc);
	if (!isFILE(sp->st_mode)
	||  !an_archive(name)) {
		Ignore(name, " (not an archive)");
		return (-1);
	}
	if (!strcmp(vcs_file((char *)0, strcpy(tmp,name),FALSE), name))
		return (ok_acc);

	set_wd(working);
	checkout(rcs2name(strcpy(tmp, name),x_opt));
	set_wd(path);
	return(ok_acc);
}

static
scan_tree(path, name, sp, ok_acc, level)
char	*path;
char	*name;
struct	stat	*sp;
{
	auto	char	tmp[BUFSIZ],
			*s = pathcat(tmp, path, name);
	auto	struct	stat	sb;

	if (RCS_DEBUG)
		PRINTF("++ %s%sscan (%s, %s, %s%d)\n",
			R_opt ? "R " : "",
			L_opt ? "L " : "",
			path, name, (sp == 0) ? "no-stat, " : "", level);

	if (sp == 0) {
		if (R_opt && (level > 0)) {
			Ignore(name, " (no such file)");
		} else
			checkout(name);
	} else if (isDIR(sp->st_mode)) {
		abspath(s);		/* get rid of "." and ".." names */
		if (!a_opt && *pathleaf(s) == '.')
			ok_acc = -1;
		else if (sameleaf(s, sccs_dir()))
			ok_acc = -1;
		else if (sameleaf(s, rcs_dir())) {
			if (R_opt) {
				(void)walktree(strcpy(working,path),
					name, scan_archive, "r", level);
			}
			ok_acc = -1;
		} else {
#ifdef	S_IFLNK
			if (!L_opt
			&&  (lstat(s, &sb) < 0 || isLINK(sb.st_mode))) {
				Ignore(name, " (is a link)");
				ok_acc = -1;
			} else
#endif
			if (!quiet || n_opt)
				track_wd(path);
		}
	} else if (isFILE(sp->st_mode)) {
		if (!quiet || n_opt)
			track_wd(path);
		if (R_opt && (level > 0)) {
			;
		} else if (stat(name2rcs(name,x_opt), &sb) >= 0
		    &&	isFILE(sb.st_mode))
			checkout(name);
		else
			Ignore(name, RCS_DEBUG ? " (no archive for it)" : "");
	} else {
		Ignore(name, RCS_DEBUG ? " (not a file)" : "");
		ok_acc = -1;
	}
	return(ok_acc);
}

static
do_arg(name)
char	*name;
{
	VERBOSE("** process %s\n", name);
#ifdef	S_IFLNK
	if (!L_opt) {
		struct	stat	sb;
		if (lstat(name, &sb) >= 0 && isLINK(sb.st_mode)) {
			Ignore(name, " (is a link)");
			return;
		}
	}
#endif
	(void)walktree((char *)0, name, scan_tree, "r", 0);
}

static
usage(option)
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
		checkout("-?");
	exit(FAIL);
}

main(argc, argv)
char	*argv[];
{
	register int	j;
	register char	*s, *t;
	auto	 int	had_args = FALSE;

	track_wd((char *)0);
	for (j = 1; j < argc; j++) {
		if (*(s = argv[j]) == '-') {
			t = s + strlen(s);
			if (strchr("lpqrcswjx", s[1]) != 0) {
				catarg(co_opts, s);
				switch (s[1]) {
				case 'q':	quiet = TRUE;	break;
				case 'x':	x_opt = TRUE;	break;
				}
			} else while (s[1]) {
				switch (s[1]) {
				case 'a':	a_opt = TRUE;	break;
				case 'R':
				case 'd':	R_opt = TRUE;	break;
				case 'L':	L_opt = TRUE;	break;
				case 'n':	n_opt = TRUE;	break;
				case 'T':	verb  = s+2;	s = t; break;
				default:	usage(s[1]);
				}
				s++;
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
