#ifndef	lint
static	char	sccs_id[] = "@(#)copy.c	1.7 88/08/25 15:48:55";
#endif	lint

/*
 * Title:	copy.c (enhanced unix copy utility)
 * Author:	T.E.Dickey
 * Created:	16 Aug 1988
 * Modified:
 *		25 Aug 1988, added "-u" option for checkin/checkout setuid.
 *		23 Aug 1988, replaced call on 'name2s()' by call to apollo
 *			     naming-server to translate names (works better)
 *		19 Aug 1988, corrected 'name2s()' call to handle "/bin/[".
 *		18 Aug 1988, use 'name2s()' so AEGIS can handle our mixed-case
 *			     filenames.
 *		17 Aug 1988, use AEGIS 'cpf' to copy files on apollo.
 *
 * Function:	Provides a portable copy function on unix systems, with the
 *		following functions (which are provided by some, but not all
 *		unix 'cp' programs):
 *
 *		* preserves modification time of copied file
 *		* lists names as they are copied
 *
 * patch:	how can we properly copy/replace a tree?
 *
 * patch:	if running as root, should try also to keep owner/mode of the
 *		destination intact.
 *
 * patch:	this copies only files; must handle other stuff such as
 *		directories, symbolic links and devices.
 *
 * patch:	how do we keep from losing the destination file if we have a
 *		fault in the system?
 */

#define		DIR_PTYPES	/* include directory-definitions */
#include	"ptypes.h"
extern	int	optind;		/* index in 'argv[]' of first argument */
extern	char	*strcat(),
		*strcpy(),
		*strrchr();

#define	R_OK	4
#define	W_OK	2

#ifndef	S_IFLNK
#define	lstat	stat
#endif	S_IFLNK

#define	TELL	FPRINTF(stderr,
#define	isFILE(s)	((s.st_mode & S_IFMT) == S_IFREG)
#define	isDIR(s)	((s.st_mode & S_IFMT) == S_IFDIR)

static	long	total_files,
		total_bytes;
static	int	d_opt,		/* obtain source from destination arg */
		i_opt,		/* interactive: force prompt before overwrite */
		n_opt,		/* true if we don't actually do copies */
		r_opt,		/* recursive in directories */
		v_opt;		/* verbose */

/************************************************************************
 *	local procedures						*
 ************************************************************************/

#ifdef	apollo
#include	</sys/ins/base.ins.c>
#include	</sys/ins/name.ins.c>

/*
 * Use the Apollo naming-server to perform complete substitution on a pathname.
 * This is used (rather than 'name2s()'), because Apollo's 'cpf' does not work
 * properly when we have a symbolic link whose text is an absolute pathname
 * (i.e., //dickey/tmp).
 *
 * We still must use 'name2s()' to make a correct string for a leaf which does
 * not yet exist.
 */
static
char *
convert(dst, src)
char	*dst, *src;
{
	extern	char	*strncpy();
	name_$pname_t	in_name, out_name;
	short		in_len,
			out_len;
	register char	*s, *d;
	status_$t	st;

#ifdef	lint
	out_len = 0;
#endif	lint
	in_len = strlen(strcpy(in_name, src));
	name_$get_path(in_name, in_len, out_name, out_len, st);
	if (st.all == status_$ok)
		strncpy(dst, out_name, out_len)[out_len] = EOS;
	else {
		if (	(st.all == name_$not_found)
		&&	(s = strrchr(in_name, '/'))
		&&	(s > in_name)
		&&	(s[-1] != '/') ) {
			*s = EOS;
			d = convert(dst, in_name);
			d += strlen(d);
			*d++ = '/';
			(void)name2s(d, BUFSIZ - (d - dst), ++s, 6);
		} else
			(void)name2s(dst, BUFSIZ, src, 6);
	}
	if (v_opt > 1)
		TELL "++ \"%s\" => \"%s\"\n", src, dst);
	return (dst);
}
#endif	apollo

/*
 * On apollo machines, each file has an object type, which is not necessarily
 * mapped into the unix system properly.  Invoke the native AEGIS 'cpf' program
 * to do the copy.
 */
static
copyfile(src, dst, previous)
char	*src, *dst;
{
	char	bfr1[BUFSIZ];
#ifdef	apollo
	char	bfr2[BUFSIZ];
	if (access(src,R_OK) < 0) {
		perror(src);
		return(-1);
	}
	*bfr1 = EOS;
	catarg(bfr1, convert(bfr2,src));
	catarg(bfr1, convert(bfr2,dst));
	if (previous >= 0)
		catarg(bfr1, "-r");
	if (v_opt > 1)
		TELL "++ cpf %s\n", bfr1);
	return (execute("/com/cpf", bfr1));
#else	apollo
	FILE	*ifp, *ofp;
	int	num;

	if ((ifp = fopen(src, "r")) == 0) {
		perror(src);
		return(-1);
	}
	if (previous >= 0 && unlink(dst) < 0) {
		FCLOSE(ifp);
		perror(dst);
		return(-1);
	}
	if ((ofp = fopen(dst, "w")) == 0) {
		FCLOSE(ifp);
		perror(dst);
		return(-1);
	}
	while ((num = fread(bfr1, 1, sizeof(bfr1), ifp)) > 0)
		if (fwrite(bfr1, 1, num, ofp) != num) {
			perror(dst);
			break;
		}
	FCLOSE(ifp);
	FCLOSE(ofp);
	return (0);
