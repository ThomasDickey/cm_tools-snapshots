#ifndef	lint
static	char	Id[] = "$Id: checkin.c,v 7.0 1990/04/19 12:06:04 ste_cm Rel $";
#endif	lint

/*
 * Title:	checkin.c (RCS checkin front-end)
 * Author:	T.E.Dickey
 * Created:	19 May 1988, from 'sccsbase'
 * $Log: checkin.c,v $
 * Revision 7.0  1990/04/19 12:06:04  ste_cm
 * BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
 *
 *		Revision 6.3  90/04/19  12:06:04  dickey
 *		corrected missing braces which broke 'chmod()' in last change
 *		
 *		Revision 6.2  90/04/19  10:31:55  dickey
 *		added "-d" option (for want of a better code for no-op).
 *		Used this option to suppress invocation of ci/rcs/mkdir/chmod
 *		and unlink (also changed "TELL" for these calls) so user can
 *		preview the actions which would be attempted.  Note that the
 *		no-op mode cannot (as yet) do anything interesting for the
 *		set-uid mode.
 *		Used the no-op mode to find that GetLock was not returning
 *		the proper code (since 'strict' was set after 'locks'!).
 *		Fixed this.
 *		Finally, recoded the "rcs -c" stuff so that user can add to
 *		the default list of comment-prefixes by setting the environment
 *		variable RCS_COMMENT appropriately.
 *		
 *		Revision 6.1  90/04/18  15:01:50  dickey
 *		added "-x" option (to assist in makefiles, etc).
 *		
 *		Revision 6.0  90/03/05  13:48:49  ste_cm
 *		BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
 *		
 *		Revision 5.4  90/03/05  13:48:49  dickey
 *		corrected length of 'tmp_name[]'
 *		
 *		Revision 5.3  90/03/05  13:47:24  dickey
 *		port to sun3 (os3.4) which has bug in tmpfile & mktemp.
 *		cleanup error-exits.
 *		
 *		Revision 5.2  89/12/12  10:31:40  dickey
 *		lint (SunOs 4.0.3)
 *		
 *		Revision 5.1  89/12/06  07:42:59  dickey
 *		if option "-?" given, don't print warning before usage, so
 *		we can invoke checkin from rcsput for combined-usage.
 *		
 *		Revision 5.0  89/09/21  09:29:41  ste_cm
 *		BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
 *		
 *		Revision 4.1  89/09/21  09:29:41  dickey
 *		added "rcs -c" decoding for IMakefile, AMakefile, changed
 *		the decoding for ".com"
 *		
 *		Revision 4.0  89/04/04  10:44:41  ste_cm
 *		BASELINE Thu Aug 24 09:23:45 EDT 1989 -- support:navi_011(rel2)
 *		
 *		Revision 3.0  89/04/04  10:44:41  ste_cm
 *		BASELINE Mon Jun 19 13:07:30 EDT 1989
 *		
 *		Revision 2.0  89/04/04  10:44:41  ste_cm
 *		BASELINE Fri Apr  7 16:39:42 EDT 1989
 *		
 *		Revision 1.33  89/04/04  10:44:41  dickey
 *		ensure that we call 'rcspermit()' not only to check permissions,
 *		but also to obtain value for RCSbase variable.
 *		
 *		Revision 1.32  89/03/31  14:55:22  dickey
 *		only close temp-file if we have opened it!
 *		
 *		Revision 1.31  89/03/29  14:43:06  dickey
 *		if working file cannot be found, this may be because checkin is
 *		running in set-uid mode.  revert to normal rights and try again.
 *		
 *		Revision 1.30  89/03/21  13:51:08  dickey
 *		sccs2rcs keywords
 *		
 *		21 Mar 1989, after invoking 'revert()', could no longer write to
 *			     temp-file (fpT); moved 'tmpfile()' call to fix.
 *		15 Mar 1989, if no tip-version found, assume we can create
 *			     initial revision.
 *		08 Mar 1989, use 'revert()' and 'rcspermit()' to implement
 *			     CM-restrictions to setuid.
 *		27 Feb 1989, Set comment for VMS filetypes .COM, .MMS, as well
 *			     as unix+VMS types ".mk" and ".e".  Also, test for
 *			     the special case of Makefile/makefile.
 *		06 Dec 1988, corrected handling of group-restricted archives.
 *			     corrected setting of RCSprot -- used in HackMode().
 *		28 Sep 1988, use $RCS_DEBUG to control debug-trace.
 *		27 Sep 1988, forgot to make "rcs" utility perform "-t" option.
 *		13 Sep 1988, for ADA-files (".a" or ".ada", added rcs's comment
 *			     header (not in rcs's default table).  Catch signals
 *			     to do cleanup.  Correctly 'rm_work()' for setuid.
 *			     Added newtime/oldtime logic to cover up case in
 *			     which 'ci' aborts but does not return error.
 *		09 Sep 1988, use 'rcspath()'
 *		30 Aug 1988, make this work as a setuid process.  If file has
 *			     not been checked-in, initialize its access list
 *			     first.
 *		24 Aug 1988, added 'usage()'; create directory if not found.
 *		15 Aug 1988, If "-k" option is used, assume time+date from the
 *			     RCS file, not from the working file.  Also, supply
 *			     default log-message for "-k" option.
 *		05 Aug 1988, revised interface with 'rcsname.c' module.
 *		08 Jul 1988, set "silent" before first call on GetLock if "-q"
 *			     precedes filename in argv.
 *		01 Jul 1988, added chmod to fix cases in which 'ci' leaves the
 *			     file writeable (Apollo bug?).  Added interpretation
 *			     for environment variable which specifies base
 *			     value for revision if none is given in the options
 *			     list (i.e., RCS_BASE).
 *		27 May 1988, recoded using 'rcsedit' module.
 *		21 May 1988, broke out common routines for 'checkout.c'.
 *
 * Function:	Invoke RCS checkin 'ci', then modify the last delta-date of the
 *		corresponding RCS-file to be the same as the modification date
 *		of the file.  The RCS-files are assumed to be in the standard
 *		location:
 *
 *			name => RCS/name,v
 *
 * patch:	Since 'ci' uses 'access()' to verify that it can put a semaphore
 *		in the RCS directory, I don't see any simple way of handling
 *		this except to make the RCS directory temporarily publicly
 *		writeable ...
 */

