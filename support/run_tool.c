#ifndef	NO_IDENT
static const char Id[] = "$Id: run_tool.c,v 11.2 2010/07/04 18:13:43 tom Exp $";
#endif

/*
 * Title:	run_tool
 * Author:	T.E.Dickey
 * Created:	27 Oct 1992
 *
 * Function:	Invokes the proper pathname for the given rcs tool
 */

#include "ptypes.h"
#include "rcsdefs.h"

_MAIN
{
    if (argc > 1) {
	char *tool = argv[1];
	execv(rcspath(tool), &argv[1]);
	failed("execv");
    }
    return EXIT_SUCCESS;
}