#endif	apollo
}

static
copyit(src, dst)
char	*src, *dst;
{
	struct	stat	dst_sb, src_sb;
	int	num;
	char	bfr1[BUFSIZ],
		bfr2[BUFSIZ],
		*s;

	/* Verify that the source and destinations are distinct */
	abspath(strcpy(bfr1, src));
	abspath(strcpy(bfr2, dst));
	if (!strcmp(bfr1, bfr2)) {
		TELL "?? repeated name: %s\n", bfr2);
		exit(FAIL);
	}

	if (s = strrchr(bfr2, '/')) {
		*s = EOS;
		if (access(bfr2, W_OK) < 0) {
			TELL "?? directory is not writeable: \"%s\"\n", bfr2);
			return;
		}
	}

	if (v_opt)
		TELL "** copy \"%s\" to \"%s\"\n", src, dst);

	/* Verify that the source is a legal file */
	if (lstat(src, &src_sb) < 0) {
		TELL "?? file not found: \"%s\"\n", src);
		return;
	}
	if (! isFILE(src_sb)) {
		TELL "?? not a file: \"%s\"\n", src);
		return;
	}

	/* Check to see if we can overwrite the destination */
	if ((num = lstat(dst, &dst_sb)) >= 0) {
		if (isFILE(dst_sb)) {
			if (i_opt) {
				TELL "%s ? ", dst);
				if (gets(bfr1)) {
					if (*bfr1 != 'y' && *bfr1 != 'Y')
						return;
				} else
					return;
			}
		} else {
			TELL "?? cannot overwrite \"%s\"\n", dst);
			return;
		}
	} else
		dst_sb = src_sb;

	/* Unless disabled, copy the file */
	if (!n_opt) {
		if (copyfile(src,dst,num) < 0)
			return;
		(void)chmod(dst, (int)(dst_sb.st_mode & 0777));
		(void)setmtime(dst, src_sb.st_mtime);
	}

	total_files++;
	total_bytes += src_sb.st_size;
}

static
usage()
{
	TELL "usage: copy [-i] [-v] [-u] {[-d] | source [...]} destination\n");
	(void)exit(FAIL);
}

/*
 * Process argument list, turning it into source/destination pairs
 */
static
arg_pairs(argc, argv)
char	*argv[];
{
	register int	j;
	struct	stat	dst_sb;
	int	num;
	char	dst[BUFSIZ];

	if ((num = (argc - optind)) < 2) {
		TELL "?? You must give both source and destination names\n");
		usage();
	}

	if (lstat(strcpy(dst, argv[argc-1]), &dst_sb) >= 0) {
		if (isDIR(dst_sb)) {
			/* copy one or more items into directory */
			for (j = optind; j < argc-1; j++) {
			char	*s = dst + strlen(dst);
				(void)strcat(strcpy(s, "/"), argv[j]);
				copyit(argv[j], dst);
				*s = EOS;
			}
		} else if (num != 2) {
			TELL "?? Destination is not a directory\n");
			usage();
		} else if (isFILE(dst_sb)) {
			copyit(argv[optind], dst);
		} else {
			TELL "?? Destination is not a file\n");
			usage();
		}
	} else {
		if (num != 2) {
			TELL "?? Wrong number of arguments\n");
			usage();
		}
		copyit(argv[optind], dst);
	}
}

/*
 * Process argument list, obtaining the source name (actually the leaf) from
 * each argument.
 */
static
derived(argc, argv)
char	*argv[];
{
	register int	j;
	char	dst[BUFSIZ], *s;

	for (j = optind; j < argc; j++) {
		(void)strcpy(dst, argv[j]);
		while (s = strrchr(dst, '/')) {
			if (s[1]) {
				copyit(s+1, dst);
				break;
			} else
				*s = EOS;
		}
		if (s == 0)
			TELL "?? No destination directory: \"%s\"\n", argv[j]);
	}
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/
main(argc,argv)
char	*argv[];
{
	register int	j;

	while ((j = getopt(argc, argv, "dinruv")) != EOF) switch (j) {
	case 'd':	d_opt = TRUE;	break;
	case 'i':	i_opt = TRUE;	break;
	case 'n':	n_opt = TRUE;	break;
	case 'r':	r_opt = TRUE;	break;
	case 'u':	(void)setuid(getuid());	break;
	case 'v':	v_opt++;	break;
	default:	usage();
	}

	if (d_opt)
		derived(argc, argv);
	else
		arg_pairs(argc, argv);

	if (v_opt)
		TELL "** %ld file%s %scopied, %ld bytes\n",
			total_files,
			total_files == 1 ? "" : "s",
			n_opt ? "would be " : "",
			total_bytes);
	(void)exit(SUCCESS);
	/*NOTREACHED*/
}

failed(s)
char	*s;
{
	perror(s);
	exit(FAIL);
}