#define		STR_PTYPES
#include	"ptypes.h"
#include	"rcsdefs.h"

#include	<ctype.h>
#include	<pwd.h>
#include	<signal.h>
#include	<time.h>
extern	struct	tm *localtime();
extern	long	packdate();
extern	int	errno;
extern	char	*ftype();
extern	char	*getuser();
extern	char	*mktemp();
extern	char	*pathleaf();

/* local declarations: */
#define	REV_OPT	"rfluqk"
#define	is_t_opt(s)	(s[1] == 't')

#define	WARN	FPRINTF(stderr,
#define	TELL	if (!silent) PRINTF
#define	DEBUG(s)	if (debug) PRINTF s;

static	int	silent	= FALSE,
		debug,			/* set by RCS_DEBUG environment */
		no_op,			/* show, but don't do */
		x_opt,			/* extended pathnames */
		locked	= FALSE,	/* set if file is re-locked */
		from_keys = FALSE,	/* set if we get date from RCS file */
		new_file,		/* per-file, true if no archive */
		TMP_mode,		/* saved protection of RCS-directory */
		RCS_uid,		/* archive's owner */
		RCS_gid,		/* archive's group */
		RCSprot;		/* protection of RCS-directory */
static	time_t	modtime,		/* timestamp of working file */
		oldtime;		/* timestamp of archive file */
static	char	*Working,
		*Archive,
		*TMP_file,		/* temp-file used to fix uid/access */
		*t_option,		/* "-t" option used for "ci" or "rcs" */
		RCSdir[BUFSIZ],
		RCSbase[20],		/* base+ version number */
		opt_all[BUFSIZ],	/* options for 'ci' */
		opt_rev[BUFSIZ],
		old_date[BUFSIZ],
		new_date[BUFSIZ];

/************************************************************************
 *	local procedures						*
 ************************************************************************/

/*
 * Remove working-file for set-uid process if the matching temp-file was.
 */
static
rm_work() { return (unlink(Working)); }

static
clean_file()
{
	if ((Working  != 0)
	&&  (TMP_file != 0)
	&&  strcmp(Working,TMP_file)) {
		if (no_op)
			TELL("%% rm -f %s\n", TMP_file);
		(void)unlink(TMP_file);
	}
	TMP_file = 0;
}

