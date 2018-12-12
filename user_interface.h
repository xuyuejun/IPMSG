//
//  user_interface.h
//  IPMSG
//
//  Created by xuyuejun on 2018/5/27.
//  Copyright © 2018年 xuyuejun. All rights reserved.
//

#ifndef user_interface_h
#define user_interface_h

#include <stdio.h>

typedef struct file_info{
    int msgnum;
    int filenum;
    int offset;
    long filesize;
    long filectime;
    long filemode;
    int ip;
    char filename[100];
}FINFO;

//命令函数指针
typedef void (*FUN)(int argc,char *argv[]);

typedef struct command
{
    char *name;     //命令名称
    FUN fun;        //命令处理函数
}COMMAND;

//添加定义，用于处理线程输入的命令
void *user_interface(void *arg);

#endif /* user_interface_h */
