#ifndef	lint
static	char	Id[] = "$Id: unlock.c,v 11.1 1992/10/28 07:48:17 dickey Exp $";
#endif

/*
 * Title:	vcs_unlock.c
 * Author:	T.E.Dickey
 * Created:	16 Oct 1991 (from 'vcs.c')
 * Modified:
 *
 * Function:	Unlocks an RCS archive-file for the user.  If the user happens
 *		to own the file, all locks are undone, otherwise only those
 *		locks that the user has made are undone.
 */

#include "vcs.h"

/******************************************************************************/
static
GetLock(
_ARX(char *,	name)
_ARX(char *,	lock_rev)
_AR1(char *,	lock_by)
	)
_DCL(char *,	name)
_DCL(char *,	lock_rev)
_DCL(char *,	lock_by)
{
	int	header	= TRUE,
		code	= S_FAIL;
	char	*s	= 0,
		tip[BUFSIZ],
		key[BUFSIZ];

	*lock_by =
	*lock_rev = EOS;

	if (!rcsopen(name, -debug, TRUE))
		return (FALSE);	/* could not open file anyway */

	while (header && (s = rcsread(s, code))) {
		s = rcsparse_id(key, s);

		switch (code = rcskeys(key)) {
		case S_HEAD:
			s = rcsparse_num(tip, s);
			if (!*tip)
				header = FALSE;
			break;

		case S_LOCKS:
			s = rcslocks(s, lock_by, lock_rev);
			/* fall-thru */

		case S_VERS:
			header = FALSE;
			break;
		}
	}
	rcsclose();
	return (*lock_rev != EOS);
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/
void
UnLockFile(
_AR1(char *,	name))
_DCL(char *,	name)
{
	char	*Archive = name2rcs(name, x_opt);
	char	tmp[BUFSIZ],
		old_rev[80],
		rev[80],
		who[80];
	time_t	oldtime,
		newtime;

	if (!Access(Archive, FALSE))
		return;
	if ((oldtime = DateOf(Archive)) == 0)
		failed(Archive);	/* someone lied to me */
	*old_rev = EOS;

	while (GetLock(Archive, rev, who)) {
		if (!strcmp(rev, old_rev))
			break;

		*RCS_cmd = EOS;
		FORMAT(tmp, "-u%s", rev);
		catarg(RCS_cmd, tmp);
		catarg(RCS_cmd, Archive);
		VERBOSE(".. locked by %s\n", who);

		RCS_verb = RCS;
		RCS_path = rcspath(RCS);
		invoke_command(RCS, rcspath(RCS));
		if (no_op)
			break;

		if ((newtime = DateOf(Archive)) == oldtime)
			break;
		oldtime = newtime;

		(void)strcpy(old_rev, rev);
	}
}
