/* $Id: vcs.h,v 11.1 2004/03/08 01:06:27 tom Exp $ */

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

MAIN	int	RCS_uid,	/* owner and group of RCS-directory */
		RCS_gid,
		RCS_prot;	/* protection of reference-directory */
MAIN	char	RCS_cmd[BUFSIZ];/* command we issue in set-uid mode */
MAIN	char	*RCS_verb,	/* command-verb we show in trace */
		*RCS_path;	/* full path of command */

MAIN	char	original[MAXPATHLEN];

	/* utilities */
extern	void	set_command(void);

extern	void	set_option(
		int		option,
		char *		value);

extern	int	do_command(void);

extern	void	invoke_command(
		char *		verb,
		char *		path);

extern	void	ShowPath(
		char *		command,
		char *		name);

extern	int	DirExists(
		char *		path);

extern	int	IsDirectory(
		char *		path);

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
