# 使用说明

作为一个纯C语言开发的服务器程序

使用了libevent和cJSON两个开源库.

因此在编译它之前,请先安装这两个开源库.

​	您可以选择使用makefile直接编译source的源文件之后使用,也可以选择编译之后使用restart.sh脚本来执行本程序,写它是为了防止崩溃重启,虽然目前并没有崩溃过.....

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

### windows连接测试:

![image](https://github.com/Sakura7301/IOT_Server/blob/master/image/windows平台测试.png)



### user:Sakura
