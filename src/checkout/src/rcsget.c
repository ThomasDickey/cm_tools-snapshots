#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/cm_tools.vcs/src/checkout/src/RCS/rcsget.c,v 8.1 1991/05/20 12:38:24 dickey Exp $";
#endif

/*
 * Title:	rcsget.c (rcs get-tree)
 * Author:	T.E.Dickey
 * Created:	19 Oct 1989
 * $Log: rcsget.c,v $
 * Revision 8.1  1991/05/20 12:38:24  dickey
 * mods to compile on apollo sr10.3
 *
 *		Revision 8.0  90/08/14  14:08:48  ste_cm
 *		BASELINE Tue Aug 14 14:11:43 1990 -- ADA_TRANS, LINCNT
 *		
 *		Revision 7.1  90/08/14  14:08:48  dickey
 *		lint
 *		
 *		Revision 7.0  90/04/19  08:25:01  ste_cm
 *		BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
 *		
 *		Revision 6.3  90/04/19  08:25:01  dickey
 *		added "-T" option so that 'checkout' isn't hard-coded.
 *		
 *		Revision 6.2  90/04/18  16:36:24  dickey
 *		changed call on rcs2name/name2rcs to support "-x" option in
 *		checkin/checkout
 *		
 *		Revision 6.1  90/04/16  13:07:17  dickey
 *		interpret "-q" (quiet) option in this program
 *		
 *		Revision 6.0  89/11/03  08:09:33  ste_cm
 *		BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
 *		
 *		Revision 5.3  89/11/03  08:09:33  dickey
 *		additional correction: if file does not exist, it is ok to
 *		ask 'checkout' to get it!  Added a hack ("-?" option) to
 *		get checkout to show its options in this usage-message.
 *		
 *		Revision 5.2  89/11/02  15:36:38  dickey
 *		oops: did cleanup, but not bug-fix!
 *		
 *		Revision 5.1  89/11/01  15:09:41  dickey
 *		walktree passes null pointer to stat-block if no-access.
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

#define	VERBOSE	if (!quiet) PRINTF

static	char	working[BUFSIZ];	/* working-directory for scan_archive */
static	char	co_opts[BUFSIZ];
static	char	*verb	= "checkout";
static	int	a_opt;		/* all-directory scan */
static	int	d_opt;		/* directory-mode */
static	int	n_opt;		/* no-op mode */
static	int	quiet;		/* "-q" option */

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

	VERBOSE("%% %s %s\n", verb, args);
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
		VERBOSE("?? ignored: %s\n", name);
		return (-1);
	}
	if (!strcmp(vcs_file((char *)0, strcpy(tmp,name),FALSE), name))
		return (ok_acc);

	set_wd(working);
	checkout(rcs2name(strcpy(tmp, name),FALSE));
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

	if (sp == 0)
		checkout(name);
	else if (isDIR(sp->st_mode)) {
		abspath(s);		/* get rid of "." and ".." names */
		if (!a_opt && *pathleaf(s) == '.')
			ok_acc = -1;
		else if (sameleaf(s, sccs_dir()))
			ok_acc = -1;
		else if (sameleaf(s, rcs_dir())) {
			if (d_opt) {
				(void)walktree(strcpy(working,path),
					name, scan_archive, "r", level);
			}
			ok_acc = -1;
		} else if (!quiet)
			track_wd(path);
	} else if (isFILE(sp->st_mode)) {
		if (!quiet)
			track_wd(path);
		if (d_opt && (level > 0)) {
			;
		} else if (stat(name2rcs(name,FALSE), &sb) >= 0
		    &&	isFILE(sb.st_mode))
			checkout(name);
		else
			VERBOSE("?? ignored: %s\n", name);
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

static
usage(option)
{
	static	char	*tbl[] = {
 "usage: rcsget [options] files"
,""
,"Options include all CHECKOUT options, plus:"
,"  -a       process all directories, including those beginning with \".\""
,"  -d       directory-mode (scan based on archives, rather than working files"
,"  -n       no-op (show what would be checked-out, but don't do it"
,"  -q       quiet (also passed to CHECKOUT)"
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
	register char	*s;
	auto	 int	had_args = FALSE;

	track_wd((char *)0);
	for (j = 1; j < argc; j++) {
		if (*(s = argv[j]) == '-') {
			if (strchr("lpqrcswj", (size_t)s[1]) != 0) {
				catarg(co_opts, s);
				if (s[1] == 'q')
					quiet = TRUE;
			} else switch (s[1]) {
			case 'a':	a_opt = TRUE;	break;
			case 'd':	d_opt = TRUE;	break;
			case 'n':	n_opt = TRUE;	break;
			case 'T':	verb  = s+2;	break;
			default:	usage(s[1]);
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
