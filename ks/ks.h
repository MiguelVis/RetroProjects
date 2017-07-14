/**
 * @file
 * @brief Keyboard & screen functions library for CP/M & MESCC.
 *
 * This library offers a common interface for keyboard and screen devices
 * under CP/M.
 *
 * Supported TTYs:
 * - KS_VT52      for generic VT52   80x24
 * - KS_VT100     for generic VT100  80x25
 * - KS_PCW       for Amstrad PCW    90x31
 * - KS_CPC       for Amstrad CPC    24x80 under CP/M Plus
 * - KS_SPECTRUM  for Spectrum +3    51x23
 * - KS_KAYPRO    for Kaypro         24x80
 *
 * Capabilities:
 * - KS_CAN_HIDE      can hide & show cursor
 * - KS_CAN_REVERSE   can reverse text
 * - KS_CAN_UNDERLINE can underline text
 */

/*	ks.h

	Keyboard & screen functions library for CP/M & MESCC - Mike's Enhanced Small C Compiler.

	Copyright (c) 2016, 2017 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

		03 Sep 2016 : Work begins.
		29 Sep 2016 : Rename functions.
		03 Oct 2016 : Reworked KsInit() and others. Added support for reverse and underline.
		14 Jul 2017 : Added TTY names.
*/

/* Public defines
   --------------
*/
#define KS_VT52     0
#define KS_VT100    1
#define KS_PCW      2
#define KS_CPC      3
#define KS_SPECTRUM 4
#define KS_KAYPRO   5

#define KS_CAN_HIDE      0 // Can hide & show cursor
#define KS_CAN_REVERSE   1 // Can reverse text
#define KS_CAN_UNDERLINE 2 // Can underline text

/* Private defines
   ---------------
*/
#define XKS_TTYS    6  // Number of supported TTYs

/* Private globals
   ---------------
*/
WORD xks_names[XKS_TTYS];  // TTY names by code -- char *xks_names[]

int xks_rows;      // Screen rows
int xks_cols;      // Screen columns
BYTE *xks_clrscr;  // Clear screen
BYTE *xks_poscur;  // Position cursor
BYTE *xks_shwcur;  // Show cursor
BYTE *xks_hidcur;  // Hide cursor
BYTE *xks_yrever;  // Reverse on
BYTE *xks_nrever;  // Reverse off
BYTE *xks_yuline;  // Underline on
BYTE *xks_nuline;  // Underline off

/**
 * @fn     int KsHello(char *tty_name)
 * @brief  Initialize the KS library.
 * Call this function before any other function (with a few exceptions).
 * @param  code - TTY code
 * @return 0 on success, -1 on failure
 */
KsHello(code)
int code;
{
	// Setup BIOS jumps, etc.
	xKsInit();

	// Setup TTY
	switch(code) {
		case KS_VT52 :
			xKsInit24x80();
			xks_clrscr = "\eH\eJ";
			xks_poscur = "A\eY%r%c";
			break;

		case KS_VT100 :
			xks_rows   = 25;
			xks_cols   = 80;
			xks_clrscr = "\e[H\e[J";
			xks_poscur = "B\e[%r;%cH";
			break;

		case KS_PCW :
			xks_rows   = 31;
			xks_cols   = 90;
			xKsInitAmstrad();
			break;

		case KS_CPC :
			xKsInit24x80();
			xKsInitAmstrad();
			break;

		case KS_SPECTRUM :
			xks_rows   = 23;
			xks_cols   = 51;
			xKsInitAmstrad();

			KsPutRawCh('\e'); KsPutRawCh('3'); KsPutRawCh(1);
			break;

		case KS_KAYPRO :
			xKsInit24x80();
			xks_clrscr = "?"; *xks_clrscr = 26; // ^Z
			xks_poscur = "A\e=%r%c";
			break;

		default :
			// Unknown tty code
			return -1;
	}

	// Success
	return 0;
}

/**
 * @fn    void KsBye(void)
 * @brief Reset the KS library.
 * Call this function before you stop your program, in order to reset the TTY.
 */
KsBye()
{
	// Nothing yet
}

/**
 * @fn     int KsGetCode(char *name)
 * @brief  Get the code for a TTY name.
 * This function can be called before KsHello().
 * @param  name - TTY name
 * @return TTY code if found, or -1 on unknown name
 */
KsGetCode(name)
char *name;
{
	int i;

	if(!xks_names[0]) {
		xKsNames();
	}

	for(i = 0; i < XKS_TTYS; ++i) {
		if(!strcmp(name, xks_names[i])) {
			// Found
			return i;
		}
	}

	// Unknown tty name
	return -1;
}

/**
 * @fn     int KsGetName(int code)
 * @brief  Get the name for a TTY code.
 * This function can be called before KsHello().
 * @param  code - TTY code
 * @return TTY name, or NULL on unknown code
 */
KsGetName(code)
int code;
{
	if(!xks_names[0]) {
		xKsNames();
	}

	if(code >= 0 && code < XKS_TTYS) {
		// Found
		return xks_names[code];
	}

	// Unknown tty code
	return NULL;
}

/**
 * @fn     void KsClear(void)
 * @brief  Clear the screen and move the cursor to 0,0.
 */
KsClear()
{
	KsPutRawStr(xks_clrscr);
}

