#ifndef _FUNC_H
#define _FUNC_H

#include"list.h"
#include<cjson/cJSON.h>
//宏定义
#define PORT 8888		//端口

#define R_BUF_SIZE 1024  //接收缓冲区大小
#define T_BUF_SIZE 1024  //发送的缓冲区大小

#define HEADLEN 11		//固定包头长度为10(将\0计入)
#define DATALEN 1008   //数据包的长度

//定义固定包头,不够的用*填充
char c_Head[HEADLEN]={"Client****\0"};//客户端的固定包头
char e_Head[HEADLEN]={"Equipment*\0"};//设备的固定包头

//定义链表
LinkList *c_list;
LinkList *e_list;
//-----------------------------------------func
//设备初始化函数
//void e_Init(Equipment* data);

//客户端初始化函数
//void c_Init(Client* data);

//客户端比较函数
int c_Compare(LinkNode* node1,LinkNode* node2);

//设备比较函数
int e_Compare(LinkNode* node1,LinkNode* node2);

//客户端打印函数
void c_Print(LinkNode* data);

//设备打印函数
void e_Print(LinkNode* data);

//客户端封包函数
char* c_packet(char *data);

//设备封包函数
char* e_packet(char *data);

//客户端数据包解析函数
void c_dataParsing(char *data,Client *clie);

//设备数据包解析函数
void e_dataParsing(char *data,Equipment* equi);

//推送订阅内容的函数
void PushMessage(LinkList* list, COMPARENODE compare,Equipment* equi);

//字符串判空函数
int Empty(char *data);

//-------------------------------------函数实现--------------------
//客户端数据包解析函数
void c_dataParsing(char *data,Client *clie)
{
	//printf("----------------------------------------------\n");
	//printf("我是[客户端]的数据包解析函数,我正在被执行\n");
	if(Empty(data)==-1)
	{
		//printf("该客户端的数据包是空的\n");
		return;
	}
	//printf("收到来自[客户端]数据:%s\n",data);
	printf("收到来自[客户端]数据\n");
	
	//解析整段JSON数据
	cJSON* cjson=cJSON_Parse(data);
	cJSON* cjsontemp=NULL;

	char str[12]="Equipment_*";
	char num[11]="0123456789";
	//解析String类型JSON数据
	int index=0;
	for(int i=0;i<10;i++)
	{

		str[10]=num[i];
		cjsontemp=NULL;
		cjsontemp=cJSON_GetObjectItem(cjson,str);
		if(cjsontemp==NULL || cjsontemp->type==cJSON_NULL)
		{
			//该类型不存在
			index++;
			continue;
		}
		else
		{
			//该类型存在
			index--;
			strcpy(clie->c_sub[i],cjsontemp->valuestring);//更新订阅
		}
	}
	if(index==10)
	{
		printf("[客户端]数据包不合法!\n");
		return;
	}
	printf("[客户端]订阅内容已更新.\n");
	//释放JSON对象
	cJSON_Delete(cjson);

	//将客户端的订阅信息存入数据库
	//将数据存进数据库
	char sub[70];
	memset(sub,0,200);
	for(int i=0;i<10;i++)
	{
		if(strlen(clie->c_sub[i])>0)
		{
			strcat(sub,clie->c_sub[i]);
		}
	}
	char query[150];
	memset(query,0,150);
	sprintf(query,"INSERT INTO Client(id,sub) VALUES('%s','%s')",clie->c_id,sub);
	mysql_query(conn_ptr,query);//执行数据库语句
	//已将该客户端的数据存入数据库
	//printf("已经将该客户端的数据存入数据库.\n");

	

	//打印客户端链表
	//Print_LinkList(c_list,c_Print);	
}

//设备数据包解析函数
void e_dataParsing(char *data,Equipment* equi)
{
	//printf("----------------------------------------------\n");
	//printf("我是[设备]的数据包解析函数,我正在被执行\n");
	if(Empty(data)==-1)
	{
		//printf("该客户端的数据包是空的\n");
		return;
	}
	//printf("收到来自[设备]的数据:%s\n",data);
	printf("收到来自[设备]的数据\n");

	//解析整段JSON数据
	cJSON* cjson=cJSON_Parse(data);
	//解析bool数据
	cJSON* cjsonLED=cJSON_GetObjectItem(cjson,"LED");
	if(cjsonLED==NULL || cjsonLED->type==cJSON_NULL)
	{
		printf("[设备]数据包不合法!\n");
		return;
	}
	//更新该设备的LED属性
	equi->e_led=cjsonLED->valueint;
	cJSON_Delete(cjson);
	printf("[设备]相关数据已更新\n");

	//遍历所有已经订阅的客户端,并发送数据
	PushMessage(c_list,c_Compare,equi);	

	printf("订阅内容推送完成\n");

	//打印设备链表
	//Print_LinkList(e_list,e_Print);
}


