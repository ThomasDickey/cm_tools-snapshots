#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/cm_tools.vcs/src/baseline/src/RCS/baseline.c,v 11.2 1994/11/08 23:44:46 tom Exp $";
#endif

/*
 * Title:	baseline.c (rcs baseline)
 * Author:	T.E.Dickey
 * Created:	24 Oct 1989
 * Modified:
 *		22 Sep 1993, gcc warnings
 *		17 Jan 1992, added "-L" option.  Renamed "-r" to "-R" option
 *			     and "-f" to "-p" for consistency.
 *		22 Oct 1991, use 'shoarg()'
 *		11 Oct 1991, converted to ANSI
 *		20 May 1991, apollo sr10.3 cpp complains about endif-tags
 *		16 Apr 1990, "-f" option must allow 'permit' to run, otherwise
 *			     the RCS,v file won't be updated.  Made "-f" only
 *			     suppress 'permit' "-p" option.
 *		03 Jan 1990, added "-l" option for stluka. added logic (for "-l"
 *			     option) so that we can allow a locked file to be
 *			     baselined (if it is locked, but not changed).
 *			     Corrected code which infers baseline revision (if
 *			     no rcs directory existed, this did not notice!)
 *		07 Nov 1989, added "-f" option, tuned some verboseness.
 *			     Corrected treatment of "-r" option (recur only
 *			     when asked).  Corrected code which infers current-
 *			     baseline version.
 *		01 Nov 1989, walktree passes null-pointer to stat-block if
 *			     no-access.
 *
 * Function:	Use checkin/checkout to force a tree of files to have a common
 *		baseline-version number.
 *
 *		This is a rewrite of the Bourne-shell script 'baseline.sh'.
 *
 * Options:	(see 'usage()')
 */

#define	STR_PTYPES
#include	<ptypes.h>
#include	<rcsdefs.h>
#include	<sccsdefs.h>
#include	<ctype.h>
#include	<time.h>

#define	isDIR(mode)	((mode & S_IFMT) == S_IFDIR)
#define	isFILE(mode)	((mode & S_IFMT) == S_IFREG)
#define	isLINK(mode)	((mode & S_IFMT) == S_IFLNK)
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
static	int	lock_up;	/* leave files locked */
static	char	*m_text;	/* message-text */
static	int	no_op;		/* no-op mode */
#ifdef	S_IFLNK
static	int	L_opt;
#endif
static	int	purge_opt;
static	int	recur;
static	int	verbose;

static
void
doit(
_ARX(char *,	verb)
_ARX(char *,	args)
_AR1(int,	really)
	)
_DCL(char *,	verb)
_DCL(char *,	args)
_DCL(int,	really)
{
	if (verbose)
		shoarg(stdout, verb, args);
	if (really) {
		if (execute(verb, args) < 0) {
			WARN "?? %s\n", verb);
			exit(FAIL);
		}
	}
}

static
void
quiet_opt(
_AR1(char *,	args))
_DCL(char *,	args)
{
	if (!verbose)	catarg(args, "-q");
}

static
void
purge_rights(
_AR1(char *,	path))
_DCL(char *,	path)
{
	auto	char	args[BUFSIZ];
	static	LIST	*purged;
	register LIST	*p;

	FORMAT(args, "-b%d ", revision);
	for (p = purged; p; p = p->link) {
		if (!strcmp(p->text, path))
			return;
	}
	p = ALLOC(LIST,1);
	p->link = purged;
	p->text = txtalloc(path);
	purged  = p;

	quiet_opt(args);
	if (no_op)	catarg(args, "-n");
	if (purge_opt)	catarg(args, "-p");
	catarg(args, m_option);
	catarg(args, rcs_dir());
	doit("permit", args, no_op < 2);
}

static
void
baseline(
_ARX(char *,	path)
_ARX(char *,	name)
_AR1(time_t,	edited)
	)
