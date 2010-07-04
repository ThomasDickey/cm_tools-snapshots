/*
 * Title:	permit.c (RCS directory-permissions)
 * Author:	T.E.Dickey
 * Created:	09 Mar 1989
 * Modified:
 *		10 May 1997, correct argument to mktemp() for descriptions.
 *		04 Dec 1992, added "-l" option, to allow links to be shown
 *		23 Nov 1992, allow user-names to be given that are not in the
 *			     passwd-file.  Fixed an error with "-a" option,
 *			     when owner was already present, caused option to
 *			     be ignored.
 *		17 Feb 1992, lint
 *		30 Oct 1991, allow "-b" value to be "1" (for initial-creation)
 *		15 Oct 1991, convert to ANSI.  Use 'shoarg()'
 *		06 Sep 1991, changed interface to 'rcsopen()'
 *		31 May 1991, lint (unused arg of 'do_arcs()')
 *		20 May 1991, mods to compile on apollo sr10.3
 *		25 Jul 1989, L_cuserid is not defined in apollo SR10 (bsd4.3)
 *		16 Jun 1989, corrected copy to 'm_buffer[]'; removed redundant
 *			     'failed()'.
 *		13 Jun 1989, added -m option to allow override of BASELINE rlog-
 *			     message.  Restructured usage-code to make it
 *			     faster.
 *		13 Mar 1989, cleanup of code which handles multiple usernames.
 *
 * Function:	This program maintains a file in each RCS directory named
 *		"RCS,v" which records the history of each baseline which has
 *		been applied to the directory, as well as the names of users
 *		who are currently allowed to place locks on the files.
 *
 *		The "RCS,v" permit-file is a normal RCS file; however, the
 *		'checkin' and 'checkout' utilities cannot modify it since they
 *		use a fixed naming convention for RCS-archives.  Since it is a
 *		normal RCS file, it can be viewed with 'rlog' and maintained
 *		(as in this program) using the 'rcs' and 'ci' utilities.
 *
 *		The purpose in making the permit-file inaccessible to 'checkin'
 *		and 'checkout' is to provide a measure of security for CM-usage.
 *
 * Options:	see 'usage()'
 */

#define		STR_PTYPES
#include	<ptypes.h>
#include	<rcsdefs.h>
#include	<ctype.h>
#include	<time.h>

MODULE_ID("$Id: permit.c,v 11.11 2010/07/04 17:21:37 tom Exp $")

/************************************************************************
 *	local definitions						*
 ************************************************************************/

#define	CI_TOOL		"ci"
#define	CO_TOOL		"co"
#define	RCS_TOOL	"rcs"

#define	if_TELL	if(verbose >= 0)
#define	TELL	if_TELL PRINTF
#define	VERBOSE	if(verbose >  0) PRINTF

static int add_opt;		/* "-a" option */
static int base_opt;		/* "-b" option */
static int expunge_opt;		/* "-e" option */
static int link_opt;		/* "-l" option */
static int purge_opt;		/* "-p" option */
static int null_opt;		/* "-n" option */
static int report_opt;		/* "-u" option */
static int modify;		/* any modification requested */
static char user_name[BUFSIZ];	/* user to add/expunge/report */
static int verbose;		/* "-v" option */
static int lines;		/* line-number, for report */
static char high_ver[20];
static char base_ver[20];
static char m_buffer[BUFSIZ];	/* baseline-message */

/* patch: should break comma-list routines out into library code */
/*
 * Determine if any option (-a, -e or -u) selected a user-name which corresponds
 * to the given argument.
 */
static int
on_list(char *list, char *s)
{
    int ok = FALSE;
    if (list != 0) {
	char bfr[BUFSIZ];
	char *d;

	for (d = strtok(strcpy(bfr, list), ",");
	     d; d = strtok((char *) 0, ",")) {
	    if (!strcmp(s, d))
		ok = TRUE;
	    /* continue, to put back delimiter */
	}
    }
    return ok;
}

/*
 * Append to a comma-separated access-list
 */
static void
cat_list(char *dst, char *src)
{
    if (*src) {
	if (*dst)
	    (void) strcat(dst, ",");
	(void) strcat(dst, src);
    }
}

/*
 * Remove a specific key from a comma-separated access list, so we can keep
 * track of the ones we have found.
 */
