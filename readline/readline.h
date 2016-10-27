/**
 * @file
 * @brief Line editing library for MESCC.
 *
 * Line editing library with basic or extended functionality.
 *
 * Supported #defines:
 * - RL_EXTENDED to enable the extended version.
 */

/*	readline.h

	Line editing library for MESCC.

	Copyright (c) 2016 Miguel Garcia / FloppySoftware

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 3, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Author's contact:

		www.floppysoftware.es
		cpm-connections.blogspot.com
		floppysoftware@gmail.com

	Revisions:

		26 Oct 2016 : Start.
		27 Oct 2016 : Added extended mode.
*/

#ifndef READLINE_H

#define READLINE_H

/* Public defines
   --------------
*/
#ifdef RL_EXTENDED

// Extended modes
#define RL_NORMAL    0
#define RL_UCASE     1
#define RL_LCASE     2
#define RL_PASSWORD  4
#define RL_DECIMAL   8
#define RL_RESET     16

#endif

/**
 * @fn    char *ReadLine(char *buf, int width)
 * @brief Read text line.
 *
 * Supported editing characters are:
 * - 0x08 / BS, 0x7F / DEL: delete character on the left
 * - 0x0D / CR, 0x0A / LF:  end line
 *
 * @param   buf - buffer; can hold an initial text; must be at last of 'width + 1' characters in size
 * @param   width - max. length of line in characters
 * @return  pointer to buf
 */
ReadLine(buf, width)
char *buf;
int width;
{
#ifdef RL_EXTENDED
	return ReadLineEx(buf, width, RL_NORMAL);
#else
	int len, ch;

	putstr(buf); len = strlen(buf);

	for(;;)
	{
		switch(ch = getch())
		{
			case 0x08 : /* BS  */
			case 0x7F : /* DEL */
				if(len)
				{
					putchar(8); putchar(' '); putchar(8); --len;
				}
				break;

			//case 0x15 : /* Ctrl-U : Delete the line */
			//case 0x18 : /* Ctrl-X : Delete the line */
			//	break;

			case '\r' : /* CR */
			case '\n' : /* LF */
				buf[len] = 0;
				return buf;

			default :
				if(len < width && ch >= ' ')
					putchar(buf[len++] = ch);
				break;
		}
	}
#endif
}

#ifdef RL_EXTENDED

/**
 * @fn    char *ReadLineEx(char *buf, int width, mode)
 * @brief Read text line (extended version).
 *
 * Supported editing characters are:
 * - 0x08 / BS, 0x7F / DEL: delete character on the left
 * - 0x0D / CR, 0x0A / LF:  end line
 *
 * Supported editing modes are:
 * - RL_NORMAL: normal mode
 * - RL_UCASE: convert input characters to upper case
 * - RL_LCASE: convert input characters to lower case
 * - RL_PASSWORD: print input characters as asterisks
 * - RL_DECIMAL: accept only decimal digits as input
 * - RL_RESET: discard buffer contents on entry
 *
 * Editing modes can be ORed - ie: RL_UCASE | RL_PASSWORD
 *
 * @param   buf - buffer; can hold an initial text; must be at last of 'width + 1' characters in size
 * @param   width - max. length of line in characters
 * @param   mode - editing modes
 * @return  pointer to buf
 */
ReadLineEx(buf, width, mode)
char *buf;
int width;
unsigned int mode;
{
	int len, ch;

	if(mode & RL_RESET)
		*buf = '\0';
	else
		putstr(buf);

	len = strlen(buf);

	for(;;)
	{
		switch(ch = getch())
		{
			case 0x08 : /* BS  */
			case 0x7F : /* DEL */
				if(len)
				{
					putchar(8); putchar(' '); putchar(8); --len;
				}
				break;

			//case 0x15 : /* Ctrl-U : Delete the line */
			//case 0x18 : /* Ctrl-X : Delete the line */
			//	break;

			case '\r' : /* CR */
			case '\n' : /* LF */
				buf[len] = 0;
				return buf;

			default :
				if(len < width && ch >= ' ')
				{
					/* Only decimal digits? */
					if(mode & RL_DECIMAL)
					{
						if(!isdigit(ch))
							break;
					}

					/* Convert to upper or lower case? */
					if(mode & RL_UCASE)
						ch = toupper(ch);
					else if(mode & RL_LCASE)
						ch = tolower(ch);

					/* Store the character */
					buf[len++] = ch;

					/* Write the character to the console */
					putchar(mode & RL_PASSWORD ? '*' : ch);
				}
				break;
		}
	}
}

#endif

#endif

