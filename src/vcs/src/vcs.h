/* $Id: vcs.h,v 11.0 1991/10/17 13:51:35 ste_cm Rel $ */

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

#define	WARN	FPRINTF(stderr,
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
extern	void	set_command(_ar0);

extern	void	set_option(
		_arx(int,	option)
		_ar1(char *,	value));

extern	int	do_command(_ar0);

extern	void	invoke_command(
		_arx(char *,	verb)
		_ar1(char *,	path));

extern	void	ShowPath(
		_arx(char *,	command)
		_ar1(char *,	name));

extern	int	DirExists(
		_ar1(char *,	path));

extern	int	IsDirectory(
		_ar1(char *,	path));

extern	int	Access(
		_arx(char *,	archive)
		_ar1(int,	ok_if_noop));

extern	time_t	DateOf(
		_ar1(char *,	name));

	/* modules */
extern	void	DeleteDir(
		_ar1(char *,	name));

extern	int	InsertDir(
		_arx(char *,	name)
		_ar1(char *,	base));

extern	void	UnLockFile(
		_ar1(char *,	name));

#endif	/* _vcs_h */
