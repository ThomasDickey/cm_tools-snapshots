/* $Id: vcs.h,v 11.3 2019/12/04 01:37:02 tom Exp $ */
/* *INDENT-OFF* */

#ifndef	_vcs_h
#define	_vcs_h

#ifndef	MAIN
#define	MAIN	extern
#endif

#define	ACC_PTYPES
#define	STR_PTYPES
#include	<ptypes.h>
#include	<rcsdefs.h>
#include	<errno.h>

#define	RCS	"rcs"
#define	PERMIT	"permit"

#define	VERBOSE	if (verbose) PRINTF

typedef	enum	{ Unknown, Insert, Delete, Unlock } OPS;

MAIN	int	debug;
MAIN	int	no_op;		/* no-op mode (show w/o doing it) */
MAIN	OPS	operation;
MAIN	int	q_opt;		/* quiet-option, passed to exec'd programs */
MAIN	int	x_opt;		/* expand full-path in 'rcsopen()' */

MAIN	int	verbose;

MAIN	uid_t	RCS_uid;	/* owner and group of RCS-directory */
MAIN	gid_t	RCS_gid;
MAIN	mode_t	RCS_prot;	/* protection of reference-directory */
MAIN	const char *RCS_verb;	/* command-verb we show in trace */
MAIN	char   *RCS_argv[20];	/* parameters for command */
MAIN	int	RCS_argc;

MAIN	char	original[MAXPATHLEN];

	/* utilities */
extern	void	set_command(void);

extern	void	set_option(
		int		option,
		const char *	value);

extern	void	add_params(
		const char *	value);

extern	int	do_command(void);

extern	void	invoke_command(
		const char *	verb,
		const char *	path);

extern	void	ShowPath(
		const char *	command,
		const char *	name);

extern	int	DirExists(
		const char *	path);

extern	int	IsDirectory(
		const char *	path);

extern	int	Access(
		char *		archive,
		int		ok_if_noop);

extern	time_t	DateOf(
		char *		name);

	/* modules */
extern	void	DeleteDir(
		char *		name);

extern	int	InsertDir(
		char *		name,
		char *		base);

extern	void	UnLockFile(
		char *		name);

#endif	/* _vcs_h */
/* *INDENT-ON* */
