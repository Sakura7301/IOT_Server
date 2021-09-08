#ifndef _LIST_H
#define _LIST_H

#include<stdio.h>
#include<stdlib.h>
#include<mysql/mysql.h>
#include<event2/bufferevent.h>

#define USERID 7      //用户名的长度为6(将\0计入)

//宏定义
#define SQL_IP "localhost"
#define SQL_USER "sakura"
#define SQL_PASSWORD "123456"
#define SQL_DATABASE "IOT"

//全局变量
MYSQL *conn_ptr;
MYSQL_RES	 *res_ptr;
MYSQL_ROW sqlrow;
unsigned int timeout = 7;


//-----------------------------------------------------数据结构(链表)
//链表小节点
typedef struct LINKNODE {
	struct LINKNODE* next;
}LinkNode;

//链表节点
typedef struct LINKLIST {
	LinkNode head;
	int size;
}LinkList;

//客户端结构体定义
typedef struct Client
{
	LinkNode node; //挂钩
	char c_id[USERID];//客户端ID
	char c_sub[10][USERID];//客户端订阅的设备的ID
	struct bufferevent *c_bev;//客户端的bev
}Client;

//设备结构体定义
typedef struct Equipment
{
	LinkNode node;  //挂钩
	char e_id[USERID];//设备ID
	struct bufferevent *e_bev;//设备的bev
	bool e_led;//LED的状态
}Equipment;


//遍历函数的指针,自定义类型写法
typedef void(*PRINTNODE)(LinkNode*);

//比较函数的指针
typedef int(*COMPARENODE)(LinkNode*, LinkNode*);

//初始化链表
LinkList* Init_LinkList(); 

//指定位置插入
void Insert_LinkList(LinkList* list, int pos, LinkNode* data);

//删除指定位置的值
void Remove_LinkList(LinkList* list, int pos);

//查找
int Find_LinkList(LinkList* list, LinkNode* data, COMPARENODE compare);

//获取节点
LinkNode* Obtain_Node(LinkList* list, int pos);

//返回链表的大小
int Size_LinkList(LinkList* list);

//打印链表结点
void Print_LinkList(LinkList* list, PRINTNODE print);

//释放链表内存
void FreeSpace_LinkList(LinkList* list);

//设备初始化函数
void e_Init(Equipment* data);

//客户端初始化函数
void c_Init(Client* data);

void sqlReadEquipment(LinkList *list);
void sqlReadClient(LinkList *list);
void sqlInit();
void sqlSaveEquiData(LinkList *list);

//数据库初始化
void sqlInit()
{
	conn_ptr=mysql_init(NULL);
	if(!conn_ptr)
	{
		printf("初始化失败!\n");
	}
	int ret=mysql_options(conn_ptr,MYSQL_OPT_CONNECT_TIMEOUT,(const char*)&timeout);//设置超时
	if(ret)
	{
		printf("选项设置错误!\n");
	}
	conn_ptr=mysql_real_connect(conn_ptr,SQL_IP,SQL_USER,SQL_PASSWORD,SQL_DATABASE,0,NULL,0);//连接数据库
	if(conn_ptr)
	{
		printf("连接数据库成功!\n");
	}
	else
	{
		printf("连接数据库失败\n");
	}

	return;
}

//读取数据库中的数据并存入设备链表
void sqlReadEquipment(LinkList *list)
{
	//printf("----------------------------------------------\n");
	//printf("正在读取数据....\n");
	mysql_query(conn_ptr,"SELECT id FROM Equipment");
	res_ptr=mysql_store_result(conn_ptr);
	if(res_ptr)
	{
		while((sqlrow=mysql_fetch_row(res_ptr)))
		{
			Equipment *equi=(Equipment*)malloc(sizeof(Equipment));
			e_Init(equi);
			strcpy(equi->e_id,sqlrow[0]);
			//将该结点插入链表
			Insert_LinkList(list,0,(LinkNode*)equi);
		}
	}
	//printf("数据库读取完成\n");
}


//读取数据库的数据并存入客户端链表
void sqlReadClient(LinkList *list)
{
	//printf("----------------------------------------------\n");
	//printf("正在读取数据....\n");
	mysql_query(conn_ptr,"SELECT id,sub FROM Client");
	res_ptr=mysql_store_result(conn_ptr);
	if(res_ptr)
	{
		while((sqlrow=mysql_fetch_row(res_ptr)))
		{
			Client *clie=(Client*)malloc(sizeof(Client));
			c_Init(clie);
			strcpy(clie->c_id,sqlrow[0]);
			char* p=sqlrow[1];
			int len=strlen(sqlrow[1])/5;
			for(int i=0;i<len;i++)
			{
				memcpy(clie->c_sub[i],p,5);
				p+=5;
			}

			//将该结点插入链表
			Insert_LinkList(list,0,(LinkNode*)clie);
		}
	}
	//printf("数据库读取完成\n");
}

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


