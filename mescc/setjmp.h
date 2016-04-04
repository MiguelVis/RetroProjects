/*	setjmp.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Non-local jumps.

	Copyright (c) 1999-2015 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

	21 Ago 2015 : Initial version.
	22 Ago 2015 : Changed jmp_buf from char to int.

	Public:
	
	int setjmp (jmp_buf env)
	void longjmp (jmp_buf env, int rv)
	
	Notes:
	
	Only one jmp_buf is allowed (sorry, typedef is not supported by MESCC).
	
	Call to setjmp is performed as:
	
	; setjmp(env);
	
	LD HL,(env)
	PUSH HL
	CALL setjmp
	POP BC
	
	Call to longjmp is performed as:
	
	; longjmp(env, 1);
	
	LD HL,(env)
	PUSH HL
	LD HL,1
	PUSH HL
	CALL longjmp
	POP BC
	POP BC
*/

#ifndef SETJMP_H

#define SETJMP_H

/* Defs. and globals
*/

#define jmp_buf int   /* Just something */

WORD setjmp_rt;       /* Return address */
WORD setjmp_sp;       /* SP */

/*	int setjmp (jmp_buf env)

	Save state information in env. Returns ZERO from direct call, NON-ZERO from a longjmp call.
*/

#asm
setjmp:
	pop  hl
	ld   (setjmp_rt), hl  ; return address
	ld   (setjmp_sp), sp  ; SP
	push hl
	ld   hl, 0
	ret
#endasm

/*	void longjmp (jmp_buf env, int rv)

	Restore state stored in env. Execution resumes after setjmp(), and returns the value of rv, that must be NON-ZERO.
*/

#asm
longjmp:
	pop  bc
	pop  hl              ; value to return
	ld   sp,(setjmp_sp)  ; SP
	ld   de,(setjmp_rt)  ; return address
	push de              ; simulate a call
	ret
#endasm

#endif

