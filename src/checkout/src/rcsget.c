#ifndef	lint
static	char	Id[] = "$Id: rcsget.c,v 5.0 1989/10/26 12:08:49 ste_cm Rel $";
#endif	lint

/*
 * Title:	rcsget.c (rcs get-tree)
 * Author:	T.E.Dickey
 * Created:	19 Oct 1989
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

#define	isDIR(mode)	((mode & S_IFMT) == S_IFDIR)
#define	isFILE(mode)	((mode & S_IFMT) == S_IFREG)

static	char	working[BUFSIZ];	/* working-directory for scan_archive */
static	char	co_opts[BUFSIZ];
static	int	a_opt;		/* all-directory scan */
static	int	d_opt;		/* directory-mode */
static	int	n_opt;		/* no-op mode */

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
	static	char	*verb	= "checkout";
	auto	char	args[BUFSIZ];

	catarg(strcpy(args, co_opts), name);

	PRINTF("%% %s %s\n", verb, args);
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
		PRINTF("?? ignored: %s\n", name);
		return (-1);
	}
	if (!strcmp(vcs_file((char *)0, strcpy(tmp,name),FALSE), name))
		return (ok_acc);

	set_wd(working);
	checkout(rcs2name(strcpy(tmp, name)));
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
			*s = pathcat(tmp, path, name),
			*t;
	auto	struct	stat	sb;

	if (isDIR(sp->st_mode)) {
		abspath(s);		/* get rid of "." and ".." names */
		if (!a_opt) {
			t = pathleaf(s);
			if (*t == '.')
				return (-1);
		}
		if (sameleaf(s, sccs_dir()))
			return (-1);
		if (sameleaf(s, rcs_dir())) {
			if (d_opt) {
				(void)walktree(strcpy(working,path),
					name, scan_archive, "r", level);
			}
			return (-1);
		}
		track_wd(path);
	} else if (isFILE(sp->st_mode)) {
		track_wd(path);
		if (d_opt && (level > 0)) {
			return (ok_acc);
		}
		if (stat(name2rcs(name), &sb) >= 0
		&&  isFILE(sb.st_mode))
			checkout(name);
		else
			PRINTF("?? ignored: %s\n", name);
	} else
		return (-1);
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
 "usage: [options] files"
,""
,"Options include all CHECKOUT options, plus:"
," -a    process all directories, including those beginning with \".\""
," -d    directory-mode (scan based on archives, rather than working files"
," -n    no-op (show what would be checked-out, but don't do it"
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
	for (j = 1; j < argc; j++) {
		if (*(s = argv[j]) == '-') {
			if (strchr("lpqrcswj", s[1]) != 0)
				catarg(co_opts, s);
			else switch (s[1]) {
			case 'a':	a_opt = TRUE;	break;
			case 'd':	d_opt = TRUE;	break;
			case 'n':	n_opt = TRUE;	break;
			default:	usage();
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
