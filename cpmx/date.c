/*	date.c

	Unix-like date for CP/M.

	Get / set the system date & time.

	Copyright (c) 2014-2016 Miguel I. Garcia Lopez.

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Usage:

	date [[YYYY-MM-DD] hh:mm:ss]
	date [[YYYY/MM/DD] hh:mm:ss]

	Examples:

	date
	date 17:20:00
	date 2015-04-08 17:20:00

	Changes:

	07 Apr 2014 : 1.00 : 1st version for SamaruX.
	08 Apr 2014 : 1.01 : Add support to change the system date & time.
	29 Nov 2015 : 1.01 : Added support for builtin / external.
	01 Jun 2016 : 1.02 : Version for plain CP/M 3.
*/

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
*/
#include <conio.h>
#include <printf.h>
#include <ctype.h>
#include <clock.h>
#include <cpm.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "date"
#define APP_VERSION "v1.02 / 01 Jun 2016"
#define APP_COPYRGT "(c) 2014-2016 FloppySoftware"
#define APP_USAGE   "date [[YYYY-MM-DD] hh:mm:ss] or date [[YYYY/MM/DD] hh:mm:ss]"

/* Project globals
   ---------------
*/
unsigned char date_clk[CLOCK_LEN];
int date_dt[DATE_LEN];
char *date_days, *date_months;

/* The program
   -----------
*/
main(argc, argv)
int argc, argv[];
{
	int cpmver;

	/* Check CP/M version */

	cpmver = bdos_hl(BF_OSVER, NULL);

	if((cpmver & 0xF000) || ((cpmver & 0x00FF) < 0x30))
		Error("Need CP/M 3");

	/* Set up some values */

	date_days   = "SunMonTueWedThuFriSat";
	date_months = "JanFebMarAprMayJunJulAugSepOctNovDec";

	/* Check arguments */

	switch(argc)
	{
		case 1 : /* date */
			DatePrint();
			break;
		case 2 : /* date hh:mm:ss */
			DateSet(NULL, argv[1]);
			break;
		case 3 : /* date YYYY-MM-DD hh:mm:ss */
			DateSet(argv[1], argv[2]);
			break;
		default :
			Error("Bad arguments");
			break;
	}

	/* Success */

	return 0;
}

/* Print the system date & time
   ----------------------------
*/
DatePrint()
{
	char *p;

	/* Get system date & time */

	getdate(getclock(date_clk), date_dt);

	/* Print system date & time as:
	   Www Mmm dd hh:mm:ss yyyy
	   Fri Jan 11 11:59:51 2008 */

	/* Print dayweek name */

	p = date_days + date_dt[6] * 3; putchar(*p++); putchar(*p++); putchar(*p); putchar(' ');

	/* Print month name */

	p = date_months + date_dt[1] * 3; putchar(*p++); putchar(*p++); putchar(*p);

	/* Print the rest */

	printf(" %02d %02d:%02d:%02d %04d\n", date_dt[2], date_dt[3], date_dt[4], date_dt[5], date_dt[0]);
}

/* Set the system date and time
   ----------------------------
*/
DateSet(dts, tms)
char *dts, *tms;
{
	/* Get current system date & time */

	getdate(getclock(date_clk), date_dt);

	/* Date as YYYY-MM-DD */

	if(dts != NULL)
	{
		if(DateChk(dts, "####-##-##") || DateChk(dts, "####/##/##"))
		{
			date_dt[0] = DateVal(dts, 4);
			date_dt[1] = DateVal(dts + 5, 2) - 1;
			date_dt[2] = DateVal(dts + 8, 2);
		}
		else
			Error("Bad date");
	}

	/* Time as HH:MM:SS */

	if(DateChk(tms, "##:##:##"))
	{
		date_dt[3] = DateVal(tms, 2);
		date_dt[4] = DateVal(tms + 3, 2);
		date_dt[5] = DateVal(tms + 6, 2);
	}
	else
		Error("Bad time");

	/* Change system date & time */

	if(setdate(date_clk, date_dt) == NULL)
		Error("Bad date or time");

	setclock(date_clk);	
}

/* Check date field
   ----------------
*/
DateChk(str, mask)
char *str, *mask;
{
	while(*str)
	{
		if(*mask == '#')
		{
			if(!isdigit(*str))
				return 0;
		}
		else if(*mask != *str)
			return 0;

		++str; ++mask;
	}

	if(*mask)
		return 0;

	return 1;
}

/* Get date field value
   --------------------
*/
DateVal(str, digits)
char *str; int digits;
{
	int val;

	val = 0;

	while(digits--)
		val = val * 10 + (*str++ - '0');

	return val;
}

/* Print error and exit
   --------------------
*/
Error(msg)
char *msg;
{
	printf("%s: %s.\n", APP_NAME, msg);

	exit(-1);
}


