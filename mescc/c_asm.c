/*	c_asm.c

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Assembler code output module.

	Copyright (c) 1999-2015 Miguel I. Garcia Lopez, FloppySoftware.

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

	20 Dec 2006 : Last revision.
	16 Apr 2007 : GPL'd.
	26 May 2007 : Solved bug in a_incdec() - assumed wrongly that
	              lval[0] never is 0.
	17 Feb 2015 : Solved bug in getvloc() and putvloc() - some systems
                      can corrupt 1 byte in variable area (stack).
	10 Apr 2015 : Removed code for argc & argv support via the -A option.
	26 Oct 2015 : Cleaned.
	28 Oct 2015 : Now ;[ is for optimizer off and ;] for optimizer on.
	22 Nov 2015 : New function wrtequ().
	01 Dec 2015 : Print a SPACE on line starts, instead of a TAB.
*/

/*	void a_start(void)

	Start assembler code.
*/

a_start()
{
	comment();
	fo_str(VERSION);
	fo_nl();

	if(typout==OUT_PRG)
	{
		/* Define stack size */

		fo_str("ccSTACKSIZE EQU ");fo_dec(stksize);fo_nl();
	}
	else if(typout==OUT_LIB)
		fo_line("#asm");
}

/*	void a_end(void)

	End assembler code.
*/

a_end()
{
	if(typout==OUT_PRG)
		fo_line("ccfreemem: DEFB 0");
	else if(typout==OUT_LIB)
		fo_line("#endasm");

	if(typout!=OUT_LIB)
		a_code("END");
}

/*	void a_opt(on)

	Code for optimizer.
*/

a_opt(on)
int on;
{
	fo_line(on ? ";]" : ";[");
}

a_code(s)
char *s;
{
	fo_ch(' ');fo_line(s);
}

a_tcode(s)
char *s;
{
	fo_ch(' ');fo_str(s);
}

a_pcode(s)
char *s;
{
	fo_ch('(');fo_str(s);fo_ch(')');
}

a_pcodenl(s)
char *s;
{
	a_pcode(s);fo_nl();
}

/*	void comment(void)

	Start of comment.
*/
	
comment()
{
	fo_ch(';');
}

/*	void getmem(char *sym)

	HL = (global var)
*/

getmem(sym)
char *sym;
{
	if((sym[SY_TYPE] & TY_CHAR) && sym[SY_IDENT]!=ID_PTR)
	{
		a_tcode(sym[SY_TYPE]==TY_CHAR ? "LD A," : "LD HL,");
		a_pcodenl(sym+SY_NAME);

		sym[SY_TYPE]==TY_CHAR ? callrt("sxt") : a_code("LD H,0");
	}
	else
	{
		a_tcode("LD HL,");a_pcodenl(sym+SY_NAME);
	}
}

getvloc(sym)
char *sym;
{
	int offset, sizflg;

	offset=(sym[SY_OFFSET] & 255) + (sym[SY_OFFSET+1] << 8) - sptr;

	sizflg=offset < 256;

	if((sym[SY_TYPE] & TY_CHAR) && sym[SY_IDENT]!=ID_PTR)
	{
		if(sym[SY_TYPE]==TY_CHAR)
			callrt(sizflg ? "xgc" : "xgc2");
		else if(offset)
			callrt(sizflg ? "xgb" : "xgb2");
		else
		{
			a_code("POP HL");
			a_code("PUSH HL");
			a_code("LD H,0");
			return;
		}
	}
	else if(sizflg)
	{
		switch(offset)
		{
			case 0:	a_code("POP HL");
				a_code("PUSH HL");
				return;
			/**************************

			NOTE: FOLLOWING CODE CAN CAUSE 1 BYTE OF LOCAL VARIABLES TO
			BE CORRUPTED, IF AN INTERRUPT OCCURS BETWEEN INC SP AND DEC SP
			AND THE SYSTEM PUTS SOMETHING IN THE PROGRAM STACK (AS DOES
			CP/M PLUS ON THE AMSTRAD PCW).

			case 1:	a_code("INC SP");
				a_code("POP HL");
				a_code("PUSH HL");
				a_code("DEC SP");
				return;
			***************************/
			case 2:	a_code("POP BC");
				a_code("POP HL");
				a_code("PUSH HL");
				a_code("PUSH BC");
				return;
		}

		callrt("xgw");
	}
	else
		callrt("xgw2");

	sizflg ? defbyte() : defword();
	fo_dec(offset);
	fo_nl();
}


