/*************************************************************************
  > File Name: equipment.c
  > Author: Sakura7301 
  > Email: sakuraduck@foxmail.com 
  > Github: https://github.com/Sakura7301 
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cjson/cJSON.h>
#define SERV_PORT 8888
#define SERV_IP "127.0.0.1"
int main()
{
	char buf[BUFSIZ];
	int fd1;
	struct sockaddr_in serv_addr;
	socklen_t serv_addr_len;
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
	bool flag=false;
	while(1)
	{
	
		memset(buf,0,sizeof(buf));
		strcat(buf,"Equipment*");//设备固定包头
		strcat(buf,"E12138");//设备ID
		cJSON* cjson=cJSON_CreateObject();
		char *str=NULL;
		cJSON_AddNumberToObject(cjson,"LED",flag);//设备数据
		str=cJSON_Print(cjson);
		strcat(buf,str);
		write(fd1,buf,strlen(buf));//上传到服务器
		flag=(!flag);//设备数据变更
		sleep(5);
	}
	close(fd1);
	return 0;


}
