
/*************************************************************************
  > File Name: equipment_01.c
  > Author: Sakura7301 
  > Email: sakuraduck@foxmail.com 
  > Github: https://github.com/Sakura7301 
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cjson/cJSON.h>
#define SERV_PORT 8888
#define SERV_IP "127.0.0.1"
int main()
{
	char buf[BUFSIZ];
	memset(buf,0,sizeof(buf));
	int fd1;
	struct sockaddr_in serv_addr;
	socklen_t serv_addr_len;
	int n;
	fd1=socket(AF_INET,SOCK_STREAM,0);//建立套接字

	memset(&serv_addr,0,sizeof(serv_addr));//初始化结构体
	/*初始化结构体*/
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(SERV_PORT);
	//serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	inet_pton(AF_INET,SERV_IP,&serv_addr.sin_addr.s_addr);

	/*连接*/
	serv_addr_len=sizeof(serv_addr);
	connect(fd1,(struct sockaddr *)&serv_addr,serv_addr_len);
	
	//准备JSON数据
	strcat(buf,"Client****");//客户端固定包头
	strcat(buf,"C00001");//客户端ID
	cJSON* cjson=cJSON_CreateObject();
	char *str=NULL;
	cJSON_AddStringToObject(cjson,"Equipment_0","E12345");
	cJSON_AddStringToObject(cjson,"Equipment_1","E88888");
	cJSON_AddStringToObject(cjson,"Equipment_2","E12138");
	cJSON_AddStringToObject(cjson,"Equipment_3","E98745");
	cJSON_AddStringToObject(cjson,"Equipment_4","E00098");
	str=cJSON_Print(cjson);
	strcat(buf,str);
	write(fd1,buf,strlen(buf));//给服务器写数据
	while(1)
	{
		n=read(fd1,buf,sizeof(buf));//读取服务器发来的数据
		write(STDOUT_FILENO,buf,n);//打印读取到的数据
		printf("\n");
	}
	close(fd1);
	return 0;


}