//推送订阅内容的函数
void PushMessage(LinkList* list, COMPARENODE compare,Equipment *equi) 
{
	//printf("----------------------------------------------\n");
	//printf("正在查询已订阅的客户端\n");
	if (list->size==0) {
		//printf("查询失败,链表为空!\n");
		return;
	}
	
	//1->准备数据,2->封包
	cJSON* cjson=cJSON_CreateObject();
	char *data=NULL;
	char *Equipment_id=equi->e_id;
	cJSON_AddStringToObject(cjson,"ID",equi->e_id);//ID
	cJSON_AddNumberToObject(cjson,"LED",equi->e_led);//LED
	data=cJSON_Print(cjson);
	cJSON_Delete(cjson);//释放JSON对象
	char *buf=c_packet(data);//数据封包
	//printf("buf:%s\n",buf);	
	//辅助指针变量
	Client* clie =(Client*)list->head.next;//指向链表的第一个元素
	while (clie != NULL) {
		for(int i=0;i<10;i++)
		{
			//查找到相同数据并且该客户端的bev不为空(在线)
			if(strcmp(clie->c_sub[i],Equipment_id)==0 && clie->c_bev!=NULL )
			{
				//3->找到客户端,给该客户端推送
				bufferevent_write(clie->c_bev,buf,strlen(buf));
			}
		}
		clie = (Client*)(clie->node.next);//不匹配则指向链表的下一个节点
	}
	
	//printf("推送完成.\n");
}



/*
//客户端初始化函数
void c_Init(Client* data)
{
	Client* p=data;
	memset(p->c_id,0,sizeof(p->c_id));
	memset(p->c_sub,0,sizeof(p->c_sub));
	p->c_bev=NULL;
}

//设备初始化函数
void e_Init(Equipment* data)
{
	Equipment* p=data;
	memset(p->e_id,0,sizeof(p->e_id));
	p->e_bev=NULL;
	p->e_led=0;
}
*/

//客户端打印函数
void c_Print(LinkNode* data)
{
	Client *p=(Client*)data;
	printf("c_id:%s\n",p->c_id);
	for(int i=0;i<10;i++)
	{
		if(strlen(p->c_sub[i])>0)
		{
			printf("equipment_%d:%s\n",i,p->c_sub[i]);
		}
	}
	return;
}


//设备打印函数
void e_Print(LinkNode* data)
{
	Equipment *p=(Equipment*)data;
	printf("e_id:%s\n",p->e_id);
	printf("e_led:%d\n",p->e_led);
	return;
}


//客户端比较函数(成功-> 0,  失败-> -1)
int c_Compare(LinkNode* node1,LinkNode* node2)
{
	Client *p1=(Client*)node1;
	Client *p2=(Client*)node2;
	if(strcmp(p1->c_id,p2->c_id)==0)
	{
		return 0;
	}
	return -1;
}

//设备比较函数(成功-> 0,   失败-> -1)
int e_Compare(LinkNode* node1,LinkNode* node2)
{
	Equipment *p1=(Equipment*)node1;
	Equipment *p2=(Equipment*)node2;
	if(strcmp(p1->e_id,p2->e_id)==0)
	{
		return 0;
	}
	return -1;
}


//客户端封包函数
char* c_packet(char *data)
{
	char *buf=malloc(R_BUF_SIZE);//定义缓冲区
	bzero(buf,R_BUF_SIZE);//初始化缓冲区
	memcpy(buf,c_Head,9);
	strcat(buf,data);
	return buf;
}

//设备封包函数
char* e_packet(char *data)
{
	char *buf=malloc(R_BUF_SIZE);//定义缓冲区
	bzero(buf,R_BUF_SIZE);//初始化缓冲区
	memcpy(buf,e_Head,9);
	strcat(buf,data);
	return buf;
}

//字符串判空函数
int Empty(char *data)
{
	if(strlen(data)>1)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
#endif
