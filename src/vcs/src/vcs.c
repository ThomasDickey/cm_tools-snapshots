/*
 * Title:	vcs.c (version-control-system utility)
 * Author:	T.E.Dickey
 * Created:	06 Sep 1989
 * Modified:
 *		22 Sep 1993, gcc warnings
 *		17 Oct 1991, split out 'vcs_insert.c' and 'vcs_unlock.c'
 *		10 Oct 1991, began rewrite (never did finish this!)
 *
 * Function:
 */

#define	MAIN
#include <vcs.h>

MODULE_ID("$Id: vcs.c,v 11.5 2010/07/04 18:22:37 tom Exp $")

/************************************************************************
 *	utility procedures						*
 ************************************************************************/

void
set_command(void)
{
    *RCS_cmd = EOS;
    if (q_opt)
	catarg(RCS_cmd, "-q");
}

void
set_option(int option, char *value)
{
    char temp[BUFSIZ];
    FORMAT(temp, "-%c%s", option, value);
    catarg(RCS_cmd, temp);
}

int
do_command(void)
{
    if (verbose || no_op)
	shoarg(stdout, RCS_verb, RCS_cmd);
    return no_op ? 0 : execute(RCS_path, RCS_cmd);
}

/*
 * Invokes 'do_command' (which in turn runs the RCS_verb + RCS_cmd), setting
 * our uid/gid to the effective user (usually!).
 */
void
invoke_command(const char *verb, const char *path)
{
    RCS_verb = verb;
    RCS_path = path;
    if (for_admin2(do_command, RCS_uid, RCS_gid) < 0)
	failed(RCS_verb);
}

/******************************************************************************/
void
ShowPath(const char *command, const char *name)
{
    if (verbose) {
	char temp[BUFSIZ];
	if (debug) {
	    if (getwd(temp))
		(void) pathcat(temp, temp, name);
	} else
	    (void) relpath(temp, original, name);
	PRINTF("%% %s %s\n", command, temp);
    }
}

/******************************************************************************/
/* test to see if an argument is a directory */
int
DirExists(const char *path)
{
    struct stat sb;
    return (stat(path, &sb) >= 0 && (sb.st_mode & S_IFMT) == S_IFDIR);
}

/******************************************************************************/
/* test to see if an argument is a directory (ignore it if so!) */
int
IsDirectory(const char *path)
{
    struct stat sb;
    if (stat(path, &sb) < 0 || (sb.st_mode & S_IFMT) != S_IFDIR)
	return FALSE;
    VERBOSE(".. is a directory\n");
    return TRUE;
}

/******************************************************************************/
/* find who owns the archive and its directory and govern our use of set-uid
 * rights accordingly */
int
Access(char *archive, int ok_if_noop)
{
    char *path;
    struct stat sb;

    ShowPath("access", archive);
    /* if we cannot even read the archive, give up */
    if (access(archive, R_OK) < 0 && !ok_if_noop) {
	failed(archive);
	/*NOTREACHED */
    }

    path = pathhead(archive, &sb);
    RCS_prot = (sb.st_mode & 0777);
    if (access(path, R_OK | W_OK | X_OK) < 0) {
	/* hope that effective user is this */
	RCS_uid = sb.st_uid;
	RCS_gid = sb.st_gid;
    } else {
	/* real user is sufficient here */
	RCS_uid = getuid();
	RCS_gid = getgid();
    }
    return TRUE;
}

/******************************************************************************/
time_t
DateOf(char *name)
{
    struct stat sb;
    if (stat(name, &sb) >= 0 && (sb.st_mode & S_IFMT) == S_IFREG)
	return sb.st_mtime;
    return 0;
}

/******************************************************************************/
static void
DoArg(char *name)
{
    char base[BUFSIZ];

    VERBOSE("** processing %s\n", name);

    switch (operation) {
    case Unlock:
	if (!IsDirectory(name))
	    UnLockFile(name);
	break;
    case Delete:
	DeleteDir(name);
	break;
    case Insert:
	if (InsertDir(name, base))
	    VERBOSE(".. completed %s.x %s\n", base, name);
	if (chdir(original) != 0)
	    failed(original);
	break;
    default:
	break;
    }
}

/******************************************************************************/
static void
usage(void)
{
    static const char *msg[] =
    {
	"Usage: vcs [options] [names]",
	"",
	"Options:",
	"  -d       delete archive-directory leaf",
	"  -i       insert archive-directory leaf",
	"  -n       no-op mode",
	"  -q       quiet mode",
	"  -u       unlock specified files",
	"  -x       assume archive and working file are in path2/RCS and path2",
	"           (normally assumes ./RCS and .)"
    };
    unsigned j;
    for (j = 0; j < sizeof(msg) / sizeof(msg[0]); j++)
	FPRINTF(stderr, "%s\n", msg[j]);
    exit(FAIL);
}

/******************************************************************************/
/*ARGSUSED*/
_MAIN
{
    register int c;

    debug = RCS_DEBUG;
    operation = Unknown;
    while ((c = getopt(argc, argv, "dinqux")) != EOF)
	switch (c) {
	case 'd':
	    operation = Delete;
	    break;
	case 'i':
	    operation = Insert;
	    break;
	case 'n':
	    no_op = TRUE;
	    break;
	case 'q':
	    q_opt = TRUE;
	    break;
	case 'u':
	    operation = Unlock;
	    break;
	case 'x':
	    x_opt = TRUE;
	    break;
	default:
	    usage();
	    /*NOTREACHED */
	}

    verbose = debug || !q_opt;
    if (operation == Unknown) {
	FPRINTF(stderr, "expected one of -u, -i, -d\n");
	usage();
    }

    if (!getwd(original))
	failed("getwd");

    if (optind < argc) {
	while (optind < argc)
	    DoArg(argv[optind++]);
    } else {
	FPRINTF(stderr, "expected %s name\n",
		(operation == Unlock) ? "file" : "directory");
	usage();
    }
    exit(SUCCESS);
    /*NOTREACHED */
}
