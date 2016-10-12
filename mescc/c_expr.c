/*	c_expr.c

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Expression parser module.

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

	16 Jan 2001 : Last revision.
	16 Apr 2007 : GPL'd.
	26 Oct 2015 : Cleaned.
	20 Nov 2015 : Updated, function addglb() has changed.
	03 Oct 2016 : Added '\e' to ChEsc().
	13 Oct 2016 : Documented and optimized a bit.

	Operators precedence:

	operator	function	associativity
	--------------	-------------	-------------
	! ~ ++ -- - * &	heir10		right to left
	* / %		heir9		left to right
	+ -		heir8		left to right
	<< >>		heir7		left to right
	< <= > >=	heir6		left to right
	== !=		heir5		left to rigth
	&		heir4		left to rigth
	^		heir3		left to right
	|		heir2		left to right
	&&		heir1e		left to right
	||		heir1b		left to right
	?:		heir1a		right to left
	=  -= += *= %=	heir1		right to left
	/= &= ^= |=

	lval[0] is a symbol table address or 0 for constants
	lval[1] is a type for indirect objects or 0 for static objects
	lval[2] is a type for a pointer or array or 0
*/

expresn()
{
	int lval[3];
	if(heir1(lval))
		rvalue(lval);
}

heir1(lval)
int lval[];
{
	int k,lval2[3];
	char op;

	k=heir1a(lval);

	if(BfNeq('=') && BfNextNeq('='))
		return k;

	switch(op=Bf())
	{
		case '=' :
			BfGet();
			if(k)
			{
				if(lval[1])
					fpush();
				if(heir1(lval2))
					rvalue(lval2);
				break;
			}

			ndlval();
			return 0;
		case '+' :
		case '-' :
			BfGet();BfGet();
			if(!k)
			{
				ndlval();
				return 0;
			}
			if(lval[1])
				fpush();
			rvalue(lval);
			fpush();
			if(heir1(lval2))
				rvalue(lval2);
			if(dbltest(lval,lval2))
				doubreg();
			fpop();
			if(dbltest(lval2,lval))
			{
				swap();
				doubreg();
				swap();
			}

			if(op=='+')
				addfn();
			else
			{
				subfn();
				if((lval[2] & TY_INT) && (lval2[2] & TY_INT))
				{
					swap();
					immed_dec(1);
					a_sr(1);
				}
			}

			/*result(lval,lval2);*/
			break;
		case '*' :
		case '/' :
		case '%' :
		case '&' :
		case '^' :
		case '|' :
			BfGet();BfGet();
			if(!k)
			{
				ndlval();
				return 0;
			}
			if(lval[1])
				fpush();
			rvalue(lval);
			fpush();
			if(heir1(lval2))
				rvalue(lval2);
			fpop();

			switch(op)
			{
				case '*' :
				a_mul(signtest(lval) && signtest(lval2));
					break;
				case '/' :
				a_div(signtest(lval) && signtest(lval2));
					break;
				case '%' :
				a_mod(signtest(lval) && signtest(lval2));
					break;
				case '&' : andfn();	break;
				case '^' : xorfn();	break;
				case '|' : orfn();	break;
			}

			break;
		default :
			return k;
	}

	store(lval);

	return 0;
}

heir1a(lval)
int lval[];
{
	int k,lval2[3],lab1,lab2;

	k=heir1b(lval);

	if(InChEq('?'))
	{
		if(k)
			rvalue(lval);

		do
		{
			lab1=getlabl();
			testjmp(lab1);

			if(heir1b(lval2))
				rvalue(lval2);

			lab2=getlabl();
			jump(lab2);

			a_lnlabel(lab1);

			ndch(':');

			if(heir1b(lval2))
				rvalue(lval2);

			a_lnlabel(lab2);
		}
		while(InChEq('?'));

		k=0;
	}

	return k;
}