/*
 * If interrupted, clean up and exit
 */
static
SIGS_T
cleanup(sig)
{
	(void)signal(sig, SIG_IGN);
	if (TMP_file)	clean_file();
	if (TMP_mode)	HackMode(FALSE);
	WARN "checkin: cleaned up\n\n");
	(void)exit(FAIL);
}

/*
 * If the given file is still checked-out, touch its time.
 * patch: should do keyword substitution for Header, Date a la 'co'.
 */
static
ReProcess ()
{
	auto	char	tmp_name[L_tmpnam];
	auto	FILE	*fpS, *fpT = 0;
	auto	struct	stat	sb;
	auto	size_t	len	= strlen(old_date);
	auto	int	mode,
			lines	= 0,
			changed	= 0;	/* number of substitutions done */
	register char	*s, *d;
	auto	char	bfr[BUFSIZ];

	if (!from_keys) {

		/* Alter the delta-header to match RCS's substitution */
		/* 0123456789.123456789. */
		/* yy.mm.dd.hh.mm.ss */
		new_date[ 2] = old_date[ 2] =
		new_date[ 5] = old_date[ 5] = '/';
		new_date[ 8] = old_date[ 8] = ' ';
		new_date[11] = old_date[11] =
		new_date[14] = old_date[14] = ':';

		if (!(fpS = fopen(Working, "r")))
			return;
		FORMAT(tmp_name, "%s/pci%d", P_tmpdir, getpid());
		(void)unlink(tmp_name);
		if (!(fpT = fopen(tmp_name, "w+")))
			GiveUp("Could not create tmp-file", tmp_name);

		while (fgets(bfr, sizeof(bfr), fpS)) {
		char	*last = bfr + strlen(bfr) - len;

			lines++;
			if ((last > bfr)
			&&  (d = strchr(bfr, '$'))) {
				while (d <= last) {
					if (!strncmp(d, old_date, len)) {
						DEBUG(("...edit date at %d\n",
							lines))
						for (s = new_date; *s; s++)
							*d++ = *s;
						changed++;
					}
					d++;
				}
			}
			(void) fputs(bfr, fpT);
		}
		FCLOSE(fpS);
	}

	if ((stat(Working, &sb) >= 0)
	&&  ((sb.st_mode & S_IFMT) == S_IFREG)) {
		mode = sb.st_mode & 0777;
		if (changed) {
			TMP_file = rcstemp(Working, FALSE);
			DEBUG(("...copyback %d lines to %s\n", lines, TMP_file))
			(void)copyback(fpT, TMP_file, mode, lines);
			if (strcmp(TMP_file,Working)) {
				if (usercopy(TMP_file, Working) < 0)
					GiveUp("recopy", Working);
			}
			clean_file();
		}
		if (!locked)		/* cover up bugs on Apollo acls */
			mode &= ~0222;
		if (userprot(Working, mode, modtime) < 0)
			GiveUp("touch failed", Working);
	}
	if (fpT != 0) {
		FCLOSE(fpT);
		(void)unlink(tmp_name);
	}
}

/*
 * Post-process a single RCS-file, replacing its check-in date with the file's
 * modification date.
 */
static
PostProcess ()
{
int	header	= TRUE,
	match	= FALSE;
char	*s	= 0,
	*old,
	revision[BUFSIZ],
	token[BUFSIZ];

	if (!rcsopen(Archive, debug))
		return;
	(void)strcpy(revision, opt_rev);

	while (header && (s = rcsread(s))) {
		s = rcsparse_id(token, s);

		switch (rcskeys(token)) {
		case S_HEAD:
			s = rcsparse_num(token, s);
			if (!*revision)
				(void)strcpy(revision, token);
			break;
		case S_COMMENT:
			s = rcsparse_str(s, NULL_FUNC);
			break;
		case S_VERS:
			if (dotcmp(token, revision) < 0)
				header = FALSE;
			match = !strcmp(token, revision);
			break;
		case S_DATE:
			if (!match)
				break;
			s = rcsparse_num(old_date, old = s);
			if (*old_date) {
			struct	tm *t;
				newzone(5,0,FALSE);	/* format for EST5EDT */
				if (from_keys) {
				int	year, mon, day, hour, min, sec;
					if (sscanf(old_date, FMT_DATE,
						&year, &mon, &day,
						&hour, &min, &sec) == 6) {
						modtime = packdate(1900+year,
							mon, day, hour, min,
							sec);
					}
				} else {
					t = localtime(&modtime);
					FORMAT(new_date, FMT_DATE,
						t->tm_year, t->tm_mon + 1,
						t->tm_mday, t->tm_hour,
						t->tm_min,  t->tm_sec);
					rcsedit(old, old_date, new_date);
				}
				oldzone();
				TELL("** revision %s\n", revision);
				TELL("** modified %s", ctime(&modtime));
			}
			break;
		case S_DESC:
			header = FALSE;
		}
	}
	rcsclose();
}