static void
del_list(char *list, char *key)
{
    size_t len = strlen(key);
    char *next;
    char *base = list;

    while (*list) {
	if ((next = strchr(list, ',')) != NULL) {
	    if ((next++ - list) == (int) len)
		if (!strncmp(list, key, len)) {
		    char *save = list;
		    while ((*list++ = *next++) != EOS) ;
		    next = save;
		}
	} else {
	    next = list + strlen(list);
	    if (!strcmp(list, key)) {
		if (list > base)
		    --list;
		*list = EOS;
	    }
	}
	list = next;
    }
}

/*
 * Indent the report for the given number of directory-levels.
 */
static void
indent(int level)
{
    ++lines;
    if (verbose >= 0) {
	PRINTF("%4d:\t", lines);
	while (level-- > 0)
	    PRINTF("|--%c", (level > 0) ? '-' : ' ');
    }
}

/*
 * Compute the "-r" option needed for the permit-file so that it will be
 * updated properly.
 */
static void
set_revision(char *dst, const char *opt)
{
    char *d;
    char bfr[20];

    if (*base_ver) {
	if (base_opt) {
	    if (strtol(base_ver, &d, 10) > base_opt) {
		FPRINTF(stderr, "?? -b%d inconsistent with version %s\n",
			base_opt, base_ver);
		exit(FAIL);
	    }
	    FORMAT(base_ver, "%d.1", base_opt);
	}
	catarg(dst, strcat(strcpy(bfr, opt), base_ver));
    } else if (base_opt) {
	FORMAT(bfr, "%s%d.1", opt, base_opt);
	catarg(dst, bfr);
    }
}

/*
 * Initialize an RCS-command string
 */
static void
set_command(char *dst)
{
    *dst = EOS;
    if (verbose < 0)
	catarg(dst, "-q");
}

/*
 * The permit-file has no tip-version.  Force it to have one.
 */
static void
set_baseline(void)
{
    char tmp[BUFSIZ];
    char acc_file[BUFSIZ];
    static char msg[] = "setting baseline version in permit-file";

    (void) vcs_file("./", acc_file, FALSE);

    set_command(tmp);
    catarg(tmp, "-l");
    catarg(tmp, acc_file);
    if_TELL shoarg(stdout, CO_TOOL, tmp);
    if (!null_opt && (execute(rcspath(CO_TOOL), tmp) < 0))
	failed(msg);

    set_command(tmp);
    set_revision(tmp, "-f");
    catarg(tmp, m_buffer);
    catarg(tmp, acc_file);
    if_TELL shoarg(stdout, CI_TOOL, tmp);
    if (!null_opt && (execute(rcspath(CI_TOOL), tmp) < 0))
	failed(msg);
}

/*
 * Create a permit-file.  Subsequent use of 'baseline' will update the change
 * history of this file.
 */
static void
create_permit(char *s)
{
    char tmp[BUFSIZ];
    char bfr[BUFSIZ];
    char owner[BUFSIZ];
    FILE *fp;
    Stat_t sb;
    char acc_file[BUFSIZ];
    char tmp_file[BUFSIZ];
    char TMP_DESC[BUFSIZ];
    char *tmp_desc;

    /* create filenames */
    (void) vcs_file("./", acc_file, FALSE);
    (void) vcs_file("./", tmp_file, TRUE);

    /* find the owner of the directory to use in access-list */
    if (stat("./", &sb) < 0) {
	perror("./");
	return;
    }

    /* if we are expunging ourselves, no sense in making permit-file */
    (void) strcpy(owner, uid2s(sb.st_uid));
    if (expunge_opt && on_list(user_name, owner))
	return;

    if (!null_opt) {
	(void) unlink(tmp_file);
	if (!(fp = fopen(tmp_file, "w")))
	    failed("opening temporary file");
#define	KEY(name)	"$%s$\n", "name"	/* prevent rcs-substitution! */
	FPRINTF(fp, KEY(Header));
	FPRINTF(fp, KEY(Log));
	FCLOSE(fp);
    }

    tmp_desc = mktemp(strcpy(TMP_DESC, "/tmp/permitXXXXXX"));
    if (tmp_desc != 0 && (fp = fopen(tmp_desc, "w"))) {
	FPRINTF(fp, "directory-level permissions for:\n%s\n", s);
	FCLOSE(fp);
    } else
	failed("creating description");

    set_command(tmp);
    catarg(tmp, "-mPERMIT FILE");
    catarg(tmp, strcat(strcpy(bfr, "-t"), tmp_desc));
    set_revision(tmp, "-r");
    catarg(tmp, tmp_file);
    catarg(tmp, acc_file);
    if_TELL shoarg(stdout, CI_TOOL, tmp);
    if (!null_opt && (execute(rcspath(CI_TOOL), tmp) < 0))
	failed("creating permit-file");
    (void) unlink(tmp_file);
    (void) unlink(tmp_desc);

    if (!purge_opt && !expunge_opt) {
	char temp_user[BUFSIZ];
	del_list(strcpy(temp_user, user_name), owner);
	set_command(tmp);
	(void) strcat(strcpy(bfr, "-a"), owner);
	if (add_opt)
	    cat_list(bfr, temp_user);
	catarg(tmp, bfr);
	catarg(tmp, acc_file);
	if_TELL shoarg(stdout, RCS_TOOL, tmp);
	if (!null_opt && (execute(rcspath(RCS_TOOL), tmp) < 0))
	    failed("modifying permit-file");
    }
}

