#include <mescc.h>
#include <conio.h>
#include <printf.h>
#include <string.h>

main(argc, argv)
int argc, argv[];
{
	printf("Wellcome to CPMX\n\n");

	LogIn();
}

#define MAX_USRNAME 9
#define MAX_PASS 9

int LogIn()
{
	char usrname[MAX_USRNAME], usrpass[MAX_PASS];
	int i;

	while(1)
	{
		usrname[0]=usrpass[0]=0;

		printf("User: ");
		ReadLine(usrname, MAX_USRNAME-1); putchar('\n');
		printf("Password: ");
		ReadLine(usrpass, MAX_PASS-1); putchar('\n');

		if(!strcmp(usrname, "elvis") && !strcmp(usrpass, "presley"))
			return 0;

		printf("\nBad user name or password. Please, retry.\n\n");
	}

	return -1;
}


ReadLine(buf, width) /* from newccp.c */
char *buf;
int width;
{
	int len;
	char ch;

	printf("%s", buf); len=strlen(buf);

	while(1)
	{
		switch(ch=getch())
		{
			case 8 :
				if(len)
				{
					putchar(8); putchar(' '); putchar(8);
					--len;
				}
				break;
			case '\r' :
				buf[len]=0;
				return;
			default :
				if(ch >= ' ' && len<width)
					putchar(buf[len++]=ch);
		}
	}
}