/*
 * The RCS utilities (ci and rcs) use the unix 'access()' function to test if
 * the caller has access to the archive-directory.  This is not done properly,
 * but I cannot easily code around it other than by temporarily changing the
 * directory's protection.
 */
static
HackMode(save)
{
	int	need;

	if (save) {
		if ((RCSdir[0] != EOS)
		&&  (getuid()  != RCS_uid)) {
			need = (getgid() == RCS_gid) ? 0775 : 0777;
			DEBUG(("...chmod %04o %s, was %04o\n",
				need, RCSdir, RCSprot))
			if (need != RCSprot) {
				TMP_mode = RCSprot | 01000;
				if (no_op) {
					TELL("%% chmod %#o %s\n", need, RCSdir);
				} else if (chmod(RCSdir, need) < 0)
					GiveUp("Could not change mode",RCSdir);
			}
		}
	} else if (TMP_mode) {
		TMP_mode &= 0777;
		if (no_op)
			TELL("%% chmod %#o %s\n", TMP_mode, RCSdir);
		DEBUG(("...chmod %04o %s, to restore\n", TMP_mode, RCSdir))
		(void)chmod(RCSdir, TMP_mode);
		TMP_mode = 0;
	}
}

/*
 * Check in the file using the RCS 'ci' utility.  If 'ci' does something, then
 * it will delete or modify the checked-in file -- return TRUE.  If no action
 * is taken, return false.
 */
static
Execute()
{
	char	cmds[BUFSIZ];
	struct	stat	sb;
	int	code;

	HackMode(TRUE);
	TMP_file = rcstemp(Working, TRUE);

	FORMAT(cmds, "%s%s %s", opt_all, Archive, TMP_file);
	TELL("%% ci %s\n", cmds);
	if (no_op)
		code = -1;
	else
		code = execute(rcspath("ci"), cmds);
	HackMode(FALSE);		/* ...restore protection */

	if (code >= 0) {			/* ... check-in file ok */
		if (stat(TMP_file, &sb) >= 0) {	/* working file not deleted */
			if (strcmp(TMP_file,Working)) {
				if (sb.st_mtime != modtime) {
					if (usercopy(TMP_file,Working) < 0)
						GiveUp("Copy to userfile",Working);
				}
			}
			clean_file();
			if (sb.st_mtime == modtime) {
				TELL("** checkin was not performed\n");
				return (FALSE);
			}
		} else if (strcmp(TMP_file, Working)
		&&	   for_user(rm_work) < 0)
			GiveUp("Deletion of working-file",Working);
		clean_file();
		return (TRUE);
	}
	clean_file();
	return (FALSE);
}

/*
 * This procedure is invoked only if 'SetAccess()' finds an RCS-archive.
 * Ensure that we have a unique lock-value.  If the user did not specify one,
 * we must get it from the archived-file.
 */
