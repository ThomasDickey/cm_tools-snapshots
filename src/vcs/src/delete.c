/*
 * Title:	vcs_delete.c (version-control-system utility)
 * Author:	T.E.Dickey
 * Created:	17 Oct 1991 (from 'vcs.c'
 * Modified:
 *		22 Sep 1993, gcc warnings
 *
 * Function:	Deletes a directory-tree from the archive if the following
 *		conditions are met:
 *
 *		a) no directory contains anything but other directories, or
 *		   RCS-directories.
 *		b) no RCS-directory contains anything but the vcs-file (RCS,v).
 *		c) the real user is on the access-list of each vcs-file.
 */

#include <vcs.h>

MODULE_ID("$Id: delete.c,v 11.5 2010/07/04 17:48:11 tom Exp $")

typedef struct _item {
    struct _item *link;
    char *name;
    int is_file;
} ITEM;

static ITEM *items;
static int can_do;

/******************************************************************************/
static void
RemoveFile(char *name)
{
    ShowPath("rm -f", name);
    if (!no_op && (unlink(name) < 0))
	failed(name);
}

/******************************************************************************/
static void
RemoveDir(char *name)
{
    ShowPath("rmdir", name);
    if (!no_op && (rmdir(name) < 0))
	failed(name);
}

/******************************************************************************/
static void
PurgeList(void)
{
    ITEM *p;

    if (!can_do && items != 0)
	VERBOSE(".. purging list (no deletion performed)\n");

    while ((p = items) != NULL) {
	if (can_do) {
	    if (p->is_file)
		RemoveFile(p->name);
	    else
		RemoveDir(p->name);
	}
	items = p->link;
	free((char *) p);
    }
}

/******************************************************************************/
static void
Append(char *path, int flag)
{
    ITEM *p = ALLOC(ITEM, 1);
    p->link = items;
    p->name = txtalloc(path);
    p->is_file = flag;
    items = p;
}

/******************************************************************************/
static void
Cannot(const char *path, const char *why)
{
    if (can_do || verbose) {
	can_do = FALSE;
	PRINTF("?? %s\nfile:%s\n", why, path);
    }
}

/******************************************************************************/
static void
UserCannot(const char *path, const char *who, const char *what)
{
    char temp[BUFSIZ];
    FORMAT(temp, "user \"%s\" %s", who, what);
    Cannot(path, temp);
}

/******************************************************************************/
/*ARGSUSED*/
static int
WALK_FUNC(do_tree)
{
    auto int mode;
    char part[MAXPATHLEN];
    char tmp[BUFSIZ];
    char *full = pathcat(tmp, path, name);

    (void) level;

    if (readable < 0 || sp == 0) {
	Cannot(full, "not readable");
    } else if ((mode = (sp->st_mode & S_IFMT)) == S_IFDIR) {
	Append(full, FALSE);
    } else if (mode == S_IFREG
	       && !strcmp(name, vcs_file((char *) 0, part, FALSE))) {
	if (!Access(full, FALSE))
	    Cannot(full, "no access");
	else if (geteuid() != 0 && RCS_uid != geteuid())
	    UserCannot(full, uid2s(RCS_uid), "not owner");
	else if (!rcspermit(path, part, (const char **) 0))
	    UserCannot(full, uid2s(getuid()), "not on access-list");
	else
	    Append(full, TRUE);
    } else
	Cannot(full, "unexpected type");
    return (can_do ? readable : -1);
}

/******************************************************************************/
void
DeleteDir(char *name)
{
    if (!(can_do = DirExists(name))) {
	VERBOSE(".. %s does not exist\n", name);
    } else
	(void) walktree((char *) 0, name, do_tree, "r", 0);
    PurgeList();
}
