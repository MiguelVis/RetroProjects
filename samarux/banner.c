/* banner - print a banner		Author: Brian Wallis */

/*****************************************************************
 *
 * SYSVbanner.c
 *
 * This is a PD version of the SYS V banner program (at least I think
 * it is compatible to SYS V) which I wrote to use with the clock
 * program written by:
 **     DCF, Inc.
 **     14623 North 49th Place
 **     Scottsdale, AZ 85254
 * and published in the net comp.sources.misc newsgroup in early July
 * since the BSD banner program works quite differently.
 *
 * There is no copyright or responsibility accepted for the use
 * of this software.
 *
 * Brian Wallis, brw@jim.odr.oz, 4 July 1988
 *
 *****************************************************************/

/*	banner.c

	Converted to MESCC by Miguel Garcia / FloppySoftware, Spain

	18 Feb 2015 : Version for CP/M.
	02 Apr 2015 : Version for SamaruX as external command.
	23 Aug 2015 : Added usage text.

	To build the CP/M version:

	cc banner
	zsm banner
	hexcom banner

	To build the SamaruX version:

	cc banner
	zmac banner.zsm --rel
	link banner [op]
	ren banner.x=banner.prl

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
#include <string.h>
#include <conio.h>
#include <fileio.h>
#include <fprintf.h>
#include <redir.h>

#endif

#ifdef FOR_SAMARUX

/* SamaruX version
   ---------------
*/

#include <samarux.h>

#endif

int glyphs[84]; /* char *glyphs[84] */

