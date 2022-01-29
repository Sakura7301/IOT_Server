![example](https://img.shields.io/badge/tiny_iot_server-v1.0-green.svg)   ![example](https://img.shields.io/badge/user:-sakura-blue.svg)
# 使用说明

作为一个纯C语言开发的服务器程序

使用了libevent和cJSON两个开源库.

因此在编译它之前,请先安装这两个开源库.

数据库我使用了mysql,有其他需求请更换代码中调用的API。

您可以选择使用makefile直接编译source的源文件之后使用,也可以选择编译之后使用restart.sh脚本来执行本程序,写它是为了防止崩溃重启,虽然目前并没有崩溃过.....
***

## **实现流程**

    1.服务器启动
 
    2.设备连接服务器->服务器数据库更新
 
    3.客户端连接服务器->更新订阅
 
    4.设备上传信息
 
    5.服务器查询订阅该设备的客户端->推送订阅信息
***

## **目录**
- 源文件: [source](https://github.com/Sakura7301/IOT_Server/tree/master/source)

- 测试demo:[test](https://github.com/Sakura7301/IOT_Server/tree/master/test)
***

## 以下为测试结果:

#### NC命令连接:

 ![image](https://github.com/Sakura7301/IOT_Server/blob/master/image/1.png)
#### 服务器反馈:

![image](https://github.com/Sakura7301/IOT_Server/blob/master/image/2.png)

#### 断开连接测试:

![image](https://github.com/Sakura7301/IOT_Server/blob/master/image/bufferevent释放测试.png)

#### 多用户测试:

![image](https://github.com/Sakura7301/IOT_Server/blob/master/image/test_01.png)

![image](https://github.com/Sakura7301/IOT_Server/blob/master/image/test_02.png)

#### windows连接测试:

![image](https://github.com/Sakura7301/IOT_Server/blob/master/image/windows平台测试.png)

***
#### **关键字:** libevent、cJSON、Subscribe

