/*
 * Title:	run_tool
 * Author:	T.E.Dickey
 * Created:	27 Oct 1992
 *
 * Function:	Invokes the proper pathname for the given rcs tool
 */

#include <ptypes.h>
#include <rcsdefs.h>

MODULE_ID("$Id: run_tool.c,v 11.2 1992/10/27 09:38:27 tom Exp $")

_MAIN
{
	if (argc > 1) {
		char	*tool	= argv[1];
		execv(rcspath(tool), &argv[1]);
		failed("execv");
	}
	/*NOTREACHED*/
}
