#ifndef	NO_IDENT
static const char Id[] = "$Id: set_date.c,v 11.4 2010/07/04 18:13:58 tom Exp $";
#endif

#include "ptypes.h"
#include <time.h>

static void
usage(void)
{
    FPRINTF(stderr, "usage: set_date [-m mode] [-t date] file1 [...]\n");
    exit(FAIL);
}

_MAIN
{
    time_t when = time((time_t *) 0);
    int mode = 0444;
    int quiet = FALSE;
    char *mark;
    int j;

    while ((j = getopt(argc, argv, "m:qt:")) != EOF)
	switch (j) {
	case 'm':
	    mode = strtol(optarg, &mark, 8) & 0777;
	    if (*mark != EOS || mark == optarg) {
		FPRINTF(stderr, "? mode \"%s\"\n", optarg);
		usage();
	    }
	    break;
	case 'q':
	    quiet = TRUE;
	    break;
	case 't':
	    when = cutoff(argc, argv);
	    break;
	default:
	    usage();
	}

    if (!quiet)
	FPRINTF(stderr, "set_date %03o %s", mode, ctime(&when));
    if (optind < argc) {
	while (optind < argc) {
	    char *name = argv[optind++];
	    if (setmtime(name, when, when) < 0)
		failed(name);
	}
    } else
	usage();
    exit(SUCCESS);
    /*NOTREACHED */
}
