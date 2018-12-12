//
//  network_service.h
//  IPMSG
//
//  Created by xuyuejun on 2018/5/27.
//  Copyright © 2018年 xuyuejun. All rights reserved.
//

#ifndef network_service_h
#define network_service_h

#include <stdio.h>

#define PORT 2425   //IPMSG使用的端口号

int udp_fd(void);
char *user(void);
char *host(void);

//接收消息线程，接收其他客户端发送的UDP数据
void *recv_msg_thread(void *arg);

#endif /* network_service_h */
