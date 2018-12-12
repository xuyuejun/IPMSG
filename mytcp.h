#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

/*tcp服务器线程，用于发送文件*/
void *tcp_server(void *data);

/*tcp客户端函数,用于接收文件*/
void tcp_client(int num, char *path);

#endif	