/*
 * Derive the value of $RCS_BASE to store in the permit-file from the highest
 * known value of tip-version from the RCS archive files.
 */
static void
compute_base(void)
{
    char *s;

    if ((s = strchr(high_ver, '.')) == 0)
	s = high_ver + strlen(high_ver);
    (void) strcpy(s, ".1");
    (void) strcpy(base_ver, high_ver);
    VERBOSE("** new versions begin at %s\n", base_ver);
}

/*
 * Use the 'rcs' utility to add/expunge user(s) from the given archive's access
 * list.
 */
static void
modify_access(const char *file, const char *name, const char *opt)
{
    char cmd[BUFSIZ], tmp[BUFSIZ];

    if (*name) {
	set_command(cmd);
	catarg(cmd, strcat(strcpy(tmp, opt), name));
	catarg(cmd, file);
	if_TELL shoarg(stdout, RCS_TOOL, cmd);
	if (!null_opt && (execute(rcspath(RCS_TOOL), cmd) < 0))
	    failed("adding to access list");
    }
}
#define	add_user(file,name)	modify_access(file,name,"-a")
#define	expunge_user(file,name)	modify_access(file,name,"-e")

/*
 * This procedure is invoked to scan an RCS archive file.  Latch the highest
 * revision level found (to infer the baseline).  If locks are found, then
 * we may expunge them.
 */
/*ARGSUSED*/
static int
WALK_FUNC(do_arcs)
{
#ifdef	PATCH
    int got_lock = FALSE;	/* true if lock found */
#endif
    int got_owner = FALSE;	/* true if owner is on access list */
    mode_t mode = (sp != 0) ? (sp->st_mode & S_IFMT) : 0;
    int header = TRUE;
    int code = S_FAIL;
    int num = (int) (strlen(name) - (sizeof(RCS_SUFFIX) - 1));
    char *s = 0;
    char list[BUFSIZ];		/* users to add/expunge */
    char to_find[BUFSIZ];	/* check-off list of users */
    char found[BUFSIZ];		/* users found (for report) */
    char owner[BUFSIZ];
    char tip[80];		/* tip (top) version number */
    char key[80];		/* current keyword */
    char tmp[BUFSIZ];

    (void) path;

    if (mode != S_IFREG)	/* please, no subdirectories of RCS! */
	return (readable);

    if ((num < 0)
	|| strcmp(name + num, RCS_SUFFIX))
	return (readable);

    (void) strcpy(owner, uid2s(sp->st_uid));
    (void) strcpy(to_find, user_name);
    *list = EOS;
    *found = EOS;
    *tip = EOS;

    if (!rcsopen(name, verbose > 1, TRUE))
	return (FALSE);		/* could not open file anyway */

    while (header && (s = rcsread(s, code))) {
	s = rcsparse_id(key, s);

	switch (code = rcskeys(key)) {
	case S_HEAD:
	    s = rcsparse_num(tip, s);
	    num = dotcmp(tip, high_ver);
	    if (num > 0)
		(void) strcpy(high_ver, tip);
	    break;
	case S_ACCESS:
	    do {
		s = rcsparse_id(tmp, s);
		if (!*tmp)
		    break;
		if (!report_opt || on_list(to_find, tmp))
		    cat_list(found, tmp);
		if (purge_opt)
		    cat_list(list, tmp);
		else if (on_list(to_find, tmp)) {
		    if (!add_opt)
			cat_list(list, tmp);
		    del_list(to_find, tmp);
		}
		if (!strcmp(tmp, owner))
		    got_owner = TRUE;
	    } while (*tmp);
	    break;
	case S_LOCKS:
	    *tmp = EOS;
	    s = rcslocks(s, strcpy(key, user_name), tmp);
#ifdef	PATCH
	    if (*tmp)
		got_lock = TRUE;
#endif
	    /* patch: am not handling multiple users for lock */
	    /* patch: not sure what to do about locks yet */
	    break;
	case S_VERS:
	    header = FALSE;
	    break;
	}
    }
    rcsclose();
    indent(level);
    TELL("%s > %s (%s)\n", tip, name, found);

    /*
     * Now, depending on what we want to do with the file, process it.
     */
    if (add_opt)
	cat_list(list, to_find);
    /*
     * Handle the special case of automatically inserting the file-owner
     * into the access list -- if we are not purging the access lists, and
     * if the owner is not specifically mentioned in the user-list.
     */
    if (!got_owner && !purge_opt)
	if (!on_list(to_find, owner)) {
	    if (add_opt)
		cat_list(list, owner);
	}

    if (purge_opt || expunge_opt)
	expunge_user(name, list);
    else if (add_opt)
	add_user(name, list);

    return (readable);
}