heir1b(lval)
int lval[];
{
	int k,lval2[3];

	k=heir1e(lval);

	if(InWordEq('||'))
	{

		if(k)
			rvalue(lval);

		do
		{
			fpush();
			if(heir1e(lval2))
				rvalue(lval2);
			fpop();
			lgorfn();
		}
		while(InWordEq('||'));

		k=0;
	}

	return k;
}

heir1e(lval)
int lval[];
{
	int k,lval2[3];

	k=heir2(lval);

	if(InWordEq('&&'))
	{
		if(k)
			rvalue(lval);

		do
		{
			fpush();
			if(heir2(lval2))
				rvalue(lval2);
			fpop();
			lgandfn();
		}
		while(InWordEq('&&'));

		k=0;
	}

	return k;
}

heir2(lval)
int lval[];
{
	int k,lval2[3];

	k=heir3(lval);

	InBlanks();

	if(BfNeq('|') || BfNextEq('|') || BfNextEq('='))
		return k;

	if(k)
		rvalue(lval);

	while(InChEq('|'))
	{
		fpush();
		if(heir3(lval2))
			rvalue(lval2);
		fpop();
		orfn();
	}

	return 0;
}

heir3(lval)
int lval[];
{
	int k,lval2[3];

	k=heir4(lval);

	InBlanks();

	if(BfNeq('^') || BfNextEq('='))
		return k;

	if(k)
		rvalue(lval);

	while(InChEq('^'))
	{
		fpush();
		if(heir4(lval2))
			rvalue(lval2);
		fpop();
		xorfn();
	}

	return 0;
}

heir4(lval)
int lval[];
{
	int k, lval2[3];

	k=heir5(lval);

	InBlanks();

	if(BfNeq('&') || BfNextEq('&') || BfNextEq('='))
		return k;

	if(k)
		rvalue(lval);

	while(InChEq('&'))
	{
		fpush();
		if(heir5(lval2))
			rvalue(lval2);
		fpop();
		andfn();
	}

	return 0;
}

heir5(lval)
int lval[];
{
	int k,lval2[3];

	k=heir6(lval);

	InBlanks();

	if(BfWordNeq('==') && BfWordNeq('!='))
		return k;

	if(k)
		rvalue(lval);

	while(1)
	{
		if(InWordEq('=='))
			k=0;
		else if(InWordEq('!='))
			k=1;
		else
			return 0;

		fpush();
		if(heir6(lval2))
			rvalue(lval2);
		fpop();

		k ? ne() : eq();
	}
}

heir6(lval)
int lval[];
{
	int k,lval2[3];

	k=heir7(lval);

	InBlanks();

	if(BfNeq('<') && BfNeq('>') && BfWordNeq('<=') && BfWordNeq('>='))
		return k;

	if(BfNextEq('<') || BfNextEq('>'))
		return k;

	if(k)
		rvalue(lval);

	while(1)
	{
		if(InWordEq('<='))
			k=0;
		else if(InWordEq('>='))
			k=1;
		else if(BfEq('<') && BfNextNeq('<'))
			k=2;
		else if(BfEq('>') && BfNextNeq('>'))
			k=3;
		else
			break;

		if(k>1)
			inbyte();

		fpush();
		if(heir7(lval2))
			rvalue(lval2);
		fpop();

		if(signtest(lval) && signtest(lval2))
			k+=4;

		switch(k)
		{
			case 0 : a_le(0); break;
			case 1 : a_ge(0); break;
			case 2 : a_lt(0); break;
			case 3 : a_gt(0); break;
			case 4 : a_le(1); break;
			case 5 : a_ge(1); break;
			case 6 : a_lt(1); break;
			case 7 : a_gt(1); break;
		}
	}

	return 0;
}

heir7(lval)
int lval[];
{
	int k,lval2[3];

	k=heir8(lval);

	InBlanks();

	if(BfWordNeq('>>') && BfWordNeq('<<'))
		return k;

	if(k)
		rvalue(lval);

	while(1)
	{
		if(InWordEq('>>'))
			k=0;
		else if(InWordEq('<<'))
			k=1;
		else
			break;

		fpush();
		if(heir8(lval2))
			rvalue(lval2);
		fpop();

		k ? a_sl(signtest(lval)) : a_sr(signtest(lval));
	}

	return 0;
}

