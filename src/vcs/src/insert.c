#ifndef	lint
static	char	Id[] = "$Id: insert.c,v 11.1 1992/11/12 13:20:35 dickey Exp $";
#endif

/*
 * Title:	vcs_insert.c (version-control-system utility)
 * Author:	T.E.Dickey
 * Created:	17 Oct 1991, broke out from 'vcs.c'
 * Modified:
 *
 * Function:	performs insert-directory function for 'vcs'.
 */

#include "vcs.h"

/******************************************************************************/
/* we have to change directories to keep fooling rcs about the vcs-file */
static
ChangeWd(
_AR1(char *,	name))
_DCL(char *,	name)
{
	ShowPath("chdir", name);
	if (!no_op) {
		if (chdir(name) < 0)
			failed(name);
	}
}

/******************************************************************************/
static
MakeDirectory(
_AR1(char *,	name))
_DCL(char *,	name)
{
	int	old = umask(0);

	ShowPath("mkdir", name);
	if (!no_op) {
		if (mkdir(name, RCS_prot) < 0)
			failed(name);
	}
	(void)umask(old);
}

/******************************************************************************/
static
MakePermit(
_ARX(char *,	dst)
_AR1(char *,	base)
	)
_DCL(char *,	dst)
_DCL(char *,	base)
{
	static	char	*prefix = "../..";
	char	src[MAXPATHLEN],
		buffer[BUFSIZ];

	set_command();
	set_option('b', base);
	catarg(RCS_cmd, dst);
	invoke_command(PERMIT, PERMIT);

	ChangeWd(dst);

	(void)pathcat(src, prefix, rcs_dir());

	set_command();
	set_option('A', vcs_file(src, buffer, FALSE));
	catarg(RCS_cmd, vcs_file(".", buffer, FALSE));
	invoke_command(RCS, rcspath(RCS));

	ChangeWd(prefix);
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/
InsertDir(
_ARX(char *,	name)
_AR1(char *,	base)
	)
_DCL(char *,	name)
_DCL(char *,	base)
{
	char		*Name = name,
			*s,
			temp	[BUFSIZ],
			head	[MAXPATHLEN],
			ref_path[MAXPATHLEN];
	struct	stat	sb;
	register int	len;

	if (DirExists(name)) {
		VERBOSE(".. %s already exists\n", name);
		return FALSE;
	}

	/*
	 * See how much of the path exists already.  The 'pathhead()' function
	 * will return the immediate-parent directory iff it exists; otherwise
	 * it will iterate up til it finds an existing directory.
	 */
	(void)strcpy(head, pathhead(name, &sb));

	VERBOSE(".. name=%s\n", name);
	VERBOSE(".. head=%s\n", head);

	for (len = 0; (head[len] == name[len]) && (head[len] != EOS); len++);
	if (len > 0)
		Name += len + 1;

	if (s = strchr(Name, '/')) {	/* immediate-parent not found */
		*s = EOS;
		VERBOSE(".. recur:%s\n", name);
		if (!InsertDir(name, base))
			return FALSE;
		VERBOSE(".. done: %s\n", name);
		*s = '/';
		strcpy(head, name)[s-name] = EOS;
	} else {
		ChangeWd(head);
		(void)strcpy(head, ".");/* avoid re-use in 'ref_path' */
	}

	/* see if the head contains an RCS directory in which the real user has
	 * permissions
	 */
	(void)pathcat(ref_path, head, rcs_dir());
	/* patch (void)pathcat(ref_path, "..", ref_path); */
	if (!Access(vcs_file(ref_path, temp, FALSE), no_op))
		return FALSE;

	if (no_op && !DirExists(ref_path))
		;	/* assume we could if we wanted to */
	else if (!rcspermit(ref_path, base, (char **)0)) {
		WARN "? no permission on %s\n", ref_path);
		return FALSE;
	} else if (s = strchr(base, '.')) {
		*s = EOS;	/* trim version to baseline (3.1 => 3) */
	}

	MakeDirectory(Name);
	MakeDirectory(pathcat(temp, Name, rcs_dir()));
	MakePermit(temp, base);

	return TRUE;
}
