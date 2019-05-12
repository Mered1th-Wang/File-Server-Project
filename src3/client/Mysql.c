#include "function.h"
#include <mysql/mysql.h>

int query_token(char* token)
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
                //进该循环表示找到token，登陆成功！
                //printf("select ok!\n");
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
