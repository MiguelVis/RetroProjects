/*	kslib.h

	Keyboard & screen functions library for CP/M & MESCC - Mike's Enhanced Small C Compiler.

	Copyright (c) 2012 Miguel I. Garcia Lopez, Valencia, Spain.

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

	Revisions:

	02 Jul 2012 : Version 1.0 for 24x80 VT52 screen.

	Functions:

	int KbdScrStart(void)
	int KbdScrEnd(void)
	int KbdIn(void)
	int KbdChk(void)
	int KbdStat(void)
	void ScrOut(ch)
	void ScrOutStr(str)
	void ScrSetRC(row, col)
	void ScrCurOn(void)
	void ScrCurOff(void)
	void ScrClr(void)
*/

#define SCR_ROWS 24
#define SCR_COLS 80

#asm

xDirConInSt:
	jp 0
xDirConIn:
	jp 0
xDirConOut:
	jp 0

KbdScrStart:
	ld hl,(1)
	inc hl
	inc hl
	inc hl		;HL = JP CONST (BIOS VECTOR)
	ld de,xDirConInSt
	ld bc,9
	ldir
	ld hl,0		;Success
	ret

KbdScrEnd:
	ld hl,0		;Success
	ret

KbdIn:
	call xDirConIn
KbdIn2:
	ld h,0
	ld l,a
	ret

KbdChk:
	call xDirConInSt
	ld h,0
	ld l,a
	ret

KbdStat:
	call xDirConInSt
	or a
	jr nz,KbdIn
	jr KbdIn2

ScrOut:
	ld c,l
	jp xDirConOut

ScrOutStr:
	ld a,(hl)
	or a
	ret z
	push hl
	ld c,a
	call xDirConOut
	pop hl
	inc hl
	jr ScrOutStr

ScrSetRC:
	call xScrEsc
	ld c,'Y'
	call xDirConOut
	pop bc
	pop de
	pop hl
	push hl
	push de
	push bc
	push de
	ld a,32
	add a,l
	ld c,a
	call xDirConOut
	pop hl
	ld a,32
	add a,l
	ld c,a
	jp xDirConOut

ScrCurOn:
	call xScrEsc
	ld c,'e'
	jp xDirConOut

ScrCurOff:
	call xScrEsc
	ld c,'f'
	jp xDirConOut

ScrClr:
	call xScrEsc
	ld c,'H'
	call xDirConOut
	call xScrEsc
	ld c,'E'
	jp xDirConOut

xScrEsc:
	ld c,27
	jp xDirConOut

#endasm
