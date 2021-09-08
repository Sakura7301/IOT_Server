# 使用说明

作为一个纯C语言开发的服务器程序

使用了libevent和cJSON两个开源库.

因此在编译它之前,请先安装这两个开源库.

​	您可以选择使用makefile直接编译source的源文件之后使用,也可以选择编译之后使用restart.sh脚本来执行本程序,写它是为了防止崩溃重启,虽然目前并没有崩溃过.....

## 以下为测试结果:

#### NC命令连接:

![1](C:\Users\Sakura\Desktop\日志\IOT服务器项目\IOTserver\image\1.png)

#### 服务器反馈:

![2](C:\Users\Sakura\Desktop\日志\IOT服务器项目\IOTserver\image\2.png)

#### 断开连接测试:

![bufferevent释放测试](C:\Users\Sakura\Desktop\日志\IOT服务器项目\IOTserver\image\bufferevent释放测试.png)

#### 多用户测试:

![test_01](C:\Users\Sakura\Desktop\日志\IOT服务器项目\IOTserver\image\test_01.png)

![test_02](C:\Users\Sakura\Desktop\日志\IOT服务器项目\IOTserver\image\test_02.png)

### windows连接测试:

![windows平台测试](C:\Users\Sakura\Desktop\日志\IOT服务器项目\IOTserver\image\windows平台测试.png)



### user:Sakura