heir8(lval)
int lval[];
{
	int k,lval2[3];

	k=heir9(lval);

	InBlanks();

	if((BfNeq('+') && BfNeq('-')) || BfNextEq('='))
		return k;

	if(k)
		rvalue(lval);

	while(1)
	{
		if(InChEq('+'))
			k=0;
		else if(InChEq('-'))
			k=1;
		else
			break;

		fpush();
		if(heir9(lval2))
			rvalue(lval2);
		if(dbltest(lval,lval2))
			doubreg();
		fpop();
		if(dbltest(lval2,lval))
		{
			swap();
			doubreg();
			swap();
		}

		if(k)
		{
			subfn();
			if((lval[2] & TY_INT) && (lval2[2] & TY_INT))
			{
				swap();
				immed_dec(1);
				a_sr(1);
			}
		}
		else
			addfn();

		if(lval2[2])
		{
			if(lval[2])
				lval[2]=0;
			else
			{
				lval[0]=lval2[0];
				lval[1]=lval2[1];
				lval[2]=lval2[2];
			}
		}
		else if(signtest(lval))
		{
			if(!signtest(lval2))
			{
				lval[0]=lval2[0];
				lval[1]=lval2[1];
				lval[2]=lval2[2];
			}
		}
	}

	return 0;
}

heir9(lval)
int lval[];
{
	int k,lval2[3];

	k=heir10(lval);

	InBlanks();

	if((BfNeq('*') && BfNeq('/') && BfNeq('%')) || BfNextEq('='))
		return k;

	if(k)
		rvalue(lval);

	while(1)
	{
		if(InChEq('*'))
			k=0;
		else if(InChEq('/'))
			k=1;
		else if(InChEq('%'))
			k=2;
		else
			break;

		fpush();
		if(heir10(lval2))
			rvalue(lval2);
		fpop();

		if(signtest(lval))
		{
			if(signtest(lval2))
				k+=3;
			else
			{
				lval[0]=lval2[0];
				lval[1]=lval2[1];
				lval[2]=lval2[2];
			}
		}

		switch(k)
		{
			case 0 : a_mul(0); break;
			case 1 : a_div(0); break;
			case 2 : a_mod(0); break;
			case 3 : a_mul(1); break;
			case 4 : a_div(1); break;
			case 5 : a_mod(1); break;
		}
	}

	return 0;
}

heir10(lval)
int lval[];
{
	int k;
	char *ptr;

	if(InWordEq('++'))
		heir10(lval) ? a_incdec(lval, 1, 0) : ndlval();

	else if(InWordEq('--'))
		heir10(lval) ? a_incdec(lval, 0, 0) : ndlval();

	else if(BfEq('-') && !isdigit(BfNext()))
	{
		BfGet();
		if(heir10(lval))
			rvalue(lval);
		negfn();
	}
	else if(InChEq('*'))
	{
		if(heir10(lval))
			rvalue(lval);
		lval[1]=(ptr=lval[0]) ? ptr[SY_TYPE] : TY_INT;
		lval[2]=0;
		return 1;
	}
	else if(InChEq('&'))
	{
		if(heir10(lval))
		{
			ptr=lval[0];
			lval[2]=ptr[SY_TYPE];
			if(!lval[1])
			{
				if(ptr[SY_STORAGE]==ST_STKLOC)
					getloc(ptr);
				else
					immed_str(ptr);
				lval[1]=ptr[SY_TYPE];
			}
		}
		else
			errcont(ERILADR);
	}
	else if(InChEq('~'))
	{
		if(heir10(lval))
			rvalue(lval);
		comfn();
	}
	else if (InChEq('!'))
	{
		if(heir10(lval))
			rvalue(lval);
		lgnotfn();
	}
	else
	{
		k=heir11(lval);

		if(InWordEq('++'))
			k ? a_incdec(lval, 1, 1) : ndlval();

		else if(InWordEq('--'))
			k ? a_incdec(lval, 0, 1) : ndlval();
		else
			return k;
	}

	return 0;
}

