//
//  main.c
//  IPMSG
//
//  Created by xuyuejun on 2018/5/27.
//  Copyright © 2018年 xuyuejun. All rights reserved.
//

#include <stdio.h>
#include "include.h"
#include "user_interface.h"
#include "network_service.h"
#include "users.h"
#include "mytcp.h"

int main(int argc, const char * argv[]) {
    pthread_t tid;
    //系统初始化
    sysinit();
    //用户界面线程,用户命令输入
    pthread_create(&tid,NULL,user_interface,NULL);
    //用户接受消息线程,接受其他客户的UDP数据
    pthread_create(&tid, NULL,recv_msg_thread, NULL);
    //文件发送接受
    pthread_create(&tid, NULL, tcp_server, NULL);
    //主线程
    pthread_join(tid,NULL);
    return 0;
}
