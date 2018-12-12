//
//  network_service.c
//  IPMSG
//
//  Created by xuyuejun on 2018/5/27.
//  Copyright © 2018年 xuyuejun. All rights reserved.
//
#include <ifaddrs.h>  
#include <arpa/inet.h>
#include <string.h>  
#include "network_service.h"
#include "user_interface.h"
#include "users.h"
#include "include.h"
#include "mytcp.h"

static int udpfd;       //UDP socket描述符
static char user_name[20] = "";         //用户名
static char host_name[20] = "";         //主机名
FINFO recv_buf[5];


//获取广播地址
struct sockaddr_in getBR()
{
    struct sockaddr_in *sin = NULL, result;
    struct ifaddrs *ifa = NULL, *ifList;
    if (getifaddrs(&ifList) < 0)
    {
        return result;
    }
    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next)
    {
        if(ifa->ifa_addr->sa_family == AF_INET)
        {
            sin = (struct sockaddr_in *)ifa->ifa_addr;
			if ( strcmp(inet_ntoa(sin->sin_addr), "127.0.0.1") != 0)
			{
				sin = (struct sockaddr_in *)ifa->ifa_dstaddr;
				result = *sin;
				break;
			}
        }
    }
    freeifaddrs(ifList);
    return result;
}


//套接字创建函数,将其与本机的2424端口绑定
void create_server()
{
    int broadcast=1;
    struct sockaddr_in addr = {AF_INET};  //初始化一个网络地址
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    udpfd = socket(AF_INET,SOCK_DGRAM,0);
    if(udpfd < 0)
        perror("Socket UDP");
    
    if(bind(udpfd, (struct sockaddr*)&addr, sizeof(addr))<0)
        perror("Bind UDP");
            
    setsockopt(udpfd,SOL_SOCKET,SO_BROADCAST,&broadcast, sizeof(int));
}

int udp_fd(void)
{
    return udpfd;
}

//获取用户名
void input_user_name()
{
    printf("Please input your User Name:");
    scanf("%s",user_name);
    
}

//返回用户名
char *user(void)
{
    return user_name;
}

//获取主机名
void input_host_name()
{
    printf("Please input your Host Name:");
    scanf("%s",host_name);
}

//返回主机名
char *host(void)
{
    return host_name;
}

void online(void)
{
    char broadcast_online_msg[512] = "";    //广播上线包
    struct sockaddr_in addr = {AF_INET};
    int t = time((time_t *)NULL);
    
    //初始化目标网络地址
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = getBR().sin_addr.s_addr;
    
    //组建广播上线包的报文
    int len = sprintf(broadcast_online_msg, "1:%d:%s:%s:%ld:%s",t,user_name,host_name,IPMSG_BR_ENTRY,user_name);
    //广播报文
    sendto(udpfd,broadcast_online_msg,len,0,(struct sockaddr*)&addr,sizeof(addr));
}



//系统初始化函数
void sysinit()
{
    input_user_name();
    input_host_name();
    create_server();
    online();
    printf("Name:%s    Host:%s\n",user(),host());
    printf("You have been successfully Online\n");
}

/*应答上线用户*/
static void answer_entry(int pkgnum, struct sockaddr_in addr)
{
    char buf[100];
    int len = 0;
    
    //组建包文
    len = sprintf(buf,"1:%d:%s:%s:%ld:%s",pkgnum,user(),
                  host(),IPMSG_ANSENTRY,user());
    //发送包文
    sendto(udp_fd(),buf,len,0,(struct sockaddr*)&addr,sizeof(addr));
}

static void print_msg(char *sender, char *msgbuf)
{
    printf("\n%s: %s\n\n", sender, msgbuf);//打印发送者，消息
    write(1,"\rIPMSG:",7);
    fflush(stdout);
}

static int isrecvfile(long cmddata)
{
    //cmddata为接收到的命令字
    if((IPMSG_SENDMSG|IPMSG_FILEATTACHOPT|IPMSG_READCHECKOPT|
        IPMSG_SENDCHECKOPT|IPMSG_SECRETOPT) == cmddata)
        return 1;
    else
        return 0;
}