/*	void getloc(char *sym)

	HL = local var. adress
*/

getloc(sym)
char *sym;
{
	immed_dec((sym[SY_OFFSET] & 255) + (sym[SY_OFFSET+1] << 8) - sptr);
	a_code("ADD HL,SP");
}

/*	void putmem(char *sym)

	(global var) = HL
*/

putmem(sym)
char *sym;
{
	if((sym[SY_TYPE] & TY_CHAR) && sym[SY_IDENT]!=ID_PTR)
	{
		a_code("LD A,L");
		a_tcode("LD ");a_pcode(sym+SY_NAME);fo_line(",A");
	}
	else
	{
		a_tcode("LD ");a_pcode(sym+SY_NAME);fo_line(",HL");
	}
}

putvloc(sym)
char *sym;
{
	int offset, sizflg;

	offset=(sym[SY_OFFSET] & 255) + (sym[SY_OFFSET+1] << 8) - sptr;

	sizflg=offset < 256;

	if((sym[SY_TYPE] & TY_CHAR) && sym[SY_IDENT]!=ID_PTR)
	{
		if(offset < 2)
		{
			a_code("POP BC");
			a_code(offset ? "LD B,L" : "LD C,L");
			a_code("PUSH BC");
			return;
		}

		callrt(sizflg ? "xpb" : "xpb2");
	}
	else if(sizflg)
	{
		switch(offset)
		{
			case 0:	a_code("POP BC");
				a_code("PUSH HL");
				return;

			/***************************

			NOTE: FOLLOWING CODE CAN CAUSE 1 BYTE OF LOCAL VARIABLES TO
			BE CORRUPTED, IF AN INTERRUPT OCCURS BETWEEN INC SP AND DEC SP
			AND THE SYSTEM PUTS SOMETHING IN THE PROGRAM STACK (AS DOES
			CP/M PLUS ON THE AMSTRAD PCW).

			case 1:	a_code("INC SP");
				a_code("POP BC");
				a_code("PUSH HL");
				a_code("DEC SP");
				return;
			*******************************/
			case 2:	a_code("POP BC");
				a_code("POP DE");
				a_code("PUSH HL");
				a_code("PUSH BC");
				return;
		}

		callrt("xpw");
	}
	else
		callrt("xpw2");

	sizflg ? defbyte() : defword();
	fo_dec(offset);
	fo_nl();
}


/*	void putstk(char obj)

	Store specified object type in HL at address
	on the top of the stack.
*/

putstk(obj)
char obj;
{
	fpop();

	if(obj & TY_CHAR)
	{
		a_code("LD A,L");
		a_code("LD (DE),A");
	}
	else
		callrt("pw");
}

/*	void indrct(char obj)

	Fetch specified object type indirect through HL into HL.
*/

indrct(obj)
char obj;
{
	if(obj==TY_UCHAR)
	{
		a_code("LD L,(HL)");
		a_code("LD H,0");
	}
	else
		callrt(obj & TY_CHAR ? "gc" : "gw");
}

/*	void swap(void)

	Swap HL and DE.
*/

swap()
{
	a_code("EX DE,HL");
}

/*	void immed(void)

	Start assembler code for loading HL.
*/

immed()
{
	a_tcode("LD HL,");
}


/*	void immed_str(char *str)

	Load HL with offset.
*/

immed_str(str)
char *str;
{
	immed();fo_line(str);
}

/*	void immed_dec(dec)

	Load HL with decimal value.
*/

immed_dec(dec)
int dec;
{
	immed();fo_dec(dec);fo_nl();
}

/*	void fpush(void)

	Push HL into stack.
*/

