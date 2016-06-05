/* cal.c - print calendar months */

/*	cal.c

	Taken from RetroBSD source (2.11BSD Unix port for embedded systems):

	http://retrobsd.org/

	Converted to MESCC by Miguel I. Garcia Lopez / FloppySoftware, Spain

	15 Dec 2014 : Version for CP/M.
	02 Apr 2015 : Version for SamaruX as external command.
	23 Aug 2015 : Minor changes.

	To build the CP/M version:

	cc cal
	zsm cal
	hexcom cal

	To build the SamaruX version:

	cc cal
	zmac cal.zsm --rel
	link cal [op]
	ren cal.x=cal.prl

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
#include <string.h>
#include <printf.h>
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

char *dayw;
unsigned int smon[12]; /* char *smon[12] */
char mon[13];
char string[432];

main(argc, argv)
int argc, argv[]; /* char *argv[] */
{
    int y, i, j;
    int m;

    if(argc < 2) {
        fprintf(stderr, "Usage: cal [month] year\n");
        return(0);
    }

    dayw = "Su Mo Tu We Th Fr Sa";

    smon[0] = "Jan";  smon[1] = "Feb";  smon[2] = "Mar";
    smon[3] = "Apr";  smon[4] = "May";  smon[5] = "Jun";
    smon[6] = "Jul";  smon[7] = "Aug";  smon[8] = "Sep";
    smon[9] = "Oct";  smon[10]= "Nov";  smon[11]= "Dec";

    mon[0] = 0;
    mon[1] = 31;  mon[2] = 29;  mon[3] = 31;  mon[4] = 30;
    mon[5] = 31;  mon[6] = 30;  mon[7] = 31;  mon[8] = 31;
    mon[9] = 30;  mon[10]= 31;  mon[11]= 30;  mon[12]= 31;

    if(argc == 2) {

	/*
	 *  print out complete year
	 */

	y = number(argv[1]);
	if(y<1 || y>9999)
		return badarg();
	printf("\n\n\n");
	printf("      %u\n", y);
	printf("\n");
	for(i=0; i<12; i+=3) {
		for(j=0; j<6*72; j++)
			string[j] = '\0';
		printf("      %s                    %s                    %s\n", smon[i], smon[i+1], smon[i+2]);
		printf("%s   %s   %s\n", dayw, dayw, dayw);
		cal(i+1, y, string, 72);
		cal(i+2, y, string+23, 72);
		cal(i+3, y, string+46, 72);
		for(j=0; j<6*72; j+=72)
			pstr(string+j, 72);
	}
	printf("\n\n\n");
	return(0);
    }

    /*
     *  print out just month
     */

    m = number(argv[1]);
    if(m<1 || m>12)
		return badarg();
    y = number(argv[2]);
    if(y<1 || y>9999)
		return badarg();
    printf("      %s %u\n", smon[m-1], y);
    printf("%s\n", dayw);
    cal(m, y, string, 24);
    for(i=0; i<6*24; i+=24)
        pstr(string+i, 24);
    return(0);
}

badarg()
{
    fprintf(stderr, "Bad argument"); return 1;
}

number(str)
char *str;
{
    int n, c;
    char *s;

    n = 0;
    s = str;
    while(c = *s++) {
        if(c<'0' || c>'9')
            return(0);
        n = n*10 + c-'0';
    }
    return(n);
}

pstr(str, n)
char *str; int n;
{
    int i;
    char *s;

    s = str;
    i = n;
    while(i--)
        if(*s++ == '\0')
            s[-1] = ' ';
    i = n+1;
    while(i--)
        if(*--s != ' ')
            break;
    s[1] = '\0';
    printf("%s\n", str);
}

cal(m, y, p, w)
int m, y; char *p; int w;
{
    int d, i;
    char *s;

    s = p;
    d = jan1(y);
    mon[2] = 29;
    mon[9] = 30;

    switch((jan1(y+1)+7-d)%7) {

    /*
     *  non-leap year
     */
    case 1:
        mon[2] = 28;
        break;

    /*
     *  1752
     */
    default:
        mon[9] = 19;
        break;

    /*
     *  leap year
     */
    case 2:
        ;
    }
    for(i=1; i<m; i++)
        d += mon[i];
    d %= 7;
    s += 3*d;
    for(i=1; i<=mon[m]; i++) {
        if(i==3 && mon[m]==19) {
            i += 11;
            mon[m] += 11;
        }
        if(i > 9)
            *s = i/10+'0';
        s++;
        *s++ = i%10+'0';
        s++;
        if(++d == 7) {
            d = 0;
            s = p+w;
            p = s;
        }
    }
}

/*
 *  return day of the week
 *  of jan 1 of given year
 */

jan1(yr)
int yr;
{
    int y, d;

/*
 *  normal gregorian calendar
 *  one extra day per four years
 */

    y = yr;
    d = 4+y+(y+3)/4;

/*
 *  julian calendar
 *  regular gregorian
 *  less three days per 400
 */

    if(y > 1800) {
        d -= (y-1701)/100;
        d += (y-1601)/400;
    }

/*
 *  great calendar changeover instant
 */

    if(y > 1752)
        d += 3;

    return(d%7);
}

