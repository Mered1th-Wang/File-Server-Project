# File-Server-Project

5.3 

数据库设计：
源码在srcX文件夹，X以日期递增。

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

5.4 数据库设计

账户
| Account | CREATE TABLE `Account` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) NOT NULL,
  `salt` varchar(20) NOT NULL,
  `crypt_code` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci |

文件池——外键关联
| FILE  | CREATE TABLE `FILE` (
  `precode` int(11) NOT NULL,
  `code` int(11) NOT NULL AUTO_INCREMENT,
  `FileName` varchar(20) NOT NULL,
  `FileType` int(11) NOT NULL,
  `belong` int(11) NOT NULL,
  `md5` int(11) DEFAULT NULL,
  PRIMARY KEY (`code`,`FileName`,`FileType`),
  KEY `belong` (`belong`),
  CONSTRAINT `FILE_ibfk_1` FOREIGN KEY (`belong`) REFERENCES `Account` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci |

5.5 一期功能设计

16:36 除了cd命令已完成一期功能，下面开始链接数据库, 密码验证。

+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Table   | Create Table                                                                                                                                                                                                                                                                      |
+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Account | CREATE TABLE `Account` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) NOT NULL,
  `salt` varchar(20) NOT NULL,
  `crypt_code` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci |
+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

23:51 密码验证完成，conf配置文件完成，数据库记录日志ing.
+-------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| log   | CREATE TABLE `log` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `requeset` varchar(50) NOT NULL,
  `connect` varchar(50) NOT NULL,
  `operate_time` varchar(50) NOT NULL,
  `operate_log` varchar(20) NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci |
+-------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+



5.7
完成功能：
1. 验证登录，通过salt和crypt加密传输。
2. 日志系统，将客户端用dup机制输出重定向至文件
3. 小文件断点续传功能，在gets文件时先看本地是否有该文件，用stat读取文件大小，结合lseek偏移接收文件。
（大文件在client端通过ftruncate创建文件大小，不能用该方法，需要将recv的值保存，保存至数据库/新文件？大文件断点续传未完成）
4. 服务端文件以md5命名存储。
