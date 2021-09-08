/*************************************************************************
    > File Name: server_libevent.c
    > Author: Sakura7301 
    > Email: sakuraduck@foxmail.com 
    > Github: https://github.com/Sakura7301 
    > Created Time: 2021年08月17日 星期二 14时41分16秒
 ************************************************************************/

//头文件
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<event2/event.h>
#include<event2/listener.h>
#include<event2/bufferevent.h>
#include<stdbool.h>
#include"list.h"
#include"func.h"


//读回调函数
void read_cb(struct bufferevent *bev , void *arg)
{
	//printf("----------------------------------------------\n");
	//设置缓冲区大小
	char buf[R_BUF_SIZE]={0};
	char head[HEADLEN]={0};
	char userid[USERID]={0};

	//读取包头
	bufferevent_read(bev ,head ,HEADLEN-1);
	if(!strcmp(c_Head,head))//------------------------------------------------->是客户端
	{
		//printf("本次事件由客户端发起--->读回调开始\n");
		//读取ID
		bufferevent_read(bev,userid,USERID-1);

		if(strlen(userid)<6)
		{
			//printf("客户端ID非法,已主动断开\n");
			bufferevent_free(bev);
			//printf("该连接的bufferevent已经释放.\n");
			return;
		}

		//验证该ID是否已存在
		Client temp;//创建临时变量
		c_Init(&temp);
		strcpy(temp.c_id,userid);
		//printf("临时变量创建完成,正在查询...\n");
		int res=Find_LinkList(c_list,(LinkNode*)&temp,c_Compare);
		if(res!=0)//res不为0说明找到了相同的值
		{
			//printf("找到客户端:%s\n",userid);
			//获取该结点
			Client *clie=(Client*)Obtain_Node(c_list,res);
			clie->c_bev=bev;//指针指向客户端的bev

			//取出数据包
			bufferevent_read(bev,buf,DATALEN);
			c_dataParsing(buf,clie);//客户端数据包解析函数
			//printf("[客户端]数据处理完毕,本次回调结束.\n");
		}
		else//未找到则开辟空间并添加到链表上
		{
			//printf("未找到对应客户端!\n");
			//初始化
			Client *clie=(Client*)malloc(sizeof(Client));
			c_Init(clie);
			clie->c_bev=bev;//指针指向客户端的bev
			strcpy(clie->c_id,userid);//存放客户端ID


			//将客户端插入链表
			Insert_LinkList(c_list,0,(LinkNode*)clie);
			Size_LinkList(c_list);
			
			//打印链表
			Print_LinkList(c_list,c_Print);
		
			//取出数据包
			bufferevent_read(bev,buf,DATALEN);
			c_dataParsing(buf,clie);//客户端数据包解析函数
			//printf("[客户端]数据处理完毕,本次回调结束.\n");
			
		}
	}
	else if(!strcmp(e_Head,head))//---------------------------------------->是下端设备
	{
		//printf("本次事件由设备发起--->读回调开始\n");
		//读取ID
		bufferevent_read(bev,userid,USERID-1);

		if(strlen(userid)<6)
		{
			//printf("客户端ID非法,已主动断开\n");
			bufferevent_free(bev);
			//printf("该连接的bufferevent已经释放.\n");
			return;
		}

		//验证该ID是否已存在
		Equipment temp;//创建临时变量
		e_Init(&temp);
		strcpy(temp.e_id,userid);
		//printf("临时变量创建完成,正在查询...\n");
		int res=Find_LinkList(e_list,(LinkNode*)&temp,e_Compare);
		if(res!=0)//res不为0说明找到了相同的值
		{
			//printf("找到设备:%s\n",userid);
			//获取该结点
			Equipment *equi=(Equipment*)Obtain_Node(e_list,res);
			equi->e_bev=bev;//指针指向设备的bev

			//取出数据包
			bufferevent_read(bev,buf,DATALEN);
			e_dataParsing(buf,equi);//设备数据包解析函数
			//printf("[设备]数据处理完毕,本次回调结束.\n");
		}
		else//未找到则开辟空间并添加到链表上
		{
			//printf("未找到对应设备!\n");
			//初始化
			Equipment *equi=malloc(sizeof(Equipment));
			e_Init(equi);
			equi->e_bev=bev;//指针指向设备的bev
			strcpy(equi->e_id,userid);//存放ID

			//将数据存进数据库
			char query[50];
			memset(query,0,50);
			sprintf(query,"INSERT INTO Equipment(id) VALUES('%s')",equi->e_id);
			mysql_query(conn_ptr,query);

			//将设备结点插入链表
			Insert_LinkList(e_list,0,(LinkNode*)equi);
			Size_LinkList(e_list);

			//打印链表
			Print_LinkList(e_list,e_Print);

			//取出数据包
			bufferevent_read(bev,buf,DATALEN);
			e_dataParsing(buf,equi);//设备数据包解析函数
			//printf("[设备]数据处理完毕,本次回调结束.\n");
		}
	}
	else
	{
		//printf("非法连接,已丢弃该数据包.\n");
		bufferevent_free(bev);
		//printf("该连接的bufferevent已经释放.\n");
		return;
	}
	//printf("----------------------------------------------\n");
}

