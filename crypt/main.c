#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <shadow.h>
#include <errno.h>
#include <crypt.h>
void getsalt(char *salt,char* passwd)
{
	int i,j;
	for(i=0,j=0;passwd[i]&&j!=3;i++)
	{
		if(passwd[i]=='$')
		{
			++j;
		}
	}
	strncpy(salt,passwd,i-1);
}


int main(int argc,char* argv[])
{
	struct spwd *sp;
	char *passwd;
	char salt[512]={0};
	if(argc!=2)
	{
		printf("error args\n");
		return -1;
	}
	passwd=getpass("请输入密码:");
	sp=getspnam(argv[1]);
	if(NULL==sp)
	{
		perror("getspnam");
		return -1;
	}
	printf("%-100s\n",sp->sp_pwdp);
	getsalt(salt,sp->sp_pwdp);
	puts(salt);
	if(!strcmp(sp->sp_pwdp,crypt(passwd,salt)))	
	{
		printf("登录成功\n");
	}else{
		printf("登录失败\n");
	}	
	return 0;

}
