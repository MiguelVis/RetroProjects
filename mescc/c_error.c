/*	c_error.c

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Errors module.

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
	27 Nov 2015 : Modified some text messages.
*/

errndch(c)
char c;
{
	char *s;

	s="Need: ?";

	*(s+strlen(s)-1)=c;

	errcont(s);
}

errcmdl(msg)
char *msg;
{
	co_nl();
	co_str(ERERROR);co_line(msg);
	exit(1);
}

errfile(msg,fname)
char *msg,*fname;
{
	co_nl();
	co_str(ERERROR);
	co_str(ERFILE);
	co_str(fname);
	co_str(" -- ");
	co_line(msg);
	exit(1);
}

errcont(msg)
char *msg;
{
	errshow(msg);

	if(errcnt==errmax)
	{
		co_nl();
		co_line("Too many errors, compilation aborted.");
		exit(1);
	}

	if(errstop)
	{
		if(getch()=='.')
			errstop=0;
	}
}

errexit(msg)
char *msg;
{
	errshow(msg);
	exit(1);
}

errshow(msg)
char *msg;
{
	int i;

	++errcnt;

	fo_nl();co_nl();
	comment();
	fo_str(co_str(ERERROR));fo_dec(co_dec05(errcnt));fo_ch(co_ch(' '));
	fo_str(co_str(ERLINE));fo_dec(co_dec05(fi_line));fo_ch(co_ch(' '));
	fo_str(co_str(ERFILE));fo_line(co_line(fi_name));

	comment();
	fo_line(co_line(msg));

	comment();
	for(i=0;line[i];++i)
	{
		if(line[i]=='\t')
			fo_ch(co_ch(' '));
		else
			fo_ch(co_ch(line[i]));
	}

	fo_nl();co_nl();

	comment();
	for(i=0;i<lptr;++i)
		fo_ch(co_ch(' '));
	fo_ch(co_ch('^'));
	fo_nl();co_nl();
}
