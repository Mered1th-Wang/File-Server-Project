# File-Server-Project

5.3 

数据库设计：

1. 文件池——复合主键，但code需要自增又不能加入primary，solution?

```mysql
create table FILE(
procode int NOT NULL, 
code int AUTO_INCREMENT NOT NULL, 
filename varchar(20) NOT NULL, 
belong int NOT NULL, 
md5 char(32) DEFAULT NULL, 
primary key(code, belong, filename));
```
2. 账户——数据库连接（需封装函数）

```mysql
create table Account( ID int NOT NULL AUTO_INCREMENT, 
                      name varchar(20) NOT NULL, 
                    salt varchar(15) NOT NULL, 
                    crypt_code varchar(512) DEFAAULT NULL , 
                    PRIMARY KEY(ID)); 
```