_DCL(char *,	path)
_DCL(char *,	name)
_DCL(time_t,	edited)
{
	auto	char	args[BUFSIZ];
	auto	char	*version,
			*locker;
	auto	time_t	date;
	auto	int	cmp;
	auto	int	pending;	/* true if lock implies change */

	purge_rights(path);
	rcslast (path, name, &version, &date, &locker);
	pending = (!lock_up) || (date != edited);

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
	} else if (*locker != '?' && pending) {
		WARN "?? %s (locked by %s)\n", name, locker);
		exit(FAIL);
		/*NOTREACHED*/
	}
	PRINTF("** %s\n", name);

	if (*locker == '?') {		/* wasn't locked, must do so now */
		*args = EOS;
		catarg(args, "-l");
		quiet_opt(args);
		catarg(args, name);
		doit("checkout", args, !no_op);
	}

	FORMAT(args, "-r%d.0 -f -u ", revision);
	quiet_opt(args);
	if (lock_up)	catarg(args, "-l");
	catarg(args, m_option);
	catarg(args, "-sRel");
	catarg(args, name);
	doit("checkin", args, !no_op);
}

static
int	WALK_FUNC(scan_tree)
{
	auto	char	tmp[BUFSIZ],
			*s = pathcat(tmp, path, name);

	if (sp == 0 || level > recur)
		readable = -1;
	else if (isDIR(sp->st_mode)) {
		abspath(s);		/* get rid of "." and ".." names */
		if (!a_opt && *pathleaf(s) == '.')
			readable = -1;
		else if (sameleaf(s, sccs_dir(path, name))
		    ||   sameleaf(s, rcs_dir())) {
			readable = -1;
		} else {
			if (level > recur) {
				PRINTF("** %s (ignored)\n", name);
				readable = -1;
			} else {
#ifdef	S_IFLNK
				if (!L_opt) {
					struct	stat	sb;
					if (lstat(name, &sb) < 0
					 || isLINK(sb.st_mode)) {
						PRINTF("** %s (link)\n", name);
						readable = -1;
					}
				} else
#endif
				track_wd(s);
			}
		}
	} else if (isFILE(sp->st_mode)) {
		if (level > 0)
			track_wd(path);
		baseline(path,name, sp->st_mtime);
	} else
		readable = -1;

	return(readable);
}

static
void
do_arg(
_AR1(char *,	name))
_DCL(char *,	name)
{
	auto	int	infer_rev = FALSE;

	FORMAT(m_option, "-mBASELINE %s", ctime(&began));
	m_option[strlen(m_option)-1] = EOS;
	if (m_text)
		(void)strcat(strcat(m_option, " -- "), m_text);

	/*
	 * If no revision was given, infer it, assuming that we are going to
	 * add forgotten stuff to the current baseline.
	 */
	if (revision < 0) {
		auto	char	vname[BUFSIZ];
		auto	time_t	date;
		auto	char	*locker, *version;
		rcslast (".",
			vcs_file(rcs_dir(), vname, FALSE),
			&version, &date, &locker);
		if (*version == '?'
		||  sscanf(version, "%d.", &revision) <= 0)
			revision = 2;
		infer_rev = TRUE;
	}
	FORMAT(r_option, "%d.0", revision);

	(void)walktree((char *)0, name, scan_tree, "r", 0);

	if (infer_rev)
		revision = -1;
}

static
void
usage(_AR0)
{
	static	char	*tbl[] = {
 "Usage: baseline [options] [files]"
,""
,"Bumps versions of rcs files to the specified integer value (i.e., 2.0)."
,"If no files are specified, all files in the current directory are assumed."
,""
,"Options are:"
,"  -{integer}  baseline version (must be higher than last baseline)"
,"  -a          process directories beginning with \".\""
,"  -l          leave files locked after baselining"
,"  -m TEXT     append reason for baseline to date-message"
#ifdef	S_IFLNK
,"  -L          traverse symbolic links to directories"
#endif
,"  -n          no-op (show what would be done, but don't do it)"
,"  -p          run \"permit\" first to purge access lists"
,"  -R          recur when lower-level directories are found"
,"  -v          verbose (shows details)"
			};
	register int	j;
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		WARN "%s\n", tbl[j]);
	FFLUSH(stderr);
	exit(FAIL);
}

/*ARGSUSED*/
_MAIN
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
					case 'l':	lock_up++;	break;
					case 'n':	no_op++;	break;
#ifdef	S_IFLNK
					case 'L':	L_opt++;	break;
#endif
					case 'p':	purge_opt++;	break;
					case 'R':	recur = 999;	break;
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
		recur++;
		do_arg(".");
	}
	exit(SUCCESS);
	/*NOTREACHED*/
}
