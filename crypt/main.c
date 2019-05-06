#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int login(int argc,char* argv[])
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

	if(!strcmp(sp->sp_pwdp,crypt(passwd,salt)))	
	{
		printf("登录成功\n");
	}else{
		printf("登录失败\n");
	}	
	return 0;
}
