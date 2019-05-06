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
