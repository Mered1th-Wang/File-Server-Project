# File-Server-Project

[toc]

***

## 5.3 

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


## 5.4 数据库设计

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


## 5.5 一期功能设计

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



## 5.7

完成功能：
1. 验证登录，通过salt和crypt加密传输。
2. 日志系统，将客户端用dup机制输出重定向至文件
3. 小文件断点续传功能，在gets文件时先看本地是否有该文件，用stat读取文件大小，结合lseek偏移接收文件。
（大文件在client端通过ftruncate创建文件大小，不能用该方法，需要将recv的值保存，保存至数据库/新文件？大文件断点续传未完成）
4. 差cd和大文件断点续传，完成三期功能

写完及时git，今天晚上血的教训，做了很多无用功，直接把main.c用别的文件覆盖了！

*** 
## 5.8

除了文件未连数据库，大文件断点续传， 三期功能全部完成。17:08

***
## 5.9

修改前三期功能中的bug，前面对端发送消息未用火车头，如客户端单发一个int，服务端接收int，但由于实际网络问题，
推测可能因为tcp连接到达对端的先后顺序不同，最后导致数据出错，修改所有发送接收全部用火车头协议。

### gets和puts与瞬时命令分离思路

明天开始写四期功能实现gets和puts与瞬时命令分离，大致思路：
1. 服务端子线程再创建孙线程，子线程处理能快速响应命令，孙线程处理upload和download，通过设置epoll监听3、4号option，任务加入队列，如果队列不空，
通过条件变量唤醒孙线程处理3、4号option，而子线程直接处理瞬时命令。
2. 客户端创建子线程，也是通过epoll将”短命令和长命令“分离， 并设置30s等待协议，如果发送命令就关闭子线程，如果30s后发出命令通过token值
校验重新创建子线程连接服务端。

3. 客户端通过单独创建一个子线程用于等待服务端断开后自动断开客户端主线程连接。

## 5.10

        昨晚将四期想的有点复杂，因为原本前三期的设计框架就是的是主线程监听客户端的socketfd，
    再直接由子线程处理该客户端的全部请求，这设计实现起来较为简单（不知道别人怎么设计的），
    但实现gets与puts方面又需要多线程操作，刚开始想到的是子线程再创子线程，相当麻烦不再赘述。

### gets与puts分离，new idea：

        客户端创建子线程，主线程和子线程分别conncect服务器，产生两个socketfd，对于服务端而言这
    两个线程是”客户端“，那么如何让主线程登录成功后，子线程免密登录呢？
    
    这里就是用token实现，子线程通过token值登录，客户端处在输入命令后首先判断长命令还是短命令，
    如果是长命令则发给子线程，短命令由主线程发送。

### Token值设计和功能实现

        token值由用户名/密码/时间结合设计产生，主线程在登录后在服务端产生token值保存至数据库，
    同时将TokeN值发送至客户端，由子线程接收，并让子线程登录即可。

5.12
        完成四期gets与puts分离，增加了数据库中Account新列current，用于保存当前操作路径，目的是当
    子线程完成登录进行gets和puts时保持与主线程操作的是同一个路径。
    这里给子线程开了权限让子线程能查询数据库，一是为了子线程与主线程同步当前操作路径current，
    二是在子线程登录的时候直接查询数据库中的Token是否存在，若存在直接登录，少了一次与主线程之间的
    通信。虽然简化了操作，但不可避免会有安全性问题。


