#include <ptypes.h>
#include <time.h>

MODULE_ID("$Id: set_date.c,v 11.1 1992/02/11 10:14:43 tom Exp $")

static
usage(_AR0)
{
	FPRINTF(stderr, "usage: set_date [-m mode] [-t date] file1 [...]\n");
	exit(FAIL);
}

_MAIN
{
	auto	time_t	when	= time((time_t *)0);
	auto	int	mode	= 0444,
			quiet	= FALSE;
	auto	char *	mark;
	register int	j;

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
			char	*name = argv[optind++];
			if (setmtime(name, when) < 0)
				failed(name);
		}
	} else
		usage();
	exit(SUCCESS);
	/*NOTREACHED*/
}