static
GetLock()
{
	int	header	= TRUE,
		strict	= FALSE;
	char	*s	= 0,
		lock_rev[80],
		lock_by[80],
		tip[BUFSIZ],
		key[BUFSIZ];

	(void)strcpy(lock_by, getuser());
	*lock_rev = EOS;

	if (*opt_rev == EOS) {

		if (!rcsopen(Archive, -debug))
			return (FALSE);	/* could not open file anyway */

		while (header && (s = rcsread(s))) {
			s = rcsparse_id(key, s);

			switch (rcskeys(key)) {
			case S_HEAD:
				s = rcsparse_num(tip, s);
				if (!*tip)
					header = FALSE;
				DEBUG(("...GetLock tip = %s\n", tip))
				break;
			case S_STRICT:
				strict = TRUE;
				break;
			case S_LOCKS:
				s = rcslocks(s, lock_by, lock_rev);
				if (*lock_rev) {
					TELL("** revision %s was locked\n",lock_rev);
					if (!strcmp(tip, lock_rev)) {
					char	*t = strrchr(lock_rev, '.');
					int	last;
						(void)sscanf(t, ".%d", &last);
						FORMAT(t, ".%d", last+1);
						(void)strcpy(opt_rev, lock_rev);
					} else {
						TELL("?? branching not supported\n");
						/* patch:finish this later... */
					}
				}
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
	}
	if (strict && !*lock_rev)
		TELL("?? no lock set by %s\n", lock_by);
	return (!strict || (*opt_rev != EOS));
}

/*
 * Before checkin, verify that the file exists, and obtain its modification
 * time/date.
 */
static
time_t
PreProcess(name)
char	*name;
{
	auto	struct	stat	sb;

	DEBUG(("...PreProcess(%s)\n", name))
	errno = 0;
	if (stat(name, &sb) >= 0) {
		if ((S_IFMT & sb.st_mode) == S_IFREG) {
			DEBUG(("=> date = %s", ctime(&sb.st_mtime)))
			return (sb.st_mtime);
		}
		GiveUp("not a file:", Working);
	}
	DEBUG(("=> not found, errno=%d\n", errno))
	return (0);
}

/*
 * For "rcs -i" command, define/override 'ci' table for comment-prefix for the
 * Log-keyword.  Use the environment variable RCS_COMMENT to permit user-
 * defined conversions (note that RCS_COMMENT is used in our local mod to 'ci'
 * as well, for prefixing the prompt-lines).  We expect an auto-delimited list
 * such as
 *	"/.c/ * -- /,/.d/>>/"
 * in which the delimiter can change for each item in the list.
 */
typedef	struct	_prefix	{
	struct	_prefix	*link;
	char	*suffix,
		*prefix;
	} PREFIX;

static	PREFIX	*clist;

	/*ARGSUSED*/
	def_ALLOC(PREFIX)

static
define_prefix(suffix, prefix)
char	*suffix, *prefix;
{
	extern	char	*stralloc();
	auto	char	tmp[BUFSIZ];
	register PREFIX	*new = ALLOC(PREFIX,1);
	new->link = clist;
	new->suffix = stralloc(suffix);
	new->prefix = stralloc(strcat(strcpy(tmp, "-c"), prefix));
	clist = new;
}

static
char	*
copy_to(dst, src)
char	*dst, *src;
{
	char	delim;
	if (delim = *src) {
		src++;
		while (*src != delim) {
			if (!(*dst++ = *src))	/* look out for EOS ! */
				break;
			src++;
		}
	}
	*dst = EOS;
	return (src);
}

static
char	*
get_prefix(suffix)
char	*suffix;
{
	PREFIX	*new;
	for (new = clist; new != 0; new = new->link) {
		if (!strcmp(new->suffix, suffix))
			return (new->prefix);
	}
	return (0);
}

/*
 * If the RCS archive does not already exist, make one, with an access list
 * properly initialized (i.e., including the current user and the owner of the
 * RCS directory.  Returns FALSE if the archive already exists.
 */
static
SetAccess()
{
	static	char	list[BUFSIZ];	/* static so we do list once */
	auto	char	cmds[BUFSIZ],
			*s, *t;

	if (clist == 0) {
		define_prefix(".a",	"--  ");
		define_prefix(".ada",	"--  ");
		define_prefix(".com",	"$!\t");
		define_prefix(".e",	" * ");
		define_prefix(".mms",	"#\t");
		define_prefix(".mk",	"#\t");
		if (s = getenv("RCS_COMMENT")) {
			char	suffix[BUFSIZ], prefix[BUFSIZ];
			while (*s) {
				s = copy_to(suffix, s);
				s = copy_to(prefix, s);
				if (*s)
					s++;
				define_prefix(suffix, prefix);
				while (*s == ',' || isspace(*s))
					s++;
			}
		}
	}

	if ((oldtime = PreProcess(Archive)) != 0)
		return (FALSE);

	HackMode(TRUE);			/* ...save protection */
	*cmds = EOS;
	if (t_option != 0)
		catarg(cmds, t_option);
	catarg(cmds, "-i");
	if (!*list) {
		FORMAT(list, "-a%s", getuser());
		if (getuid() != geteuid()) {
			register struct passwd *p;
			if (p = getpwuid(RCS_uid))
				FORMAT(list + strlen(list), ",%s", p->pw_name);
			else
				GiveUp("owner of RCS directory for", Working);
		}
	}
	catarg(cmds, list);

	s = ftype(t = pathleaf(Working));
	if (!*s && (	!strcmp(t, "Makefile")
		||	!strcmp(t, "IMakefile")
		||	!strcmp(t, "AMakefile")
		||	!strcmp(t, "makefile")) )
		catarg(cmds, "-c#\t");
	else if (s = get_prefix(s))
		catarg(cmds, s);

	if (silent) catarg(cmds, "-q");
	catarg(cmds, Archive);
	TELL("%% rcs %s\n", cmds);
	if (!no_op) {
		if (execute(rcspath("rcs"), cmds) < 0)
			GiveUp("rcs initialization for", Working);
	}
	HackMode(FALSE);		/* ...restore protection */
	return (TRUE);
}

/*
 * If the RCS-directory does not exist, make it.  Save the name in 'RCSdir[]'
 * in case we need it for the setuid-hack in 'Execute()'.
 */
static
MakeDirectory()
{
	struct	stat	sb;
	char	*s;
	size_t	len;

	if (s = strrchr(Archive, '/')) {
		len = s - Archive;
		if (len == strlen(RCSdir)
		&& !strncmp(RCSdir, Archive, len))
			return (TRUE);	/* same as last directory */

		(strncpy(RCSdir, Archive, len))[len] = EOS;

		if (stat(RCSdir, &sb) >= 0) {
			if (sb.st_uid != geteuid()
			||  sb.st_gid != getegid())
				revert(debug ? "non-CM use" : (char *)0);
			if (!rcspermit(RCSdir,RCSbase))
				revert("not listed in permit-file");
			DEBUG((".. RCSbase='%s'\n", RCSbase))
			RCS_uid = sb.st_uid;
			RCS_gid = sb.st_gid;
			RCSprot = sb.st_mode & 0777;
			if ((sb.st_mode & S_IFMT) == S_IFDIR)
				return (TRUE);
			GiveUp("not a directory:", RCSdir);
		} else {
			revert(debug ? "new user directory" : (char *)0);
			RCS_uid = getuid();
			RCS_gid = getgid();
			RCSprot = 0755;
			TELL("%% mkdir %s\n", RCSdir);
			if (!no_op) {
				if (mkdir(RCSdir, RCSprot) < 0)
					GiveUp("directory-create",RCSdir);
			}
		}
	} else		/* ...else... user is putting files in "." */
		RCSdir[0] = EOS;
	return (TRUE);
}

/*
 * Generate the string 'opt_all[]' which we will pass to 'ci' for options.
 * We may have to generate it before each file because some are initial
 * checkins.
 *
 * For initial checkins we have a special case: if the environment variable
 * RCS_BASE is set, we use this for the revision code rather than "1.1".
 */
static
SetOpts(argc,argv,code)
char	*argv[];
{
	int	last	= 0;
	int	logmsg	= 0;
	register int	j;
	char	bfr[BUFSIZ];
	char	*dft	= (code < 0) ? RCSbase : 0;
	register char *s;

	*opt_rev = EOS;
	*opt_all = EOS;
	for (j = 1; j < argc; j++) {
		s = argv[j];
		if (*s == '-') {
			if (s[1] == 'x' || s[1] == 'd')
				continue;
			if (is_t_opt(s) && !new_file)
				continue;
			catarg(opt_all, s);
			if (*++s == 'q')
				silent++;
			if (strchr(REV_OPT, (long)*s)) {
				if (*s == 'l')	locked = TRUE;
				if (*(++s)) {
					s = strcpy(opt_rev, s);
					s += strlen(opt_rev) - 2;
					if (s > opt_rev && !strcmp(s, ".0"))
						revert("baseline version");
				}
				last = j;
			} else if (*s == 'm')
				logmsg = TRUE;
		}
	}

	/*
	 * If no revision was specified, and there is a default in effect,
	 * reprocess the list of options so that it includes the default
	 * revision.  Also, if no log-message was specified, add one so we
	 * don't get prompted unnecessarily.
	 */
	if ((*opt_rev == EOS)
	&&  (!from_keys)
	&&  ((dft != 0) && (*dft != EOS))) {
		if (last) {
			*opt_all = EOS;
			for (j = 1; j < argc; j++) {
				s = argv[j];
				if (is_t_opt(s) && !new_file)
					continue;
				if (*s == '-') {
					if (j == last) {
						FORMAT(bfr, "%s%s", s, dft);
						s = bfr;
					}
					catarg(opt_all, s);
				}
			}
		} else {	/* no revision-related option was given */
			FORMAT(bfr, "-r%s", dft);
			catarg(opt_all, bfr);
		}
		if (!logmsg) {
			catarg(opt_all, "-mRCS_BASE");
		}
	} else if (from_keys && (*opt_rev == EOS) && (!logmsg))
		catarg(opt_all, "-mFROM_KEYS");
}

static
GiveUp(msg,arg)
char	*msg, *arg;
{
	TELL("?? %s \"%s\"\n", msg, arg);
	(void)exit(FAIL);
}

static
usage()
{
	static	char	*tbl[] = {
"Usage: checkin [-options] [working_or_archive [...]]",
"",
"Options (from \"ci\"):",
"  -r[rev]  assigns the revision number \"rev\" to the checked-in revision",
"  -f[rev]  forces a deposit",
"  -k[rev]  obtains keywords from working file (rev overrides)",
"  -l[rev]  like \"-r\", but follows with \"co -l\"",
"  -u[rev]  like \"-l\", but no lock is made",
"  -q[rev]  quiet mode",
"  -mmsg    specifies log-message \"msg\"",
"  -nname   assigns symbolic name to the checked-in revision",
"  -Nname   like \"-n\", but overrides previous assignment",
"  -sstate  sets the revision-state (default: \"Exp\")",
"  -t[txtfile] writes descriptive text into the RCS file",
"",
"Non-\"ci\" options:",
"  -d       debug/no-op (show actions, but don't do)",
"  -x       assume archive and working file are in path2/RCS and path2",
"           (normally assumes ./RCS and .)"
	};
	register int	j;
	setbuf(stderr, opt_all);
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	(void)exit(FAIL);
}

/************************************************************************
 *	main program							*
 ************************************************************************/

main (argc, argv)
char	*argv[];
{
	register int	j;
	int		code;
	register char	*s;

	debug = RCS_DEBUG;
	if (s = getenv("RCS_BASE"))
		(void)strcpy(RCSbase,s);
	catchall(cleanup);
	DEBUG(("uid=%d, euid=%d, gid=%d, egid=%d\n",
		getuid(), geteuid(), getgid(), getegid()))

	/*
	 * Process the argument list
	 */
	for (j = 1; j < argc; j++) {
		s = argv[j];
		if (*s == '-') {
			if (*(++s) == 'q')
				silent++;
			else if (*s == 'k')
				from_keys++;
			if (is_t_opt(argv[j]))
				t_option = argv[j];
			if (strchr("rfkluqmnNst", (long)*s) == 0) {
				if (*s == 'd') {
					no_op++;
					continue;
				} else if (*s == 'x') {
					x_opt++;
					continue;
				} else if (*s != '?')
					WARN "unknown option: %s\n", s-1);
				usage();
			}
		} else {

			Working = rcs2name(s,x_opt),
			Archive = name2rcs(s,x_opt);
			code = -1;

			if (!(modtime = PreProcess (Working))) {
				revert(debug ? "directory access" : (char *)0);
				if (!(modtime = PreProcess (Working))) {
					GiveUp("file not found:", Working);
				}
			}

			oldtime = 0;	/* assume we don't find archive */
			if (MakeDirectory()
			&&  (new_file = SetAccess()
			||   (code = GetLock()))) {
				SetOpts(j,argv,code);
				if (Execute()) {
					time_t	newtime = PreProcess(Archive);
					if (newtime != 0
					&&  newtime != oldtime) {
						PostProcess();
						ReProcess();
					}
				}
			}
		}
	}
	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
