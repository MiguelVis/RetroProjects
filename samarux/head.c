/* head - print the first few lines of a file	Author: Andy Tanenbaum */

/*	head.c

	Taken from UZIX (UZI implementation for MSX computers),
	but it seems that the original source comes from MINIX.

	Converted to MESCC by Miguel I. Garcia Lopez / FloppySoftware, Spain

	18 Dec 2014 : Version for CP/M.
	24 Aug 2015 : Version for SamaruX as external command.

	To build the CP/M version:

	cc head
	zsm head
	hexcom head

	To build the SamaruX version:

	cc head
	zmac head.zsm --rel
	link head [op]
	ren head.x=head.prl

	Define only one of following defs.:
*/

/* #define FOR_CPM */
#define FOR_SAMARUX

#ifdef FOR_CPM

/* CP/M version
   ------------
*/

#define CC_STDIO  /* Support for stdin, stdout, stderr */
#define CC_REDIR  /* Support for CP/M command line redirection */

#include <mescc.h>
#include <conio.h>
#include <fileio.h>
#include <printf.h>
#include <fprintf.h>
#include <redir.h>

#endif

#ifdef FOR_SAMARUX

/* SamaruX version
   ---------------
*/

#include <samarux.h>

#endif

#define DEFAULT 10

#define void
#define getc fgetc
#define putc fputc

int main(argc, argv)
int argc, argv[]; /* char *argv[] */
{
  FILE *f;
  int n, k, nfiles;
  char *ptr;
  
  /* Check for flag.  Only flag is -n, to say how many lines to print. */
  k = 1;
  ptr = argv[1];
  n = DEFAULT;
  if (argc > 1 && *ptr++ == '-') {
	k++;
	n = atoi(ptr);
	if (n <= 0) {
		usage();
		return(1);
	}
  }
  nfiles = argc - k;

  if (nfiles == 0) {
	/* Print standard input only. */
	if (stdin == NULL) {
		usage();
		return(1);
	}
	do_file(n, stdin);
	return(0);
  }

  /* One or more files have been listed explicitly. */
  while (k < argc) {
	if (nfiles > 1) printf("==> %s <==\n", argv[k]);
	if ((f = fopen(argv[k], "r")) == NULL)
		printf("head: cannot open %s\n", argv[k]);
	else {
		do_file(n, f);
		fclose(f);
	}
	k++;
	if (k < argc) printf("\n");
  }
  return(0);
}

void do_file(n, f)
int n;
FILE *f;
{
  int c;

  /* Print the first 'n' lines of a file. */
  while (n) switch (c = getc(f)) {
	    case EOF:
		return;
	    case '\n':
		--n;
	    default:	putc(c, stdout);
	}
}

void usage()
{
  fprintf(stderr, "Usage: head [-n] [file ...]\n");
}
