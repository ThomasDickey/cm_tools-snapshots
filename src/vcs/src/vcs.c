#ifndef	lint
static	char	Id[] = "$Id: vcs.c,v 4.1 1989/09/07 13:46:15 dickey Exp $";
#endif	lint

/*
 * Title:	vcs.c (version-control-system utility)
 * Author:	T.E.Dickey
 * Created:	06 Sep 1989
 *
 * Function:
 */

#define	STR_PTYPES
#include	"ptypes.h"
extern	int	optind;
extern	char	*vcs_file();

#define	WARN	FPRINTF(stderr,

static	int	i_or_d;		/* initialize/delete directories */
static	int	n_opt;		/* no-op mode (show w/o doing it) */
static	int	q_opt;		/* quiet-option, passed to exec'd programs */

char	*
relpath(dst, src)
char	*dst, *src;
{
	extern	char	*pathcat();
	auto	char	tmp[BUFSIZ];
	auto	char	cwd[BUFSIZ];
	auto	char	pre[BUFSIZ];
	auto	int	j,k;

	src = strcpy(tmp, src);
	if (getwd(cwd) != 0) {
		strcpy(pre, ".");
		for (;;) {
			printf("pre(%s) cwd(%s)\n", pre, cwd);
			if (	!strcmp(cwd,src))
				return (strcpy(dst, pre));
			if (	((j = strlen(cwd)) < (k = strlen(src)))
			&&	!strncmp(cwd,src,j)
			&&	src[j] == '/')
				return (pathcat(dst, pre, src+j+1));

			/*
			 * Trim off a leaf from current-directory, add a level
			 * of ".." to prefix:
			 */
			(void)strcat(pre, pre[1] ? "/.." : ".");
			if (j > 0 && cwd[--j] != '/') {
				while (cwd[j] != '/')
					cwd[j--] = 0;
				if (j > 0 && cwd[j-1] != '/')
					cwd[j] = 0;
			} else
				break;
		}
	}
	return (strcpy(dst, src));
}

/************************************************************************
 *	local entrypoints						*
 ************************************************************************/
static
dir_exists(name)
char	*name;
{
	auto	struct	stat	sb;
	return ((stat(name, &sb) >= 0)
	&&	((sb.st_mode & S_IFMT) == S_IFDIR));
}

static
vcs_delete(name)
char	*name;
{
	printf("delete: %s\n", name);
}

static
vcs_create(name, base)
char	*name, *base;
{
	char	tmp[BUFSIZ];
	printf("create: %s (%s)\n", name, base);
	printf("=====>: %s\n", relpath(tmp, name));
}

/*
 * Process a list of directory-names
 */
static
do_name(name, old_name, old_base, nested)
char	*name;		/* pathname to process */
char	*old_name;	/* => caller's original name-string */
char	*old_base;	/* => caller's 'base[]' string */
int	nested;		/* true iff we recurred to find a parent */
{
	auto	char	path[BUFSIZ];
	auto	char	p_name[BUFSIZ];
	auto	char	v_name[BUFSIZ];
	auto	char	base[80];

	abspath(name = strcpy(path, name));	/* prune out "/.." */
	printf(": %s\n", name);

	/*
	 * Determine if the vcs-file exists in the given directory.
	 */
	printf("> %s\n", vcs_file(name, v_name, TRUE));
	if (dir_exists(v_name)) {
		/*
		 * If we find an RCS-directory, we must have permissions in it
		 * to do insert/deletion of lower-level stuff.
		 */
		if (!rcspermit(v_name, base)) {
			WARN "?? no permission in %s\n", v_name);
			return (FALSE);
		}
		printf("->%s\n", base);

		/*
		 * If do have permissions on the directory, then the only thing
		 * we care about is if we are asked to delete it.  Don't delete
		 * it if we recurred to get here.
		 */
		if (i_or_d < 0 && !nested)
			vcs_delete(name);

	} else {	/* no RCS-directory, hence no VCS-file */
		/*
		 * If we cannot find the vcs-file at all, we eventually may
		 * find that we have come up to the file-system root.
		 */
		if (name[strlen(name)-1] == '/') {
			WARN "?? cannot find VCS-file from %s\n",
				old_name);
			return (FALSE);
		}

		/*
		 * If the vcs-file does not exist, find the vcs-file in the
		 * parent-directory so we know what baseline version to use,
		 * and whether we will have permissions.
		 */
		printf("look for parent\n");
		if (!do_name(strcat(strcpy(p_name, name), "/.."),
				old_name, base, nested+1))
			return (FALSE);	

		/*
		 * If the caller specified "-i", create the directory, no matter
		 * if we recurred to get here.
		 */
		if (i_or_d > 0)
			vcs_create(name, base);
	}
	(void)strcpy(old_base, base);	/* let caller know about RCS_BASE */
	return (TRUE);
}

static
do_args(name)
char	*name;
{
	auto	char	path[BUFSIZ];
	auto	char	base[80];

	abspath(strcpy(path,name));	/* ensure no trailing "/" */

	if (!do_name(path, name, base, 0)) {
		exit(FAIL);
	}
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/
usage()
{
	static	char	*msg[] = {
	"usage: vcs [options] [directory_names]",
	"",
	"options:",
	"  -i    initialize directories",
	"  -d    delete (empty) directories",
	"  -n    no-op mode",
	"  -q    quiet mode"
	};
	register int j;
	for (j = 0; j < sizeof(msg)/sizeof(msg[0]); j++)
		WARN "%s\n", msg[j]);
	exit(FAIL);
}

main(argc, argv)
char	*argv[];
{
	register int	c;

	while ((c = getopt(argc, argv, "idnq")) != EOF)
		switch (c) {
		case 'i':	i_or_d = 1;	break;
		case 'd':	i_or_d = -1;	break;
		case 'n':	n_opt = TRUE;	break;
		case 'q':	q_opt = TRUE;	break;
		default:	usage();
				/*NOTREACHED*/
		}
	if (optind < argc) {
		while (optind < argc)
			do_args(argv[optind++]);
	} else if (i_or_d < 0)
		do_args(".");
	else
		usage();
	exit(SUCCESS);
	/*NOTREACHED*/
}
