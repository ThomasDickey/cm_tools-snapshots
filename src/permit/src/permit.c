#ifndef	lint
static	char	sccs_id[] = "$Header: /users/source/archives/cm_tools.vcs/src/permit/src/RCS/permit.c,v 3.0 1989/06/16 09:38:50 ste_cm Rel $";
#endif	lint

/*
 * Title:	permit.c (RCS directory-permissions)
 * Author:	T.E.Dickey
 * Created:	09 Mar 1989
 * $Log: permit.c,v $
 * Revision 3.0  1989/06/16 09:38:50  ste_cm
 * BASELINE Mon Jun 19 14:49:13 EDT 1989
 *
 *		Revision 2.2  89/06/16  09:38:50  dickey
 *		corrected copy to 'm_buffer[]'; removed redundant 'failed()'.
 *		
 *		Revision 2.1  89/06/13  08:58:12  dickey
 *		added -m option to allow override of BASELINE rlog-message
 *		restructured usage-code to make it faster
 *		
 *		Revision 2.0  89/03/31  09:09:53  ste_cm
 *		BASELINE Thu Apr  6 13:54:53 EDT 1989
 *		
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
#include	"ptypes.h"
#include	"rcsdefs.h"
#include	<ctype.h>
#include	<time.h>
extern	time_t	time();
extern	long	strtol();
extern	char	*mktemp();
extern	char	*pathcat();
extern	char	*pathleaf();
extern	char	*rcs_dir();
extern	char	*strtok();
extern	char	*uid2s();
extern	char	*vcs_file();

extern	int	optind;		/* 'getopt()' index to argv */
extern	char	*optarg;	/* 'getopt()' argument in argv */

/************************************************************************
 *	local definitions						*
 ************************************************************************/

#define	WARN	FPRINTF(stderr,
#define	TELL	if(verbose >= 0) PRINTF(
#define	VERBOSE	if(verbose >  0) PRINTF(

static	int	add_opt;		/* "-a" option */
static	int	base_opt;		/* "-b" option */
static	int	expunge_opt;		/* "-e" option */
static	int	purge_opt;		/* "-p" option */
static	int	null_opt;		/* "-n" option */
static	int	report_opt;		/* "-u" option */
static	int	modify;			/* any modification requested */
static	char	user_name[BUFSIZ];	/* user to add/expunge/report */
static	int	verbose;		/* "-v" option */
static	int	lines;			/* line-number, for report */
static	char	high_ver[20];
static	char	base_ver[20];
static	char	m_buffer[BUFSIZ];	/* baseline-message */

/* patch: should break comma-list routines out into library code */
/*
 * Determine if any option (-a, -e or -u) selected a user-name which corresponds
 * to the given argument.
 */
static
on_list(list, s)
char	*list;
char	*s;
{
	if (list != 0) {
		auto	char	bfr[BUFSIZ];
		register char	*d;

		for (d = strtok(strcpy(bfr, list), ",");
			d; d = strtok((char *)0, ",")) {
			if (!strcmp(s, d))
				return (TRUE);
		}
	}
	return (FALSE);
}

/*
 * Append to a comma-separated access-list
 */
static
cat_list(dst, src)
char	*dst, *src;
{
	if (*src) {
		if (*dst)
			(void)strcat(dst, ",");
		(void)strcat(dst, src);
	}
}

/*
 * Remove a specific key from a comma-separated access list, so we can keep
 * track of the ones we have found.
 */
