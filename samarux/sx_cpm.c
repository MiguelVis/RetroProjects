/*	sx_cpm.c

	CPM for Samarux.

	Execute CP/M commands.

	Copyright (c) 2007 - 2015 Miguel I. Garcia Lopez.

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

	cpm command [arg ...]

	Options:

	None.

	Examples:

	cpm type hello.txt

	Changes:

	04 Jan 2015 : 1.00 : 1st version.
	05 Jan 2015 : 1.01 : Uses temporary drive in CP/M 3.
	08 Jan 2015 : 1.02 : Adapted to shell changes.
	16 Feb 2015 : 1.03 : Changed return command.
	23 Feb 2015 : 1.04 : Changed return command again.
	                     Save SamaruX status, to be restored when restart.
	27 Feb 2015 : 1.05 : $$$.SUB will be created in current drive.
	09 Apr 2015 : 1.06 : Status file will be created in temporary directory.
	                     For CP/M 2, use A00:$$$.SUB.
	                     For CP/M 3, use BDOS CHAIN fn.
	05 Jun 2016 : 1.07 : Added support for builtin / external.

	Issues in CP/M 2:

	If the CP/M command fails, we will not return to SamaruX.

	Issues in CP/M 3:

	Drive & user in CCP doesn't change to SamaruX ones,
	but when SamaruX returns, the values are right (!).
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_CPM

#else

#include "samarux.h"

#define CpmMain main

#endif

#define CPM_STF "profrst.sx"

CpmMain(argc, argv)
int argc, argv[];
{
	int cpm3;
	char *stf, *cmd;

	/* Check arguments */

	if(argc == 1)
		return ErrorPars();

	/* Get CP/M version */

	cpm3 = sv_cpmver >= 0x30;

	/* For CP/M 2, we must be in A00: */

	if(!cpm3 && sv_drive + sv_user)
		return Error("Current directory must be A00:");

	/* Build temporary file name for status */

	stf = TmpFile(CPM_STF);

	/* Save SamaruX status */

	if(SaveStatus(stf))
		return Error("Can't save status");

	/* UnParse the command line */

	if((cmd = UnParse(argc - 1, argv + 1)) == NULL)
		return 1;

	/* Execute procedure, according to CP/M version */

	if(cpm3)
		Cpm3(cmd, stf);
	else
		Cpm2(cmd, stf);

	/* There was an error */

	free(cmd);

	remove(stf);

	return 1;
}

Cpm3(cmd, stf)
char *cmd, *stf;
{
	char *bf;

	/* Build command line */

	bf = 0x0080;

	strcpy(bf, cmd); strcat(bf, "!sx -r "); strcat(bf, stf);

	/* Chain to program (default CCP drive & user WOULD be the SamaruX current ones) */

	bdos_a(0x2F, 0x00FF);
}

Cpm2(cmd, stf)
char *cmd, *stf;
{
	char *bf, *sub;
	FILE *fp;

	/* Buffer */

	if((bf = malloc(256)) == NULL)
		return ErrorMem();

	/* 1st record: Samarux return command */

	strcpy(bf + 1, "sx -r "); strcat(bf + 1, stf); *bf = strlen(bf + 1);

	/* 2nd record: CP/M command line */

	strcpy(bf + 129, cmd); bf[128] = strlen(cmd);

	/* Make $$$.SUB file */

	sub = "A00:$$$.SUB";

	if((fp = fopen(sub, "wb")) != NULL)
	{
		if(fwrite(bf, 256, 1, fp) == 1)
		{
			if(!fclose(fp))
			{
				/* Exit to CCP */

				exit();
			}
		}

		/* Failure */

		fclose(fp); remove(sub);
	}

	/* Free buffer */

	free(bf);

	/* Failure */

	return Error("Can't create A00:$$$.SUB");
}

#undef CPM_STF
