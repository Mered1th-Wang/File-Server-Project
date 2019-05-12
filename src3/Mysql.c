#include "factory.h"

int login_query(char* username, pUserInfo_t info)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	const char* server = "localhost";
	const char* user = "root";
	const char* password = "123";
	const char* database = "NetDisk";//要访问的数据库名称
	char query[300] = "select * from Account where name='";
	sprintf(query, "%s%s%s", query, username, "'");
	int t;
	conn = mysql_init(NULL);
	if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
	{
		printf("Error connecting to database:%s\n", mysql_error(conn));
		return -1;
	}
	t = mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n", mysql_error(conn));
	}else{
		res = mysql_use_result(conn);
		if(res)
		{
			while((row = mysql_fetch_row(res)) != NULL)
			{	
                strcpy(info->salt, row[2]);
                strcpy(info->crypt_code, row[3]);
                break;
            }
		}else{
			printf("Don't find the Account!\n");
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int update_mysql(char *token, char*name)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	const char* server = "localhost";
	const char* user = "root";
	const char* password = "123";
	const char* database = "NetDisk";//要访问的数据库名称
	char update[200]="update Account set token='";
    //printf("name = %s\n", name);
    sprintf(update, "%s%s%s%s%s", update, token, "'where name='", name, "'");
	//puts(update);
    int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}else{
		//printf("Connected...\n");
	}
	t=mysql_query(conn,update);
	if(t)
	{
		printf("Error making update:%s\n",mysql_error(conn));
	}else{
		//printf("update success\n");
	}
	mysql_close(conn);
	return 0;
}

int update_dir(char *name, char *current)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	const char* server = "localhost";
	const char* user = "root";
	const char* password = "123";
	const char* database = "NetDisk";//要访问的数据库名称
	char update[200]="update Account set current='";
    //printf("name = %s\n", name);
    sprintf(update, "%s%s%s%s%s", update, current, "'where name='", name, "'");
	//puts(update);
    int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}else{
		//printf("Connected...\n");
	}
	t=mysql_query(conn,update);
	if(t)
	{
		printf("Error making update:%s\n",mysql_error(conn));
	}else{
		//printf("update success\n");
	}
	mysql_close(conn);
	return 0;
}


int query_current(char* token, pDir current)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	const char* server = "localhost";
	const char* user = "root";
	const char* password = "123";
	const char* database = "NetDisk";//要访问的数据库名称
	char query[300] = "select * from Account where token='";
	sprintf(query, "%s%s%s", query, token, "'");
    //puts(query);
	int t;
	conn = mysql_init(NULL);
	if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
	{
		printf("Error connecting to database:%s\n", mysql_error(conn));
		return -1;
	}
	t = mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n", mysql_error(conn));
	}else{
		res = mysql_use_result(conn);
		if(res)
		{
			while((row = mysql_fetch_row(res)) != NULL)
			{	
                //printf("query_current = %s\n", row[5]);
                strcpy(current->pathNow, row[5]);
                break;
            }
		}else{
			printf("Don't find the Account!\n");
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}