static
del_list(list, key)
char	*list, *key;
{
	register int	len = strlen(key);
	register char	*next;
	auto	 char	*base = list;

	while (*list) {
		if (next = strchr(list, ',')) {
			if ((next++ - list) == len)
				if (!strncmp(list,key,len)) {
					char	*save = list;
					while (*list++ = *next++);
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
static
indent(level)
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
static
set_revision(dst, opt)
char	*dst, *opt;
{
	auto	char	*d;
	auto	char	bfr[20];

	if (*base_ver) {
		if (base_opt) {
			if (strtol(base_ver, &d, 10) > base_opt) {
				WARN "?? -b%d inconsistent with version %s\n",
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
static
set_command(dst)
char	*dst;
{
	*dst = EOS;
	if (verbose < 0)
		catarg(dst, "-q");
}

/*
 * The permit-file has no tip-version.  Force it to have one.
 */
static
set_baseline()
{
	auto	char	tmp[BUFSIZ],
			acc_file[BUFSIZ];
	static	char	msg[]	= "setting baseline version in permit-file";

	(void)vcs_file("./", acc_file, FALSE);

	set_command(tmp);
	catarg(tmp, "-l");
	catarg(tmp, acc_file);
	TELL "co %s\n", tmp);
	if (!null_opt && (execute(rcspath("co"), tmp) < 0))
		failed(msg);

	set_command(tmp);
	set_revision(tmp, "-f");
	catarg(tmp, m_buffer);
	catarg(tmp, acc_file);
	TELL "ci %s\n", tmp);
	if (!null_opt && (execute(rcspath("ci"), tmp) < 0))
		failed(msg);
}

/*
 * Create a permit-file.  Subsequent use of 'baseline' will update the change
 * history of this file.
 */
static
create_permit(s)
char	*s;
{
	auto	char	tmp[BUFSIZ];
	auto	char	bfr[BUFSIZ];
	auto	char	owner[L_cuserid+1];
	auto	FILE	*fp;
	auto	struct stat sb;
	auto	char	acc_file[BUFSIZ],
			tmp_file[BUFSIZ],
			*tmp_desc;
	static	char	TMP_DESC[] = "/tmp/permitXXXXXX";

	/* create filenames */
	(void)vcs_file("./", acc_file, FALSE);
	(void)vcs_file("./", tmp_file, TRUE);

	/* find the owner of the directory to use in access-list */
	if (stat("./", &sb) < 0) {
		perror("./");
		return;
	}

	/* if we are expunging ourselves, no sense in making permit-file */
	(void)strcpy(owner, uid2s(sb.st_uid));
	if (expunge_opt && on_list(user_name, owner))
		return;

	if (!null_opt) {
		(void)unlink(tmp_file);
		if (!(fp = fopen(tmp_file, "w")))
			failed("opening temporary file");
#define	KEY(name)	"$%s$\n", "name"	/* prevent rcs-substitution! */
		FPRINTF(fp, KEY(Header));
		FPRINTF(fp, KEY(Log));
		FCLOSE(fp);
	}

	tmp_desc = mktemp(TMP_DESC);
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
	TELL "ci %s\n", tmp);
	if (!null_opt && (execute(rcspath("ci"), tmp) < 0))
		failed("creating permit-file");
	(void)unlink(tmp_file);
	(void)unlink(tmp_desc);

	if (!purge_opt && !expunge_opt) {
		set_command(tmp);
		(void)strcat(strcpy(bfr, "-a"), owner);
		if (add_opt && !on_list(user_name, owner))
			cat_list(bfr, user_name);
		catarg(tmp, bfr);
		catarg(tmp, acc_file);
		TELL "rcs %s\n", tmp);
		if (!null_opt && (execute(rcspath("rcs"), tmp) < 0))
			failed("modifying permit-file");
	}
}

/*
 * Derive the value of $RCS_BASE to store in the permit-file from the highest
 * known value of tip-version from the RCS archive files.
 */
static
compute_base()
{
	register char *s;

	if ((s = strchr(high_ver, '.')) == 0)
		s = high_ver + strlen(high_ver);
	(void)strcpy(s, ".1");
	(void)strcpy(base_ver, high_ver);
	VERBOSE "** new versions begin at %s\n", base_ver);
}

/*
 * Use the 'rcs' utility to add/expunge user(s) from the given archive's access
 * list.
 */
static
modify_access(file, name, opt)
char	*file, *name, *opt;
{
	char	cmd[BUFSIZ], tmp[BUFSIZ];

	if (*name) {
		set_command(cmd);
		catarg(cmd, strcat(strcpy(tmp, opt), name));
		catarg(cmd, file);
		TELL "rcs %s\n", cmd);
		if (!null_opt && (execute(rcspath("rcs"), cmd) < 0))
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
static
do_arcs(path, name, sp, readable, level)
char	*path;
char	*name;
struct	stat	*sp;
{
#ifdef	PATCH
	int	got_lock= FALSE;	/* true if lock found */
#endif	PATCH
	int	got_owner= FALSE;	/* true if owner is on access list */
	int	mode	= (sp != 0) ? (sp->st_mode & S_IFMT) : 0;
	int	header	= TRUE,
		num	= strlen(name) - (sizeof(RCS_SUFFIX) - 1);
	char	*s	= 0,
		list	[BUFSIZ],	/* users to add/expunge */
		to_find	[BUFSIZ],	/* check-off list of users */
		found	[BUFSIZ],	/* users found (for report) */
		owner	[L_cuserid+1],
		tip	[80],		/* tip (top) version number */
		key	[80],		/* current keyword */
		tmp	[BUFSIZ];

	if (mode != S_IFREG)		/* please, no subdirectories of RCS! */
		return(readable);

	if ((num < 0)
	||  strcmp(name + num, RCS_SUFFIX))
		return(readable);

	(void)strcpy(owner, uid2s(sp->st_uid));
	(void)strcpy(to_find, user_name);
	*list = EOS;
	*found = EOS;
	*tip = EOS;

	if (!rcsopen(name, verbose > 1))
		return (FALSE);	/* could not open file anyway */

	while (header && (s = rcsread(s))) {
		s = rcsparse_id(key, s);

		switch (rcskeys(key)) {
		case S_HEAD:
			s = rcsparse_num(tip, s);
			num = dotcmp(tip, high_ver);
			if (num > 0)
				(void)strcpy(high_ver, tip);
			break;
		case S_ACCESS:
			do {
				s = rcsparse_id(tmp,s);
				if (!*tmp)
					break;
				if (!report_opt || on_list(to_find, tmp))
					cat_list(found,tmp);
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
#endif	PATCH
			/* patch: am not handling multiple users for lock */
			/* patch: not sure what to do about locks yet */
			break;
		case S_VERS:
			header = FALSE;
			break;
		case S_COMMENT:
			s = rcsparse_str(s, NULL_FUNC);
			break;
		}
	}
	rcsclose();
	indent(level);
	TELL "%s > %s (%s)\n", tip, name, found);

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
scan_arcs(path, name, level)
char	*path, *name;
{
	*high_ver = EOS;
	(void)walktree(path, name, do_arcs, "r", level);
	VERBOSE "** high version = '%s'\n", *high_ver ? high_ver : "1?");
	if (!*high_ver)
		(void)strcpy(high_ver, "1.1");
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
static
do_base(path, parent, level, flag_)
char	*path;
char	*parent;
int	*flag_;
{
	char		nextpath[BUFSIZ];
	char		acc_file[BUFSIZ];
	struct	stat	sb;

	(void)vcs_file((char *)0, acc_file, FALSE);
	if ((stat(pathcat(nextpath, path, acc_file), &sb) >= 0)
	&&  (sb.st_mode & S_IFMT) == S_IFREG) {
		if (chdir(path) < 0)
			failed(path);
		*high_ver = EOS;
		(void)do_arcs(path, acc_file, &sb, 0, level+1);
		(void)chdir(parent);
		*flag_	= TRUE;
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
static
do_tree(path, name, sp, readable, level)
char	*path;
char	*name;
struct	stat	*sp;
{
	char	*notes;
	int	mode	= (sp != 0) ? (sp->st_mode & S_IFMT) : 0;

	if (mode == S_IFDIR) {
		auto	char	tmp[BUFSIZ],
				bfr[80],
				*s = pathcat(tmp, path, name);
		auto	int	flag	= FALSE;

		abspath(s);		/* get rid of "." and ".." names */
		if (sameleaf(s, rcs_dir())) {
			indent(level);
			TELL "%s/\n", name);

			*base_ver = EOS;
			if (modify)
				scan_arcs(path,name,level);

			if (!do_base(s,path,level,&flag)) {
				if (!modify)
					scan_arcs(path,name,level);
				if (report_opt)
					return(-1);

				if (chdir(s) < 0)
					failed(s);
				if (flag)
					set_baseline();
				else
					create_permit(s);
				(void)chdir(path);
			} else if (base_opt) {
				FORMAT(bfr, "%d.1", base_opt);
				if (strcmp(base_ver, bfr)) {
					if (chdir(s) < 0)
						failed(s);
					set_baseline();
					(void)chdir(path);
				}
			}
			return (-1);	/* don't let walktree try this alone! */
		}
	}

	if (readable < 0 || sp == 0) {
		VERBOSE "?? %s/%s\n", path, name);
	} else if (mode == S_IFDIR) {
		notes = (name[strlen(name)-1] == '/') ? "" : "/";
#ifdef	S_IFLNK
		if ((lstat(name, sp) >= 0)
		&&  (sp->st_mode & S_IFMT) == S_IFLNK) {
			notes = " (link)";
			readable = -1;
		}
#endif	S_IFLNK
		indent(level);
		TELL "%s%s\n", name, notes);
	}
	return(readable);
}

/*
 * Process a single argument: a directory name.
 */
static
do_arg(name)
char	*name;
{
	TELL "** path = %s\n", name);
	lines	= 0;
	(void)walktree((char *)0, name, do_tree, "r", 0);
}

usage()
{
	auto	char	buffer[BUFSIZ];
	static	char	*tbl[] = {
	"usage: permit [-{a|e|u}USER] [-bBASE] [-p] [-nqsv] [directory [...]]",
	"",
	"This maintains a RCS-directory permissions file, as well as",
	"the access lists for archive files.",
	"",
	"options:",
	"  -aUSER  add specified user(s) to access list",
	"  -bBASE  set baseline value for directory",
	"  -eUSER  expunge specified user(s) from access list",
	"  -mTEXT  specifies baseline-message (default: BASELINE {date}",
	"  -n      no-op mode (shows actions that would be done)",
	"  -p      purge all users from access list",
	"  -q      quiet (less verbose)",
	"  -uUSER  report specified user(s) in access list",
	"  -v      verbose",
	};
	register int	j;
	setbuf(stderr,buffer);
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		WARN "%s\n", tbl[j]);
	(void)exit(FAIL);
}

static
disjoint()
{
	if (add_opt || expunge_opt || purge_opt)
		usage();
	modify++;
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/

main(argc, argv)
char	*argv[];
{
	register int	j;
	auto	 char	*user_opt = 0;
	auto	 char	*d;
	auto	time_t	now	= time((time_t *)0);

	(void)strclean(strcat(strcpy(m_buffer, "-mBASELINE at "), ctime(&now)));

	while ((j = getopt(argc, argv, "a:b:e:m:npqsu:v")) != EOF)
		switch (j) {
		case 'a':
			disjoint();
			user_opt = optarg;
			add_opt = TRUE;		/* add permissions */
			break;
		case 'b':
			if (((base_opt = strtol(optarg, &d, 10)) < 2)
			||  (*d != EOS))
				usage();
			break;
		case 'e':
			disjoint();
			expunge_opt = TRUE;	/* expunge */
			user_opt = optarg;
			break;
		case 'm':
			(void)strcpy(m_buffer+2, optarg);
			break;
		case 'n':
			null_opt = TRUE;
			break;
		case 'p':			/* purge permissions */
			disjoint();
			purge_opt = TRUE;
			break;
		case 'q':
		case 's':
			verbose--;
			break;
		case 'u':			/* set name (report only) */
			disjoint();
			report_opt = TRUE;
			user_opt = optarg;
			break;
		case 'v':
			verbose++;
			break;
		default:
			usage();
			/*NOTREACHED*/
		}

	/*
	 * Validate user-list, if specified
	 */
	if (user_opt) {
		auto	char	bfr[BUFSIZ];
		register char	*s = strcpy(bfr, user_opt);

		for (s = strtok(s, ","); s; s = strtok((char *)0, ",")) {
			VERBOSE "** user = %s\n",s);
			if (s2uid(s) < 0) {
				TELL "?? user \"%s\" not found\n", s);
				(void)exit(FAIL);
			}
			cat_list(user_name, s);
		}
	}

	revert("does not run in set-uid mode");

	if (optind < argc) {
		for (j = optind; j < argc; j++)
			do_arg(argv[j]);
	} else
		do_arg(".");

	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
