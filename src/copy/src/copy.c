#ifndef	lint
static	char	sccs_id[] = "@(#)copy.c	1.9 89/01/24 11:41:32";
#endif	lint

/*
 * Title:	copy.c (enhanced unix copy utility)
 * Author:	T.E.Dickey
 * Created:	16 Aug 1988
 * Modified:
 *		24 Jan 1989, use 'pathleaf()' to correct pathnames of copy into
 *			     an existing directory.  Added copy-directory with
 *			     "-m" option.
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
 * patch:	if running as root, should try also to keep owner/mode of the
 *		destination intact.
 *
 * patch:	this copies only files and directories; must handle other stuff
 *		such as symbolic links and devices.
 *
 * patch:	how do we keep from losing the destination file if we have a
 *		fault in the system?
 */

#define		DIR_PTYPES	/* include directory-definitions */
#include	"ptypes.h"
extern	int	optind;		/* index in 'argv[]' of first argument */
extern	char	*pathcat(),
		*pathleaf(),
		*strcat(),
		*strcpy(),
		*strrchr();

#define	R_OK	4
#define	W_OK	2

#ifndef	S_IFLNK
#define	lstat	stat
#endif	S_IFLNK

#define	TELL	FPRINTF(stderr,
#define	VERBOSE	if (v_opt) TELL
#define	DEBUG	if (v_opt > 1)	TELL

#define	isFILE(s)	((s.st_mode & S_IFMT) == S_IFREG)
#define	isDIR(s)	((s.st_mode & S_IFMT) == S_IFDIR)

static	long	total_files,
		total_bytes;
static	int	d_opt,		/* obtain source from destination arg */
		i_opt,		/* interactive: force prompt before overwrite */
		m_opt,		/* merge directories */
		n_opt,		/* true if we don't actually do copies */
		v_opt;		/* verbose */
static	int	no_dir_yet;	/* disables access-test on destination-dir */

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
	DEBUG "++ \"%s\" => \"%s\"\n", src, dst);
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
	if (previous)
		catarg(bfr1, "-r");
	DEBUG "++ cpf %s\n", bfr1);
	return (execute("/com/cpf", bfr1));
#else	apollo
	FILE	*ifp, *ofp;
	int	num;

	if ((ifp = fopen(src, "r")) == 0) {
		perror(src);
		return(-1);
	}
	if (previous && chmod(dst, 0644) < 0) {
		FCLOSE(ifp);
		perror(dst);
		return(-1);
	}
	if ((ofp = fopen(dst, previous ? "w+" : "w")) == 0) {
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
copydir(src, dst, previous)
char	*src, *dst;
{
	auto	DIR		*dp;
	auto	struct	direct	*de;
	auto	char		bfr1[BUFSIZ],
				bfr2[BUFSIZ];
	auto	int		save_dir = no_dir_yet;

	DEBUG "copydir(%s, %s, %d)\n", src, dst, previous);
	if (previous > 0) {
		if (!n_opt) {
			if (unlink(dst) < 0) {
				perror(dst);
				return(-1);
			}
		}
	}

	if (previous >= 0) {	/* called from 'copyit()' */
		VERBOSE "** make directory \"%s\"\n", dst);
		if (!n_opt) {
			if (mkdir(dst, 0755) < 0) {
				perror(dst);
				return (-1);
			}
		}
		no_dir_yet = n_opt;
	}

	if (dp = opendir(src)) {
		while (de = readdir(dp)) {
			if (!dotname(de->d_name))
				copyit(	pathcat(bfr1, src, de->d_name),
					pathcat(bfr2, dst, de->d_name));
		}
		(void)closedir(dp);
	}
	no_dir_yet = save_dir;
	return (0);	/* no errors found */
}