// Level 11: [], function calls

heir11(lval)
int *lval;
{
	int k;
	char *ptr;

	k = primary(lval);

	ptr = lval[0];

	InBlanks();

	if(BfEq('[') || BfEq('('))
	{
		while(1)
		{
			if(InChEq('['))
			{
				if(!ptr)
				{
					errcont(ERCTSUB);
					junk();
					ndch(']');

					return 0;
				}
				else if(ptr[SY_IDENT] == ID_PTR)
					rvalue(lval);
				else if(ptr[SY_IDENT] != ID_ARR)
				{
					errcont(ERCTSUB);
					k = 0;
				}

				fpush();
				expresn();
				ndch(']');

				if(ptr[SY_TYPE] & TY_INT)
					doubreg();

				fpop();
				addfn();

				lval[0] = 0;
				lval[1] = ptr[SY_TYPE];
				k=1;
			}
			else if(InChEq('('))
			{
				if(!ptr)
					callfnc(0);
				else if(ptr[SY_IDENT] != ID_FUN)
				{
					rvalue(lval);
					callfnc(0);
				}
				else callfnc(ptr);

				k = lval[0] = 0;
			}
			else
				return k;
		}
	}

	if(ptr)
	{
		if(ptr[SY_IDENT] == ID_FUN)
		{
			immed_str(ptr);
			k = 0;
		}
	}

	return k;
}

// Primary level: parenthesis, symbols, constants

primary(lval)
int *lval;
{
	char *ptr, sname[NAME_SIZ];
	int num[1], k, szof;

	lval[2] = 0;

	// Parenthesis
	if(InChEq('('))
	{
		k = heir1(lval);
		ndch(')');
		return k;
	}

	// Symbol name
	if(symname(sname))
	{
		// Local symbol
		if(ptr = findloc(sname))
		{
			lval[0] = ptr;

			if(ptr[SY_IDENT] == ID_ARR)
			{
				getloc(ptr);
				lval[1] = lval[2] = ptr[SY_TYPE];
				return 0;
			}

			lval[1] = 0;

			if(ptr[SY_IDENT] == ID_PTR)
				lval[2] = ptr[SY_TYPE];

			return 1;
		}

		// Global symbol
		if(ptr = findglb(sname))
		{
			if(ptr[SY_IDENT] != ID_FUN)
			{
				lval[0] = ptr;

				if(ptr[SY_IDENT] == ID_ARR)
				{
					immed_str(ptr);
					lval[1] = lval[2] = ptr[SY_TYPE];
					return 0;
				}

				lval[1] = 0;

				if(ptr[SY_IDENT] == ID_PTR)
					lval[2] = ptr[SY_TYPE];

				return 1;
			}
		}
		else if(!strcmp(sname, "sizeof"))
		{
			ndch('(');
			if(InSymEq("char"))
				szof = 1;
			else if(InSymEq("int"))
				szof = 2;
			else
				errcont(ERSIZOF);

			if(InChEq('*'))
				szof = 2;
			ndch(')');

			immed_dec(szof);

			return lval[0] = lval[1] = 0;
		}
		else
		{
			// Undefined symbol, function assumed
			ptr = addglb(sname, ID_FUN, TY_INT, 0, 0, 0);
		}

		// It's a function

		lval[0] = ptr;
		lval[1] = 0;

		return 0;
	}

	if(cnstant(num))
		return lval[0] = lval[1] = 0;

	// Error: invalid expression
	errcont(ERINVEX);

	immed_dec(0);
	junk();

	return 0;
}

// Check if val1 is int pointer or int array and val2 it's not pointer or array

dbltest(val1, val2)
int val1[], val2[];
{
	char *ptr;

	if(val1[2] & TY_INT)
	{
		ptr = val1[0];

		if(ptr[SY_IDENT] == ID_PTR || ptr[SY_IDENT] == ID_ARR)
		{
			if(!val2[2])
				return 1;
		}
	}

	return 0;
}

// Check if signed

