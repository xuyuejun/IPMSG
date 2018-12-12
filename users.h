//
//  users.h
//  IPMSG
//
//  Created by xuyuejun on 2018/5/28.
//  Copyright © 2018年 xuyuejun. All rights reserved.
//

#ifndef users_h
#define users_h

typedef struct usr_info{
    char usr_name[40];  //用户名
    char host_name[40];  //主机名
    unsigned int ip;    //IP地址(32位网络字节序)
    struct usr_info *next;
}USR,*pUSR;

//增
void add_user(USR user_node);

//将新节点添加到适应的位置
void order_user(pUSR new_node);

//删
void dele_user(USR user_node);

//查
pUSR find_user(char *username);

//打印列表
void list(void);

//释放链表
void free_link(void);

#endif /* users_h */