/*
 * Set up and perform a COPY
 */
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

	DEBUG "** src: \"%s\"\n** dst: \"%s\"\n", bfr1, bfr2);

	if (!no_dir_yet && (s = strrchr(bfr2, '/'))) {
#ifdef	apollo
		if ((s == (bfr2 + 1)) && (s[-1] == '/'))
			s++;
#endif	apollo
		*s = EOS;
		if (access(bfr2, W_OK) < 0) {
			TELL "?? directory is not writeable: \"%s\"\n", bfr2);
			return;
		}
	}

	VERBOSE "** copy \"%s\" to \"%s\"\n", src, dst);

	/* Verify that the source is a legal file */
	if (lstat(src, &src_sb) < 0) {
		TELL "?? file not found: \"%s\"\n", src);
		return;
	}
	if (!isFILE(src_sb) && !isDIR(src_sb)) {
		TELL "?? not a file: \"%s\"\n", src);
		return;
	}

	/* Check to see if we can overwrite the destination */
	if (num = (lstat(dst, &dst_sb) >= 0)) {
		if (isFILE(dst_sb)) {
			if (i_opt) {
				TELL "%s ? ", dst);
				if (gets(bfr1)) {
					if (*bfr1 != 'y' && *bfr1 != 'Y')
						return;
				} else
					return;
			}
		} else if (isDIR(dst_sb) && isDIR(src_sb)) {
			num = FALSE;	/* we will merge directories */
		} else {
			TELL "?? cannot overwrite \"%s\"\n", dst);
			return;
		}
	} else
		dst_sb = src_sb;

	/* Unless disabled, copy the file */
	if (isDIR(src_sb) && copydir(src,dst,num) < 0)
		return;
	if (!n_opt) {
		if (isFILE(src_sb) && copyfile(src,dst,num) < 0)
			return;
		(void)chmod(dst, (int)(dst_sb.st_mode & 0777));
		(void)setmtime(dst, src_sb.st_mtime);
	}

	if (isFILE(src_sb)) {
		total_files++;
		total_bytes += src_sb.st_size;
	}
}

static
usage()
{
	auto	char	bfr[BUFSIZ];
	setbuf(stderr, bfr);
	TELL "usage: copy [-i] [-v] [-u] {[-d] | source [...]} destination\n\
Options:\n\
  -d  infer source (leaf) from destination path\n\
  -i  interactive (prompt before overwriting)\n\
  -m  merge directories\n\
  -n  no-op (show what would be copied)\n\
  -u  reset effective uid before executing\n\
  -v  verbose\n");
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
	auto	struct	stat	dst_sb,
				src_sb;
	auto	int	num;
	auto	char	dst[BUFSIZ];

	if ((num = (argc - optind)) < 2) {
		TELL "?? You must give both source and destination names\n");
		usage();
	}

	if (lstat(strcpy(dst, argv[argc-1]), &dst_sb) >= 0) {

		if (m_opt) {
			if (num == 2 && isDIR(dst_sb)) {
				VERBOSE "** merge directories\n");
				if ((lstat(argv[optind], &src_sb) >= 0)
				&&  (isDIR(src_sb))) {
					(void)copydir(
						argv[optind],
						argv[argc-1], -1);
					return;
				}
			}
			TELL "?? both arguments must be directories with -m\n");
			usage();
		}

		if (isDIR(dst_sb)) {
			/* copy one or more items into directory */
			for (j = optind; j < argc-1; j++) {
			char	*s = dst + strlen(dst);
				*s = EOS;
				if (s[-1] != '/')
					(void)strcpy(s, "/");
				(void)strcat(s, pathleaf(argv[j]));
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

	while ((j = getopt(argc, argv, "dimnuv")) != EOF) switch (j) {
	case 'd':	d_opt = TRUE;	break;
	case 'i':	i_opt = TRUE;	break;
	case 'm':	m_opt = TRUE;	break;
	case 'n':	n_opt = TRUE;	break;
	case 'u':	(void)setuid(getuid());	break;
	case 'v':	v_opt++;	break;
	default:	usage();
	}

	if (d_opt)
		derived(argc, argv);
	else
		arg_pairs(argc, argv);

	VERBOSE "** %ld file%s %scopied, %ld bytes\n",
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
