#ifndef	lint
static	char	Id[] = "$Id: baseline.c,v 5.0 1989/10/26 12:06:05 ste_cm Rel $";
#endif	lint

/*
 * Title:	baseline.c (rcs baseline)
 * Author:	T.E.Dickey
 * Created:	24 Oct 1989
 * Function:	Use checkin/checkout to force a tree of files to have a common
 *		baseline-version number.
 *
 *		This is a rewrite of the Bourne-shell script 'baseline.sh'.
 *
 * Options:	(see 'usage()')
 */

#define	STR_PTYPES
#include	"ptypes.h"
#include	"rcsdefs.h"
#include	<ctype.h>
#include	<time.h>
extern	time_t	time();
extern	long	strtol();
extern	char	*pathcat();
extern	char	*pathleaf();
extern	char	*txtalloc();

#define	isDIR(mode)	((mode & S_IFMT) == S_IFDIR)
#define	isFILE(mode)	((mode & S_IFMT) == S_IFREG)
#define	WARN		FPRINTF(stderr,

typedef	struct	_list	{
	struct	_list	*link;
	char		*text;
	} LIST;

	/* ARGSUSED */
	def_ALLOC(LIST)

static	time_t	began;
static	int	revision = -1;
static	char	m_option[BUFSIZ];	/* message to show as "-m" */
static	char	r_option[80];

				/* options */
static	int	a_opt;		/* all-directory scan */
static	char	*m_text;	/* message-text */
static	int	no_op;		/* no-op mode */
static	int	recur;
static	int	verbose;

static
doit(verb, args, really)
char	*verb, *args;
{
	if (verbose)
		PRINTF("%% %s %s\n", verb, args);
	if (really) {
		if (execute(verb, args) < 0) {
			WARN "?? %s\n", verb);
			exit(FAIL);
		}
	}
}

static
quiet_opt(args)
char	*args;
{
	if (!verbose)	catarg(args, "-q");
}

static
purge_rights(path)
char	*path;
{
	auto	char	args[BUFSIZ];
	static	LIST	*purged;
	register LIST	*p;

	for (p = purged; p; p = p->link) {
		if (!strcmp(p->text, path))
			return;
	}
	p = ALLOC(LIST,1);
	p->link = purged;
	p->text = txtalloc(path);
	purged  = p;

	FORMAT(args, "-b%d -p ", revision);
	quiet_opt(args);
	if (no_op)	catarg(args, "-n");
	catarg(args, m_option);
	catarg(args, rcs_dir());
	doit("permit", args, no_op < 2);
}

static
baseline(path, name)
char	*path;
char	*name;
{
	auto	char	args[BUFSIZ];
	auto	char	*version,
			*locker;
	auto	time_t	date;
	auto	int	cmp;

	purge_rights(path);
	rcslast (path, name, &version, &date, &locker);

	if (*version == '?') {
		if (istextfile(name)) {
			WARN "?? %s (not archived)\n", name);
			if (!recur)
				exit(FAIL);
		} else
			PRINTF("** %s (ignored)\n", name);
		return;
	} else if ((cmp = vercmp(r_option, version, FALSE)) < 0) {
		WARN "?? %s (version mismatch -b%d vs %s)\n",
			name, revision, version);
		exit(FAIL);
		/*NOTREACHED*/
	} else if (cmp == 0) {
		PRINTF("** %s (already baselined)\n", name);
		return;
	} else if (*locker != '?') {
		WARN "?? %s (locked by %s)\n", name, locker);
		exit(FAIL);
		/*NOTREACHED*/
	}
	PRINTF("** %s\n", name);

	*args = EOS;
	catarg(args, "-l");
	quiet_opt(args);
	catarg(args, name);
	doit("checkout", args, !no_op);

	FORMAT(args, "-r%d.0 -f -u ", revision);
	quiet_opt(args);
	catarg(args, m_option);
	catarg(args, "-sRel");
	catarg(args, name);
	doit("checkin", args, !no_op);
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

	if (isDIR(sp->st_mode)) {
		abspath(s);		/* get rid of "." and ".." names */
		if (!a_opt) {
			t = pathleaf(s);
			if (*t == '.')
				return (-1);
		}
		if (sameleaf(s, sccs_dir())
		||  sameleaf(s, rcs_dir())) {
			return (-1);
		}
		if (!recur && level > 0) {
			PRINTF("** %s (ignored)\n", name);
			return (-1);
		}
		track_wd(path);
	} else if (isFILE(sp->st_mode)) {
		track_wd(path);
		baseline(path,name);
	} else
		return (-1);
	return(ok_acc);
}

static
do_arg(name)
char	*name;
{
	auto	int	infer_rev = FALSE;

	FORMAT(m_option, "-mBASELINE %s", ctime(&began));
	m_option[strlen(m_option)-1] = EOS;
	if (m_text)
		(void)strcat(strcat(m_option, " -- "), m_text);

	/*
	 * If no revision was given, infer it, assuming that we are only
	 * going to bump up by one level.
	 */
	if (revision < 0) {
		auto	time_t	date;
		auto	char	*locker, *version;
		rcslast (".", rcs_dir(), &version, &date, &locker);
		if (*version != '?'
		&&  sscanf(version, "%d.", &revision) > 0)
			infer_rev = TRUE;
	}
	FORMAT(r_option, "%d.0", revision);

	(void)walktree((char *)0, name, scan_tree, "r", 0);

	if (infer_rev)
		revision = -1;
}

usage()
{
	static	char	*tbl[] = {
 "Usage: baseline [options] files"
,""
,"Options are:"
,"  -{integer}  baseline version (must be higher than last baseline)"
,"  -m{text}    append reason for baseline to date-message"
,"  -n          no-op (show what would be done, but don't do it)"
,"  -r          recur when lower-level directories are found"
,"  -v          verbose (shows details)"
			};
	register int	j;
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		WARN "%s\n", tbl[j]);
	(void)fflush(stderr);
	exit(FAIL);
}

main(argc, argv)
char	*argv[];
{
	register int	j;
	register char	*s;
	auto	 char	*d;
	auto	 int	had_args = FALSE;

	track_wd((char *)0);
	began = time((time_t *)0);

	for (j = 1; j < argc; j++) {
		if (*(s = argv[j]) == '-') {
			while (*(++s)) {
				if (isdigit(*s)) {
					if ((revision = strtol(s, &d, 10)) < 2)
						usage();
					s = d - 1;
				} else if (*s == 'm') {
					if (*(++s)) {
						m_text = s;
						break;
					} else
						usage();
				} else {
					switch (*s) {
					case 'a':	a_opt++;	break;
					case 'n':	no_op++;	break;
					case 'r':	recur++;	break;
					case 'v':	verbose++;	break;
					default:	usage();
					}
				}
			}
		} else {
			do_arg(s);
			had_args = TRUE;
		}
	}

	if (!had_args) {
		do_arg(".");
	}
	exit(SUCCESS);
	/*NOTREACHED*/
}