init_glyps()
{
	glyphs[0]  =   "         @@@  @@   @@  @ @   @@@@@          @@     @@@  ";
	glyphs[1]  =   "         @@@  @@   @@  @ @  @  @  @@@   @  @  @    @@@  ";
	glyphs[2]  =   "         @@@   @   @ @@@@@@@@  @   @@  @    @@      @   ";
	glyphs[3]  =   "          @            @ @   @@@@@    @    @@@     @    ";
	glyphs[4]  =   "                     @@@@@@@   @  @  @    @   @ @       ";
	glyphs[5]  =   "         @@@           @ @  @  @  @ @  @@ @    @        ";
	glyphs[6]  =   "         @@@           @ @   @@@@@ @   @@  @@@@ @       ";
	glyphs[7]  =   "   @@    @@                                            @";
	glyphs[8]  =   "  @        @   @   @    @                             @ ";
	glyphs[9]  =   " @          @   @ @     @                            @  ";
	glyphs[10] =   " @          @ @@@@@@@ @@@@@   @@@   @@@@@           @   ";
	glyphs[11] =   " @          @   @ @     @     @@@                  @    ";
	glyphs[12] =   "  @        @   @   @    @      @            @@@   @     ";
	glyphs[13] =   "   @@    @@                   @             @@@  @      ";
	glyphs[14] =   "  @@@     @    @@@@@  @@@@@ @      @@@@@@@ @@@@@ @@@@@@@";
	glyphs[15] =   " @   @   @@   @     @@     @@    @ @      @     @@    @ ";
	glyphs[16] =   "@   @ @ @ @         @      @@    @ @      @          @  ";
	glyphs[17] =   "@  @  @   @    @@@@@  @@@@@ @@@@@@@ @@@@@ @@@@@@    @   ";
	glyphs[18] =   "@ @   @   @   @            @     @       @@     @  @    ";
	glyphs[19] =   " @   @    @   @      @     @     @ @     @@     @  @    ";
	glyphs[20] =   "  @@@   @@@@@ @@@@@@@ @@@@@      @  @@@@@  @@@@@   @    ";
	glyphs[21] =   " @@@@@  @@@@@          @@@      @           @     @@@@@ ";
	glyphs[22] =   "@     @@     @  @@@    @@@     @             @   @     @";
	glyphs[23] =   "@     @@     @  @@@           @     @@@@@     @        @";
	glyphs[24] =   " @@@@@  @@@@@@         @@@   @                 @     @@ ";
	glyphs[25] =   "@     @      @         @@@    @     @@@@@     @     @   ";
	glyphs[26] =   "@     @@     @  @@@     @      @             @          ";
	glyphs[27] =   " @@@@@  @@@@@   @@@    @        @           @       @   ";
	glyphs[28] =   " @@@@@    @   @@@@@@  @@@@@ @@@@@@ @@@@@@@@@@@@@@ @@@@@ ";
	glyphs[29] =   "@     @  @ @  @     @@     @@     @@      @      @     @";
	glyphs[30] =   "@ @@@ @ @   @ @     @@      @     @@      @      @      ";
	glyphs[31] =   "@ @ @ @@     @@@@@@@ @      @     @@@@@@  @@@@@  @  @@@@";
	glyphs[32] =   "@ @@@@ @@@@@@@@     @@      @     @@      @      @     @";
	glyphs[33] =   "@     @@     @@     @@     @@     @@      @      @     @";
	glyphs[34] =   " @@@@@ @     @@@@@@@  @@@@@ @@@@@@ @@@@@@@@       @@@@@ ";
	glyphs[35] =   "@     @  @*@        @@    @ @      @     @@     @@@@@@@@";
	glyphs[36] =   "@     @   @         @@   @  @      @@   @@@@    @@     @";
	glyphs[37] =   "@     @   @         @@  @   @      @ @ @ @@ @   @@     @";
	glyphs[38] =   "@@@@@@@   @         @@@@    @      @  @  @@  @  @@     @";
	glyphs[39] =   "@     @   @   @     @@  @   @      @     @@   @ @@     @";
	glyphs[40] =   "@     @   @   @     @@   @  @      @     @@    @@@     @";
	glyphs[41] =   "@     @  @@@   @@@@@ @    @ @@@@@@@@     @@     @@@@@@@@";
	glyphs[42] =   "@@@@@@  @@@@@ @@@@@@  @@@@@ @@@@@@@@     @@     @@     @";
	glyphs[43] =   "@     @@     @@     @@     @   @   @     @@     @@  @  @";
	glyphs[44] =   "@     @@     @@     @@         @   @     @@     @@  @  @";
	glyphs[45] =   "@@@@@@ @     @@@@@@@  @@@@@    @   @     @@     @@  @  @";
	glyphs[46] =   "@      @   @ @@   @        @   @   @     @ @   @ @  @  @";
	glyphs[47] =   "@      @    @ @    @ @     @   @   @     @  @ @  @  @  @";
	glyphs[48] =   "@       @@@@ @@     @ @@@@@    @    @@@@@    @    @@ @@ ";
	glyphs[49] =   "@     @@     @@@@@@@@ @@@@@ @       @@@@@    @          ";
	glyphs[50] =   " @   @  @   @      @  @      @          @   @ @         ";
	glyphs[51] =   "  @ @    @ @      @   @       @         @  @   @        ";
	glyphs[52] =   "   @      @      @    @        @        @               ";
	glyphs[53] =   "  @ @     @     @     @         @       @               ";
	glyphs[54] =   " @   @    @    @      @          @      @               ";
	glyphs[55] =   "@     @   @   @@@@@@@ @@@@@       @ @@@@@        @@@@@@@";
	glyphs[56] =   "  @@@                                                   ";
	glyphs[57] =   "  @@@     @@   @@@@@   @@@@  @@@@@  @@@@@@ @@@@@@  @@@@ ";
	glyphs[58] =   "   @     @  @  @    @ @    @ @    @ @      @      @    @";
	glyphs[59] =   "    @   @    @ @@@@@  @      @    @ @@@@@  @@@@@  @     ";
	glyphs[60] =   "        @@@@@@ @    @ @      @    @ @      @      @  @@@";
	glyphs[61] =   "        @    @ @    @ @    @ @    @ @      @      @    @";
	glyphs[62] =   "        @    @ @@@@@   @@@@  @@@@@  @@@@@@ @       @@@@ ";
	glyphs[63] =   "                                                        ";
	glyphs[64] =   " @    @    @        @ @    @ @      @    @ @    @  @@@@ ";
	glyphs[65] =   " @    @    @        @ @   @  @      @@  @@ @@   @ @    @";
	glyphs[66] =   " @@@@@@    @        @ @@@@   @      @ @@ @ @ @  @ @    @";
	glyphs[67] =   " @    @    @        @ @  @   @      @    @ @  @ @ @    @";
	glyphs[68] =   " @    @    @   @    @ @   @  @      @    @ @   @@ @    @";
	glyphs[69] =   " @    @    @    @@@@  @    @ @@@@@@ @    @ @    @  @@@@ ";
	glyphs[70] =   "                                                        ";
	glyphs[71] =   " @@@@@   @@@@  @@@@@   @@@@   @@@@@ @    @ @    @ @    @";
	glyphs[72] =   " @    @ @    @ @    @ @         @   @    @ @    @ @    @";
	glyphs[73] =   " @    @ @    @ @    @  @@@@     @   @    @ @    @ @    @";
	glyphs[74] =   " @@@@@  @  @ @ @@@@@       @    @   @    @ @    @ @ @@ @";
	glyphs[75] =   " @      @   @  @   @  @    @    @   @    @  @  @  @@  @@";
	glyphs[76] =   " @       @@@ @ @    @  @@@@     @    @@@@    @@   @    @";
	glyphs[77] =   "                       @@@     @     @@@   @@    @ @ @ @";
	glyphs[78] =   " @    @  @   @ @@@@@@ @        @        @ @  @  @ @ @ @ ";
	glyphs[79] =   "  @  @    @ @      @  @        @        @     @@ @ @ @ @";
	glyphs[80] =   "   @@      @      @  @@                 @@        @ @ @ ";
	glyphs[81] =   "   @@      @     @    @        @        @        @ @ @ @";
	glyphs[82] =   "  @  @     @    @     @        @        @         @ @ @ ";
	glyphs[83] =   " @    @    @   @@@@@@  @@@     @     @@@         @ @ @ @";
}

int main(argc, argv)
int argc, *argv; /* char *argv[] */
{
  int a, b, c, len, ind;
  char line[80], *p, *q;

  if(argc < 2) {
      fprintf(stderr, "Usage: banner text\n");
      return(0);
  }

  init_glyps();

  for (argv++; --argc; argv++) {
	p = *argv;
	len = strlen(p);
	if (len > 10) len = 10;
	for (a = 0; a < 7; a++) {
		for (b = 0; b < len; b++) {
			if ((ind = p[b] - ' ') < 0) ind = 0;
			for (c = 0; c < 7; c++) {
				q = glyphs[(ind / 8 * 7) + a];
				line[b * 8 + c] = q[(ind % 8 * 7) + c] == '@' ? ind + ' ' : ' ';
			}
			line[b * 8 + 7] = ' ';
		}
		for (b = len * 8 - 1; b >= 0; b--) {
			if (line[b] != ' ') break;
			line[b] = '\0';
		}
		puts(line);
	}
	putchar('\n');
  }
  return(0);
}

