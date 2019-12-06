/*
 * Title:	vcs_unlock.c
 * Author:	T.E.Dickey
 * Created:	16 Oct 1991 (from 'vcs.c')
 * Modified:
 *		03 Dec 2019, use "executev()"
 *		22 Sep 1993, gcc warnings
 *
 * Function:	Unlocks an RCS archive-file for the user.  If the user happens
 *		to own the file, all locks are undone, otherwise only those
 *		locks that the user has made are undone.
 */

#include <vcs.h>

MODULE_ID("$Id: unlock.c,v 11.5 2019/12/04 01:30:47 tom Exp $")

/******************************************************************************/
static int
GetLock(char *name, char *lock_rev, char *lock_by)
{
    int header = TRUE;
    int code = S_FAIL;
    char *s = 0;
    char tip[BUFSIZ];
    char key[BUFSIZ];

    *lock_by =
	*lock_rev = EOS;

    if (!rcsopen(name, -debug, TRUE))
	return (FALSE);		/* could not open file anyway */

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
UnLockFile(char *name)
{
    char *Archive = name2rcs(name, x_opt);
    char tmp[BUFSIZ];
    char old_rev[80];
    char rev[80];
    char who[80];
    time_t oldtime, newtime;

    if (!Access(Archive, FALSE))
	return;
    if ((oldtime = DateOf(Archive)) == 0)
	failed(Archive);	/* someone lied to me */
    *old_rev = EOS;

    while (GetLock(Archive, rev, who)) {
	if (!strcmp(rev, old_rev))
	    break;

	RCS_argc = 1;
	FORMAT(tmp, "-u%s", rev);
	add_params(tmp);
	add_params(Archive);
	VERBOSE(".. locked by %s\n", who);

	invoke_command(RCS, rcspath(RCS));
	if (no_op)
	    break;

	if ((newtime = DateOf(Archive)) == oldtime)
	    break;
	oldtime = newtime;

	(void) strcpy(old_rev, rev);
    }
}