signtest(lval)
int lval[];
{
	char *ptr;

	if(lval[2] || lval[1] == TY_UINT || ((ptr = lval[0]) && ptr[SY_TYPE] == TY_UINT)) // FIXME -- Optimize this ??
		return 0;

	return 1;
}

// Store value

store(lval)
int *lval;
{
	char *ptr;

	if(lval[1])
		putstk(lval[1]);
	else
	{
		ptr = lval[0];

		if(ptr[SY_STORAGE] == ST_STKLOC)
			putvloc(ptr);
		else
			putmem(ptr);
	}
}

// Read value

rvalue(lval)
int *lval;
{
	char *ptr;

	if(lval[0] && !lval[1])
	{
		ptr = lval[0];

		if(ptr[SY_STORAGE] == ST_STKLOC)
			getvloc(ptr);
		else
			getmem(ptr);
	}
	else
		indrct(lval[1]);
}

// Test expression

test(label)
int label;
{
	ndch('('); expresn(); ndch(')');
	testjmp(label);
}

// Get constant

cnstant(val)
int val[];
{
	if(number(val))
		immed();
	else if(pstr(val))
		immed();
	else if(qstr(val))
	{
		immed(); a_label(litlab); fo_ch('+');
	}
	else
		return 0;

	fo_dec(val[0]); fo_nl();

	return 1;
}

// Get number

number(val)
int val[];
{
	int k, sign, c;

	k = sign = 1;

	while(k)
	{
		k = 0;

		if(InChEq('+'))
			k = 1;

		if(InChEq('-'))
		{
			sign = (-sign);
			k = 1;
		}
	}

	// Check for hexadecimal number

	if(BfEq('0'))
	{
		if(BfNextEq('x') || BfNextEq('X'))
		{
			BfGet();
			BfGet();

			//if(!isxdigit(Bf()))
			//	return 0;

			// FIXME -- What if 0xT ??

			while(isxdigit(Bf()))
			{
				if((c = BfGet() /*inbyte()*/) <= '9')
					k = k * 16 + (c - '0');
				else
					k = k * 16 + ((c & 95) - '7');
			}

			val[0] = k;

			return 1;
		}
	}

	// Check for decimal number

	if(isdigit(Bf()))
	{
		do
		{
			/*c = inbyte();*/
			k = k * 10 + (BfGet() - '0');
		} while(isdigit(Bf()));

		if(sign < 0)
			k = (-k);
		val[0] = k;

		return 1;
	}

	// Not a number
	return 0;
}

// Get single quoted strings

pstr(val)
int *val;
{
	if(!InChEq('\''))
		return 0;

	*val = 0;

	while(Bf() && BfNeq('\''))
	{
		*val = (*val << 8) + (ChEsc() & 0xFF);
	}

	if(BfGet())
		return 1;

	// Missing trailing quote
	errndch('\'');
}

// Get double quoted strings

qstr(val)
int *val;
{
	if(!InChEq('"'))
		return 0;

	*val = litptr;

	while(Bf() && BfNeq('"'))
	{
		if(litptr != STRBUF_SIZ)
			litq[litptr++] = ChEsc();
		else
			ChEsc();
	}

	if(BfGet())
	{
		if(litptr != STRBUF_SIZ)
		{
			litq[litptr++] = 0; return 1;
		}

		// String buffer exhausted
		errexit(EROFSTR);
	}

	// Missing trailing quote
	errndch('"');
}

// Get (possibly escaped) character

ChEsc()
{
	char c;

	if(BfNEq('\\'))
		return BfGet();

	// It's \x
	BfGet();

	switch((c = BfGet()))
	{
		case 'n' : return 10;
		case 't' : return 9;
		case 'r' : return 13;
		case 'b' : return 8;
		case 'a' : return 7;
		case 'f' : return 12;
		case 'e' : return 27; /* No ANSI C compliant; supported by GCC and others */
		case '0' : return 0;
	}

	return c;
}

// Error: need lvalue

ndlval()
{
	errcont(ERMBLVL);
}