/**
 * @fn     void KsPosCursor(int row, int col)
 * @brief  Move the cursor on screen.
 * @param  row - screen row from 0
 * @param  col - screen column from 0
 */
KsPosCursor(row, col)
int row, col;
{
	char *p; int v;

	for(p = xks_poscur + 1; *p; ++p) {
		if(*p != '%') {
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

/**
 * @fn     int KsCan(int cap)
 * @brief  Check if the TTY has a capability.
 * @param  cap - capatility code
 * @return 0 on NO, other on YES
 */
KsCan(cap)
int cap;
{
	switch(cap) {
		case KS_CAN_HIDE      : return xks_shwcur != NULL;
		case KS_CAN_REVERSE   : return xks_yrever != NULL;
		case KS_CAN_UNDERLINE : return xks_yuline != NULL;
	}

	return 0;
}

/**
 * @fn     void KsSetCursor(int toggle)
 * @brief  Show or hide the cursor on screen.
 * @param  toggle - 0 to hide, other to show
 */
KsSetCursor(toggle)
int toggle;
{
	if(xks_shwcur) {
		KsPutRawStr(toggle ? xks_shwcur : xks_hidcur);
	}
}

/**
 * @fn     void KsUnderline(int toggle)
 * @brief  Set or reset underline text.
 * If the TTY does not have this capability, does nothing.
 * @param  toggle - 0 to reset, other to set
 */
KsUnderline(toggle)
int toggle;
{
	if(xks_yuline) {
		KsPutRawStr(toggle ? xks_yuline : xks_nuline);
	}
}

/**
 * @fn     void KsReverse(int toggle)
 * @brief  Set or reset reverse text.
 * If the TTY does not have this capability, does nothing.
 * @param  toggle - 0 to reset, other to set
 */
KsReverse(toggle)
int toggle;
{
	if(xks_yrever) {
		KsPutRawStr(toggle ? xks_yrever : xks_nrever);
	}
}

/**
 * @fn     int KsGetRows(void)
 * @brief  Get TTY rows.
 * @return  TTY rows.
 */
KsGetRows()
{
	return xks_rows;
}

/**
 * @fn     int KsGetCols(void)
 * @brief  Get TTY columns.
 * @return  TTY columns.
 */
KsGetCols()
{
	return xks_cols;
}

/**
 * @fn     void KsPutRawCh(int ch)
 * @brief  Send a character to the TTY.
 * @param  ch - character
 */
#asm
KsPutRawCh
	ld   c,l
	jp   xKsConOut
#endasm

/**
 * @fn     void KsPutRawStr(char *s)
 * @brief  Send a string to the TTY using KsPutRawCh().
 * @param  s - string
 */
KsPutRawStr(s)
char *s;
{
	while(*s) {
		KsPutRawCh(*s++);
	}
}

/**
 * @fn     void KsPutCh(int ch)
 * @brief  Send a character to the TTY.
 * This functions performs the following translations:
 * - '\n' to '\r' + '\n'
 * @param  ch - character
 */
#asm
KsPutCh
	ld   a,l
	cp   10
	jp   nz,KsPutRawCh
	ld   l,13
	call KsPutRawCh
	ld   l,10
	jp   KsPutRawCh
#endasm

/**
 * @fn     void KsPutStr(char *s)
 * @brief  Send a string to the TTY using KsPutCh().
 * @param  s - string
 */
KsPutStr(s)
char *s;
{
	while(*s) {
		KsPutCh(*s++);
	}
}


/**
 * @fn     int KsGetKb(void)
 * @brief  Check if there is an input character from the keyboard.
 * @return 0 for NO, other for YES
 */
#asm
KsGetKb:
	call xKsConInSt
	ld   h,a
	ld   l,a
	ret
#endasm

/**
 * @fn     int KsGetCh(void)
 * @brief  Get character from the keyboard, waiting for one if necessary.
 * @return character
 */
#asm
KsGetCh
	call xKsConIn
	ld   h,0
	ld   l,a
	ret
#endasm

/* Private functions
   -----------------
*/

// void xKsInit(void) : Set BIOS JUMPs.
#asm
xKsInit
	ld   hl,(1)
	inc  hl
	inc  hl
	inc  hl
	ld   de,xKsConInSt
	ld   bc,9
	ldir
	ret

xKsConInSt
	jp 0  ; BIOS ConSt
xKsConIn
	jp 0  ; BIOS ConIn
xKsConOut
	jp 0  ; BIOS ConOut
#endasm

// void xKsNames(void) : Set TTY names.
xKsNames()
{
	xks_names[KS_VT52]     = "vt52";
	xks_names[KS_VT100]    = "vt100";
	xks_names[KS_PCW]      = "pcw";
	xks_names[KS_CPC]      = "cpc";
	xks_names[KS_SPECTRUM] = "spectrum";
	xks_names[KS_KAYPRO]   = "kaypro";
}

// void xKsInit24x80(void) : Set TTY to 24 rows and 80 columns.
xKsInit24x80()
{
	xks_rows = 24;
	xks_cols = 80;
}

// void xKsInitAmstrad(void) : Set generic Amstrad mode.
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

// void xKsPutDec(int num) : Send positive decimal number to TTY.
xKsPutDec(num)
int num;
{
#ifdef SPRINTF_H
	char buf[6];

	sprintf(buf, "%d", num);

	KsPutRawStr(buf);
#else
	char buf[6];
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
#endif
}