/*
 * Scan through all RCS archives in the given directory, adding/expunging
 * the specified user from the access lists, and incidentally computing the
 * highest version, from which the most recent baseline can be inferred.
 */
static void
scan_arcs(const char *path, const char *name, int level)
{
    *high_ver = EOS;
    (void) walktree(path, name, do_arcs, "r", level);
    VERBOSE("** high version = '%s'\n", *high_ver ? high_ver : "1?");
    if (!*high_ver)
	(void) strcpy(high_ver, "1.1");
    compute_base();
}

/*
 * Test for the existence of the permit-file in the given directory.  If it is
 * found, assume that the baseline information in it is up-to-date.  We then
 * must check for the proper permissions, etc.
 *
 * As a byproduct of invoking 'do_arcs()', we will ensure that the owner of
 * the RCS directory is on the access list of the permit-file.
 */
static int
do_base(const char *path, const char *parent, int level, int *flag_)
{
    char nextpath[BUFSIZ];
    char acc_file[BUFSIZ];
    Stat_t sb;

    (void) vcs_file((char *) 0, acc_file, FALSE);
    if ((stat(pathcat(nextpath, path, acc_file), &sb) >= 0)
	&& (sb.st_mode & S_IFMT) == S_IFREG) {
	if (chdir(path) < 0)
	    failed(path);
	*high_ver = EOS;
	(void) do_arcs(path, acc_file, &sb, 0, level + 1);
	(void) chdir(parent);
	*flag_ = TRUE;
	if (*high_ver) {
	    compute_base();
	    return (TRUE);
	}
    }
    return (FALSE);
}

/*
 * This procedure is invoked from 'walktree()' for each file/directory which
 * is found in the specified tree.  We are interested only in walking the
 * directory-tree, and looking for RCS-directories at each point, to process.
 */
static int
WALK_FUNC(do_tree)
{
    const char *notes;
    mode_t mode = (sp != 0) ? (sp->st_mode & S_IFMT) : 0;

    if (mode == S_IFDIR) {
	char tmp[BUFSIZ];
	char bfr[80];
	char *s = pathcat(tmp, path, name);
	int flag = FALSE;

	abspath(s);		/* get rid of "." and ".." names */
	if (sameleaf(s, rcs_dir(NULL, NULL))) {
	    indent(level);
	    TELL("%s/\n", name);

	    *base_ver = EOS;
	    if (modify)
		scan_arcs(path, name, level);

	    if (!do_base(s, path, level, &flag)) {
		if (!modify)
		    scan_arcs(path, name, level);
		if (report_opt)
		    return (-1);

		if (chdir(s) < 0)
		    failed(s);
		if (flag)
		    set_baseline();
		else
		    create_permit(s);
		(void) chdir(path);
	    } else if (base_opt) {
		FORMAT(bfr, "%d.1", base_opt);
		if (strcmp(base_ver, bfr)) {
		    if (chdir(s) < 0)
			failed(s);
		    set_baseline();
		    (void) chdir(path);
		}
	    }
	    return (-1);	/* don't let walktree try this alone! */
	}
    }

    if (readable < 0 || sp == 0) {
	VERBOSE("?? %s/%s\n", path, name);
    } else if (mode == S_IFDIR) {
	notes = (name[strlen(name) - 1] == '/') ? "" : "/";
#ifdef	S_IFLNK
	if ((lstat(name, sp) >= 0)
	    && (sp->st_mode & S_IFMT) == S_IFLNK) {
	    notes = " (link)";
	    readable = -1;
	    if (!link_opt)
		return readable;
	}
#endif
	indent(level);
	TELL("%s%s\n", name, notes);
    }
    return (readable);
}