static void save_file_info(int msgnum,long cmddata,char *msgbuf,char *sender,char *attrbuf,struct sockaddr_in addr)
{
	int i = 0;
	int j = 0;
	int t = time((time_t *)NULL); //获取包编号
	char *p[5] = {NULL};          //用于存放文件的属性，一共可以存放5个文件        
	char buf[100];  
	int len;  
	
	//发送确认应答，以确保发送文件的包文已被接收
	len = sprintf(buf, "1:%d:%s:%s:%ld:%d",t,user_name,host_name,IPMSG_RECVMSG,msgnum);		
	sendto(udpfd, buf, len, 0, (struct sockaddr*)&addr,sizeof(addr));
	
	//将recv_buf数组初始化为0，想想sizeof(recv_buf+1)有多大？	
	memset(recv_buf, 0, sizeof(recv_buf));
	
	//以‘\a’切割attrbuf
	p[0] = attrbuf;
	while( (p[i]=strtok(p[i],"\a")) != NULL ) //学习这种切割技巧
	{
		//对recv_buf[i]中的每个元素赋值，注意是%lx，而不是%ld
		sscanf(p[i], "%d:%[^:]:%lx:%lx:%lx:",&recv_buf[i].filenum,recv_buf[i].filename,&recv_buf[i].filesize,&recv_buf[i].filectime,&recv_buf[i].filemode);
		recv_buf[i].msgnum = msgnum;
		recv_buf[i].offset = 0;
		recv_buf[i].ip = addr.sin_addr.s_addr;
		i++;	
	}
	
	//打印相关信息
	printf("\n%s: %s\n", sender,msgbuf); //打印发送者名字、消息
	printf("等待接收的文件:");         //打印要接收的文件
	for(j=0; j<i; j++)
		printf("  %s", recv_buf[j].filename); 
	printf("\n\n");	
	write(1,"\rIPMSG:",7);             //打印命令提示符
	fflush(stdout);
}

//接受消息线程
void *recv_msg_thread(void *arg)
{
    while(1)
    {
        char buf[300]="\0";
        struct sockaddr_in addr = {AF_INET}; //存放发送者的网络地址
        socklen_t addrlen = sizeof(addr);
        int len = 0;
        long pkgnum = 0;                     //接收包文的包编号
        long cmd = 0;                        //命令字
        char msg[100]="";                    //消息
        char attr[200]="";                   //文件属性
        int t = 0;                           //接收time函数的返回值，以作为包编号
        USR temp;                             //存放上下线用户的信息
        
        //等待接收包文
        len = recvfrom(udp_fd(),buf,sizeof(buf),0,(struct sockaddr*)&addr, &addrlen);
        
        //判断包文是否正确
        if(strncmp(buf, "1:", 2)!=0)
            continue;
        
        //解析包文
        sscanf(buf, "1:%ld:%[^:]:%[^:]:%ld:%s",&pkgnum,temp.usr_name,temp.host_name, &cmd, msg);
        strcpy(attr, buf+strlen(buf)+1);
        
        temp.ip = addr.sin_addr.s_addr;
        
        //分析命令字，并做相应的事情
        switch(GET_MODE(cmd))
        {
            case IPMSG_BR_ENTRY:     //有用户上线
                add_user(temp);
                answer_entry(t, addr);
                break;
            case IPMSG_ANSENTRY:     //自己上线
                add_user(temp);
                break;
            case IPMSG_SENDMSG:      //收到消息
                if(isrecvfile(cmd))  //是否为接收文件的消息
                    save_file_info(pkgnum, cmd, msg, temp.usr_name, attr, addr);
                else
                    print_msg(temp.usr_name, msg);
                break;
            case IPMSG_RECVMSG:      //应答消息，须在发送时添加应答选项
                break;
            case IPMSG_BR_EXIT:      //有用户退出
                dele_user(temp);
                break;
            default :
                break;
        }
    }
    return NULL;
}