fpush()
{
	a_code("PUSH HL");
	sptr-=2;
}

/*	void fpop(void)

	Pop DE from stack.
*/

fpop()
{
	a_code("POP DE");
	sptr+=2;
}

/*	void swapstk(void)

	Swap HL and stack content.
*/

swapstk()
{
	a_code("EX (SP),HL");
}

a_label(num)
int num;
{
	fo_ch(letlab);fo_dec(num);
}

a_lnlabel(num)
int num;
{
	a_label(num);fo_nl();
}

/*	void callrt(char *label)

	Call runtime label.
*/

callrt(label)
char *label;
{
	a_tcode("CALL cc");fo_line(label);
}

/*	void callfn(char *label)

	Call function label.
*/

callfn(label)
char *label;
{
	a_tcode("CALL ");fo_line(label);
}

/*	void execsw(int count, int label, int endlab)

	Execute switch command in runtime.
*/

execsw(count,label,endlab)
int count,label,endlab;
{
	a_tcode("LD DE,");a_lnlabel(label);
	a_tcode("LD HL,");a_lnlabel(endlab);
	a_tcode("LD B,");fo_dec(count);fo_nl();
	a_code("JP ccswtch");
}

/*	void retfn(void)

	Return from function.
*/

retfn()
{
	a_code("RET");
}

/*	void callstk(void)

	Perform subroutine call to address on top of stack.
*/

callstk()
{
	immed_str("5+$");		/* MGL 20 Dec 03 - ZSM error if $+5 */
	swapstk();
	a_code("JP (HL)");
	sptr+=2;
}

/*	void jump(int label)

	Jump to local label.
*/

jump(label)
int label;
{
	a_tcode("JP ");a_lnlabel(label);
}

/*	void testjmp(int label)

	Jump to local label if HL == 0.
*/

testjmp(label)
int label;
{
	a_code("LD A,H");
	a_code("OR L");
	a_tcode("JP Z,");a_lnlabel(label);
}

/*	void defbyte(void)

	Start assembler command defb.
*/

defbyte()
{
	a_tcode("DEFB ");
}

/*	void defword(void)

	Start assembler command defw.
*/

defword()
{
	a_tcode("DEFW ");
}

/*	void defstrg(void)

	Start assembler command defs.
*/

defstrg()
{
	a_tcode("DEFS ");
}

/*	void wrtequ(void)

	Write assembler command equ.
*/

wrtequ()
{
	a_tcode("EQU ");
}

/*	int modstk(int newsp, int flag)

	Modify stack pointer value. Return newsp.
	If flag!=0 preserve hl.
*/

modstk(newsp, flag)
int newsp, flag;
{
	int k, m;

	k=newsp-sptr;
	m=flag ? 14 : 10;

	if(k>0 && k<m)
	{
		if(k & 1)
		{
			a_code("INC SP");
			--k;
		}
		while(k)
		{
			a_code("POP BC");
			k-=2;
		}
	}
	else if(k<0 && k>-m)
	{
		if(k & 1)
		{
			a_code("DEC SP");
			++k;
		}
		while(k)
		{
			a_code("PUSH BC");
			k+=2;
		}
	}
	else if(k)
	{
		if(flag)
			swap();
		immed_dec(k);
		a_code("ADD HL,SP");
		a_code("LD SP,HL");
		if(flag)
			swap();
	}

	return newsp;
}

/*	void doubreg(void)

	HL = HL + HL.
*/

doubreg()
{
	a_code("ADD HL,HL");
}