//--------------------------------------------链表函数实现
//------------初始化链表
LinkList * Init_LinkList() 
{
	//printf("----------------------------------------------\n");
	//printf("正在初始化链表....\n");
	LinkList* list = (LinkList*)malloc(sizeof(LinkList));
	list->head.next = NULL;
	list->size = 0;
	//printf("初始化完成!\n");
	return list;
}

//------------指定位置插入
void Insert_LinkList(LinkList* list, int pos, LinkNode* data) 
{
	//printf("----------------------------------------------\n");
	//printf("正在插入链表...\n");
	if (list == NULL) {
		printf("插入失败,链表为空!\n");
		return;
	}
	if (data == NULL) {
		printf("插入失败,数据为空\n");
		return;
	}
	if (pos<0 || pos>list->size) {
		pos = list->size;
	}

	//查找插入位置
	LinkNode* pCurrent = &(list->head);
	for (int i = 0; i < pos; i++) {
		pCurrent = pCurrent->next;
	}

	//插入新节点
	data->next = pCurrent->next;
	pCurrent->next = data;

	list->size++;
	//printf("插入成功!\n");
}

//------------删除指定位置的值
void Remove_LinkList(LinkList* list, int pos) 
{
	//printf("----------------------------------------------\n");
	//printf("正在删除数据...\n");
	if (list == NULL) {
		//printf("删除失败,链表为空!\n");
		return;
	}
	if (pos<0 || pos>list->size) {
		pos = list->size;
	}
	//辅助指针变量
	LinkNode* pCurrent = &(list->head);
	for (int i = 0; i < pos; i++) {
		pCurrent = pCurrent->next;
	}

	//删除节点
	pCurrent->next = pCurrent->next->next;
	list->size--;
	//printf("删除成功!\n");
}

//-----------查找
int Find_LinkList(LinkList* list, LinkNode* data, COMPARENODE compare) 
{
	//printf("----------------------------------------------\n");
	//printf("正在查询...\n");
	if (list == NULL) {
		//printf("查询失败,链表为空!\n");
		return -1;
	}
	if (data == NULL) {
		//printf("查询失败,数据为空!\n");
		return -1;
	}
	//辅助指针变量
	LinkNode* pCurrent = list->head.next;//指向链表的第一个元素
	int index = 1;
	while (pCurrent != NULL) {
		//查找到相同数据
		if (compare(pCurrent, data) == 0) {
			//printf("查询成功!\n");
			return index;//若找到相同数据,返回元素位置(index)
		}
		pCurrent = pCurrent->next;//不匹配则指向链表的下一个节点
		index++;
	}
	//printf("未能查询到该数据!");
	return 0;//未查找到相同数据,返回0
}

//------------获取目标节点
LinkNode * Obtain_Node(LinkList * list, int pos)
{
	//printf("----------------------------------------------\n");
	//printf("正在获取结点...\n");
	//链表为空
	if (list == NULL) {
		//printf("获取失败,链表为空!\n");
		return NULL;
	}
	//链表不合法
	if (pos<0 || pos>list->size) {
		//printf("获取失败,链表长度不合法!\n");
		return NULL;
	}
	//查找目标位置的结点
	LinkNode* pCurrent = &(list->head);
	for (int i = 0; i < pos; i++) {
		pCurrent = pCurrent->next;
	}
	//printf("获取成功!正在返回.\n");
	return pCurrent;
}

//------------链表大小
int Size_LinkList(LinkList * list)
{
	//printf("[ 链表长度为:%d ]\n",list->size);
		return list->size;
}

//------------打印链表结点
void Print_LinkList(LinkList* list, PRINTNODE print) 
{
	//printf("----------------------------------------------\n");
	//printf("正在打印链表...\n");
	if (list == NULL) {
		//printf("表是空的!\n");
		return;
	}
	//辅助指针变量
	LinkNode* pCurrent = list->head.next;
	while (pCurrent != NULL) {
		print(pCurrent);
		pCurrent = pCurrent->next;
	}
	//printf("打印完成\n");
}

//-----------释放链表内存
void FreeSpace_LinkList(LinkList* list) {
	//printf("----------------------------------------------\n");
	//printf("正在释放链表内存...\n");
	if (list == NULL) {
		return;
	}

	free(list);
	//printf("释放成功!\n");
}

#endif