//事件回调函数
void event_cb(struct bufferevent *bev ,short events , void *arg)
{
	//printf("----------------------------------------------\n");
	//printf("事件回调开始\n");
	if(events & BEV_EVENT_EOF)
	{
		printf("连接已断开\n");
	}
	else if(events & BEV_EVENT_ERROR)
	{
		printf("网络异常!\n");
	}

	if(c_list->size>0)//表中有数据才去遍历
	{
		Client* clie=(Client*)c_list->head.next;
		while(clie!=NULL)
		{
			if(bev==clie->c_bev)
			{
				clie->c_bev=NULL;
				printf("客户端%s的bufferevent已经释放.\n",clie->c_id);
				break;
			}
			clie=(Client*)(clie->node.next);
		}
	}
	
	if(e_list->size>0)//表中有数据才去遍历
	{
		Equipment* equi=(Equipment*)e_list->head.next;
		while(equi!=NULL)
		{
			if(bev==equi->e_bev)
			{
				equi->e_bev=NULL;
				printf("设备%s的bufferevent已经释放.\n",equi->e_id);
				break;
			}
			equi=(Equipment*)(equi->node.next);
		}
	}
	bufferevent_free(bev);
	//printf("事件回调完成.\n");
	//printf("----------------------------------------------\n");
}

//事件监听器的回调函数
void cb_listener(struct evconnlistener *listener,
				 evutil_socket_t fd,
				 struct sockaddr *addr,
				 int len, void *ptr)
{
	//接收传入的base
	struct event_base* base = (struct event_base*)ptr;

	//与客户端通信
	struct bufferevent *bev=NULL;
	//创建bufferevent.(因为创建bufferevent需要用到base,因此我们将base用泛型指针传入回调函数使用)
	bev = bufferevent_socket_new(base , fd , BEV_OPT_CLOSE_ON_FREE);

	//给bufferevent缓冲区设置回调函数
	bufferevent_setcb(bev , read_cb , NULL , event_cb ,NULL);

	//设置使能
	bufferevent_enable(bev , EV_READ);//设置读缓冲区使能
}

int main()
{
	//创建链表
	c_list=Init_LinkList();
	e_list=Init_LinkList();

	//初始化数据库
	sqlInit();
	//读取数据
	sqlReadEquipment(e_list);
	sqlReadClient(c_list);

	//打印链表
	//Print_LinkList(e_list,e_Print);
	//Print_LinkList(c_list,c_Print);

	//初始化服务器
	struct sockaddr_in serv;
	memset(&serv , 0 , sizeof(serv));

	//老三样
	serv.sin_family=AF_INET;
	serv.sin_port=htons(PORT);
	serv.sin_addr.s_addr=htonl(INADDR_ANY);
	
	//创建base
	struct event_base *base = event_base_new();
	
	//设置监听器
	struct evconnlistener *listener;
	listener = evconnlistener_new_bind(base , cb_listener , base ,
				LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
				-1,(struct sockaddr*)&serv,sizeof(serv));
	
	//开始循环
	printf("----------------------------------------------\n");
	printf("服务器已启动,监听中...\n");
	event_base_dispatch(base);
	
	//释放资源
	FreeSpace_LinkList(c_list);//释放链表
	FreeSpace_LinkList(e_list);//释放链表
	evconnlistener_free(listener);
	event_base_free(base);
	return 0;
}