/*
 * Process a single argument: a directory name.
 */
static void
do_arg(const char *name)
{
    TELL("** path = %s\n", name);
    lines = 0;
    (void) walktree((char *) 0, name, do_tree, "r", 0);
}

static void
usage(void)
{
    char buffer[BUFSIZ];
    static const char *tbl[] =
    {
	"usage: permit [-{a|e|u}USER] [-bBASE] [-p] [-nqsv] [directory [...]]",
	"",
	"This maintains a RCS-directory permissions file, as well as",
	"the access lists for archive files.",
	"",
	"options:",
	"  -aUSER  add specified user(s) to access list",
	"  -bBASE  set baseline value for directory (must be > 0)",
	"  -eUSER  expunge specified user(s) from access list",
#ifdef	S_IFLNK
	"  -l      show symbolic link-names, otherwise ignored",
#endif
	"  -mTEXT  specifies baseline-message (default: BASELINE {date}",
	"  -n      no-op mode (shows actions that would be done)",
	"  -p      purge all users from access list",
	"  -q      quiet (less verbose)",
	"  -uUSER  report specified user(s) in access list",
	"  -v      verbose",
    };
    unsigned j;
    setbuf(stderr, buffer);
    for (j = 0; j < sizeof(tbl) / sizeof(tbl[0]); j++)
	FPRINTF(stderr, "%s\n", tbl[j]);
    (void) exit(FAIL);
}

static void
disjoint(void)
{
    if (add_opt || expunge_opt || purge_opt)
	usage();
    modify++;
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/

/*ARGSUSED*/
_MAIN
{
    int j;
    char *user_opt = 0;
    char *d;
    time_t now = time((time_t *) 0);

    (void) strclean(strcat(strcpy(m_buffer, "-mBASELINE at "), ctime(&now)));

    while ((j = getopt(argc, argv, "a:b:e:lm:npqsu:v")) != EOF)
	switch (j) {
	case 'a':
	    disjoint();
	    user_opt = optarg;
	    add_opt = TRUE;	/* add permissions */
	    break;
	case 'b':
	    if (((base_opt = (int) strtol(optarg, &d, 10)) < 1)
		|| (*d != EOS))
		usage();
	    break;
	case 'e':
	    disjoint();
	    expunge_opt = TRUE;	/* expunge */
	    user_opt = optarg;
	    break;
	case 'l':
	    link_opt = TRUE;
	    break;
	case 'm':
	    (void) strcpy(m_buffer + 2, optarg);
	    break;
	case 'n':
	    null_opt = TRUE;
	    break;
	case 'p':		/* purge permissions */
	    disjoint();
	    purge_opt = TRUE;
	    break;
	case 'q':
	case 's':
	    verbose--;
	    break;
	case 'u':		/* set name (report only) */
	    disjoint();
	    report_opt = TRUE;
	    user_opt = optarg;
	    break;
	case 'v':
	    verbose++;
	    break;
	default:
	    usage();
	    /*NOTREACHED */
	}

    /*
     * Validate user-list, if specified
     */
    if (user_opt) {
	char bfr[BUFSIZ];
	char *s = strcpy(bfr, user_opt);

	for (s = strtok(s, ","); s; s = strtok((char *) 0, ",")) {
	    VERBOSE("** user = %s\n", s);
	    cat_list(user_name, s);
	}
    }

    (void) revert("does not run in set-uid mode");

    if (optind < argc) {
	for (j = optind; j < argc; j++)
	    do_arg(argv[j]);
    } else
	do_arg(".");

    (void) exit(SUCCESS);
    /*NOTREACHED */
}
