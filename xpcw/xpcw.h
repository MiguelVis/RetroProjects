/*	xpcw.h

	Graphical and keyboard functions for the Amstrad PCW and MESCC.

	Basic library.

	Copyright (c) 2015 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

	Changes:

	21 Jul 2015 : v1.00 : First version.
	12 Aug 2015 : v1.01 : Added X_BUFSIZE and X_BUFCOLS.
	17 Aug 2015 : v1.02 : Added HelloRsx() and X_SIGNATURE.
	14 Oct 2018 : Added X_VertLine, X_HorzLine.

	Notes:

	Needs the RSX xpcw.
*/

/* RSX functions
   -------------
*/
#define X_Hello     0
#define X_WriteEnv  1
#define X_ReadEnv   2
#define X_SetPixel  3
#define X_GetPixel  4
#define X_PutRC     5
#define X_GetRC     6
#define X_PutBmpRC  7
#define X_GetBmpRC  8
#define X_KeyMatrix 9
#define X_SetFont   10
#define X_SetTxAtt  11
#define X_SetTxRC   12
#define X_PrintCh   13
#define X_PrintRpt  14
#define X_VertLine  15
#define X_HorzLine  16

/* Draw modes
   ----------
*/
#define XM_SET    0
#define XM_CLEAR  1
#define XM_TOGGLE 2

/* Text attributes
   ---------------
*/
#define XA_NORMAL 0
#define XA_BOLD   1
#define XA_ITALIC 2
#define XA_ULINE  4
#define XA_LINK   8
#define XA_GRAY   16
#define XA_REV    32

/* Other
   -----
*/
#define X_BUFSIZE   720     /* RSX buffer size in bytes */
#define X_BUFCOLS   90      /* RSX buffer size in columns (8 bytes each) */
#define X_SIGNATURE 0xCACA  /* RSX signature for X_Hello function */

/* Globals
   -------
*/
extern WORD x_fun;    /* RSX function */
extern WORD x_par[5]; /* RSX function parameters */

#asm                  /* RSX data block */
x_dat:
	defb 9  ;Function
	defb 6  ;# of parameters (defw below, incl. fun.)
x_fun:
	defw 0  ;Subfunction
x_par:
	defw 0  ;Parameters
	defw 0
	defw 0
	defw 0
	defw 0
#endasm

/* Call to RSX
   -----------
   Returns something or nothing, according to the called function.
*/
#asm
x_call:
	ld c,60
	ld de,x_dat
	jp 5
#endasm

/* Check if the RSX is in memory
   -----------------------------
   Returns NZ if true, else Z
*/
HelloRsx()
{
	x_fun = X_Hello;

	return x_call() == X_SIGNATURE;
}