a_incdec(lval, inc, post)
int lval[], inc, post;
{
	char *ptr;
	int i, dbl;

	ptr=lval[0];

	if((ptr && (ptr[SY_TYPE] & TY_CHAR) && ptr[SY_IDENT]!=ID_PTR)
		|| (lval[1] & TY_CHAR))
	{
		if(!lval[1])
			ptr[SY_STORAGE]==ST_STATIK ? immed_str(ptr+SY_NAME) : getloc(ptr);
		if(post)
			a_code("LD A,(HL)");

		a_code(inc ? "INC (HL)" : "DEC (HL)");

		if(lval[1]==TY_CHAR || (!lval[1] && ptr[SY_TYPE]==TY_CHAR))
			callrt(post ? "sxt" : "gc");
		else
		{
			a_code(post ? "LD L,A" : "LD L,(HL)");
			a_code("LD H,0");
		}
	}
	else
	{
		i=dbl=((lval[2] & TY_INT) && (!lval[1] || (!lval[1] && ptr[SY_IDENT]==ID_PTR))) ? 2 : 1;
		if(lval[1])
			fpush();
		rvalue(lval);
		while(dbl--)
			inc ? finc() : fdec();
		store(lval);
		if(post)
		{
			while(i--)
				inc ? fdec() : finc();
		}
	}
}

/*	void addfn(void)

	HL = DE + HL
*/

addfn()
{
	a_code("ADD HL,DE");
}

/*	void subfn(void)

	HL = DE - HL
*/

subfn()
{
	callrt("sub");
}

/*	void a_mul(int sign)

	HL = DE * HL
*/

a_mul(sign)
int sign;
{
	callrt(sign ? "mul" : "umul");
}

/*	void a_div(int sign)

	HL = DE / HL
*/

a_div(sign)
int sign;
{
	callrt(sign ? "div" : "udiv");
}

/*	void a_mod(int sign)

	HL = DE % HL
*/

a_mod(sign)
int sign;
{
	callrt(sign ? "mod" : "umod");
}

/*	void orfn(void)

	HL = DE | HL	(BINARY OR)
*/

orfn()
{
	callrt("or");
}

/*	void xorfn(void)

	HL = DE ^ HL	(BINARY XOR)
*/

xorfn()
{
	callrt("xor");
}

/*	void andfn(void)

	HL = DE & HL	(BINARY AND)
*/

andfn()
{
	callrt("and");
}

/*	void a_sr(int sign)

	HL = DE >> HL
*/

a_sr(sign)
int sign;
{
	callrt(sign ? "asr" : "uasr");
}

/*	void a_sl(int sign)

	HL = DE << HL
*/

a_sl(sign)
int sign;
{
	callrt(sign ? "asl" : "uasl");
}

/*	void negfn(void)

	HL = -HL	(TWO COMPLEMENT)
*/

negfn()
{
	callrt("neg");
}

/*	void comfn(void)

	HL = ~HL	(ONE COMPLEMENT)
*/

comfn()
{
	callrt("com");
}

/*	void finc(void)

	HL = HL + 1
*/

finc()
{
	a_code("INC HL");
}

/*	void fdec(void)

	HL = HL - 1
*/

fdec()
{
	a_code("DEC HL");
}

/*	Logical and conditional operators.

	Perform DE with HL comparison, returning in HL:

	Non zero value on true condition.
	Zero value on false condition.
*/

/*	void eq(void)

	HL = DE == HL
*/

eq()
{
	callrt("eq");
}

/*	void ne(void)

	HL = DE != HL
*/

ne()
{
	callrt("ne");
}

/*	void a_lt(int sign)

	HL = DE < HL
*/

a_lt(sign)
int sign;
{
	callrt(sign ? "lt" : "ult");
}

/*	void a_le(int sign)

	HL = DE <= HL
*/

a_le(sign)
int sign;
{
	callrt(sign ? "le" : "ule");
}

/*	void a_gt(int sign)

	HL = DE > HL
*/

a_gt(sign)
int sign;
{
	callrt(sign ? "gt" : "ugt");
}

/*	void a_ge(int sign)

	HL = DE >= HL
*/

a_ge(sign)
int sign;
{
	callrt(sign ? "ge" : "uge");
}

/*	void lgandfn(void)

	HL = DE && HL	(LOGIC AND)
*/

lgandfn()
{
	callrt("lgand");
}

/*	void lgorfn(void)

	HL = DE || HL	(LOGIC OR)
*/

lgorfn()
{
	callrt("lgor");
}

/*	void lgnotfn(void)

	HL = ! HL		(LOGIC NOT)
*/

lgnotfn()
{
	callrt("lgnot");
}
