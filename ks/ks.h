/**
 * @file
 * @brief Keyboard & screen functions library for CP/M & MESCC.
 *
 * The functionality of this library is similar to the curses / ncurses
 * libraries for Unix: a common interface for keyboard and screen devices
 * under CP/M.
 */

/*	ks.h

	Keyboard & screen functions library for CP/M & MESCC - Mike's Enhanced Small C Compiler.

	Copyright (c) 2016 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

	Revisions:

	03 Sep 2016 : Work begins.
	29 Sep 2016 : Rename functions.
	03 Oct 2016 : Reworked KsInit() and others. Added support for reverse and underline.

	Public:

	TO-DO
*/

/* Public defines
   --------------
*/

/* Private defines
   ---------------
*/

/* Private globals
   ---------------
*/
BYTE xks_rows;     // Screen rows
BYTE xks_cols;     // Screen columns
BYTE *xks_clrscr;  // Clear screen
BYTE *xks_poscur;  // Position cursor
BYTE *xks_shwcur;  // Show cursor
BYTE *xks_hidcur;  // Hide cursor
BYTE *xks_yrever;  // Reverse on
BYTE *xks_nrever;  // Reverse off
BYTE *xks_yuline;  // Underline on
BYTE *xks_nuline;  // Underline off

/**
 * @fn    int KsInit(char *tty_name)
 * @brief Initialize the KS library.
 *
 * Supported TTY names are:
 * - vt52
 * - vt100
 * - pcw
 * - cpc
 * - spectrum
 * - kaypro
 *
 * @param   tty_name - TTY name
 * @return  0 on success, -1 on failure
 */
KsInit(tty_name)
char *tty_name;
{
	xKsInit();

	if(!strcmp(tty_name, "vt52")) {
		xKsInit24x80();
		xks_clrscr = "\eH\eJ";
		xks_poscur = "A\eY%r%c";
	}
	else if(!strcmp(tty_name, "vt100")) {
		xks_rows   = 25;
		xks_cols   = 80;
		xks_clrscr = "\e[H\e[J";
		xks_poscur = "B\e[%r;%cH";
	}
	else if(!strcmp(tty_name, "pcw")) {
		xks_rows   = 31;
		xks_cols   = 90;
		xKsInitAmstrad();
	}
	else if(!strcmp(tty_name, "cpc")) {
		xKsInit24x80();
		xKsInitAmstrad();
	}
	else if(!strcmp(tty_name, "spectrum")) {
		xks_rows   = 23;
		xks_cols   = 51;
		xKsInitAmstrad();

		KsPutRawCh('\e'); KsPutRawCh('3'); KsPutRawCh(1);
	}
	else if(!strcmp(tty_name, "kaypro")) {
		xKsInit24x80();
		xks_clrscr = "?"; *xks_clrscr = 26; // ^Z
		xks_poscur = "A\e=%r%c";
	}
	else {
		return -1;
	}

	return 0;
}

KsExit()
{
	// Nothing yet
}

KsClrScr()
{
	KsPutRawStr(xks_clrscr);
}

KsPosCursor(row, col)
BYTE row, col;
{
	char *p; int v;

	for(p = xks_poscur + 1; *p; ++p) {
		if(*p != '%')
		{
			KsPutRawCh(*p);
		}
		else {
			v = (*(++p) == 'r' ? row : col);

			if(*xks_poscur == 'A') {
				// A
				KsPutRawCh(32 + v);
			}
			else {
				// B
				xKsPutDec(1 + v);
			}
		}
	}
}

KsShowCursor(toggle)
int toggle;
{
	if(xks_shwcur) {
		KsPutRawStr(toggle ? xks_shwcur : xks_hidcur);
	}
}

KsCanShowCursor()
{
	return xks_shwcur != NULL;
}

KsUnderline(toggle)
int toggle;
{
	if(xks_yuline) {
		KsPutRawStr(toggle ? xks_yuline : xks_nuline);
	}
}

KsCanUnderline()
{
	return xks_yuline != NULL;
}

KsReverse(toggle)
int toggle;
{
	if(xks_yrever) {
		KsPutRawStr(toggle ? xks_yrever : xks_nrever);
	}
}

KsCanReverse()
{
	return xks_yrever != NULL;
}

KsGetRows()
{
	return xks_rows;
}

KsGetCols()
{
	return xks_cols;
}

KsPutStr(s)
char *s;
{
	while(*s) {
		KsPutCh(*s++);
	}
}

KsPutRawStr(s)
char *s;
{
	while(*s) {
		KsPutRawCh(*s++);
	}
}


#asm

KsGetSt:
	call xKsConInSt
	ld   h,0
	ld   l,a
	ret

KsGetCh
	call xKsConIn
	ld   h,0
	ld   l,a
	ret

KsPutCh
	ld   a,l
	cp   10
	jr   nz,KsPutRawCh
	ld   l,13
	call KsPutRawCh
	ld   l,10

KsPutRawCh
	ld   c,l
	jr   xKsConOut

xKsConInSt
	jp 0
xKsConIn
	jp 0
xKsConOut
	jp 0

xKsInit
	ld   hl,(1)
	inc  hl
	inc  hl
	inc  hl
	ld   de,xKsConInSt
	ld   bc,9
	ldir
	ret

;xKsEsc
;	ld   c,27
;	jr   xKsConOut

;xKsEscRaw
;	call xKsEsc
;	pop  hl
;	pop  bc
;	push bc
;	push hl
;	jr   xKsConOut

#endasm

xKsInit24x80()
{
	xks_rows = 24;
	xks_cols = 80;
}

xKsInitAmstrad()
{
	xks_clrscr = "\eH\eE";
	xks_poscur = "A\eY%r%c";
	xks_shwcur = "\ee";
	xks_hidcur = "\ef";
	xks_yrever = "\ep";
	xks_nrever = "\eq";
	xks_yuline = "\er";
	xks_nuline = "\eu";
}

// FIXME : use sprintf if it's available

xKsPutDec(num)
int num;
{
	char buf[5];
	int i;

	for(i = 0; i < 6; ++i) {
		if(num > 9) {
			buf[i] = num % 10;
			num /= 10;
		}
		else {
			buf[i++] = num;
			break;
		}
	}

	do {
		KsPutRawCh('0' + buf[--i]);
	} while(i);
}


/* -- Non Kernel Functions --------------------- */

KsCenterStr(row, s)
int row; char *s;
{
	KsPosCursor(row, (xks_cols - strlen(s)) / 2);
	KsPutStr(s);
}