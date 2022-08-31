#include <iostream>
#include <WinSock2.h>
using namespace std;
SOCKET server_handler(SOCKET server) {
	struct sockaddr_in addr = { 0 };
	int asize = sizeof(addr);
	return accept(server, (struct sockaddr*)&addr, &asize);
}
int client_handler(SOCKET client) {
	char buf[1024] = { 0 };
	int ret = recv(client, buf, sizeof(buf) - 1,0);
	if (ret > 0) {
		buf[ret] = 0;
		cout << buf << endl;
		if (strcmp(buf, "quit") != 0) {
			ret = send(client, buf, ret,0);
		}
		else {
			ret = -1;
		}
	}
	return ret;
}
int main() {
	SOCKET server = 0;
	struct sockaddr_in saddr = { 0 };
	unsigned int max = 0;  //最大文件句柄号
	unsigned int num = 0;  //用于select()函数返回值，若非0代表有监听的文件资源改变

	fd_set reads = { 0 };
	fd_set temps = { 0 };
	struct timeval timeout = { 0 };

	WSADATA  wd = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wd) != 0) {
		cout << "startup error" << endl;
		return -1;
	}

	server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); /*申请系统资源*/

	if (server == INVALID_SOCKET) {
		cout << "socket error" << endl;
		return -1;
	}

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY); /*设置服务器IP地址 */
	saddr.sin_port = htons(8891); /*设置服务器端口号*/
	if (bind(server, (struct sockaddr*)&saddr, sizeof(struct sockaddr)) == SOCKET_ERROR) {
		perror("bind");
		cout << "server bind error" << endl;
		return -1;
	}
	if (listen(server, 1) == SOCKET_ERROR) {
		perror("listen");
		cout << "server start error" << endl;
		return -1;
	}
	cout << "server start success" << endl;

	FD_ZERO(&reads);
	FD_SET(server, &reads);
	max = server;
	while (1) {
		temps = reads;

		timeout.tv_sec = 0;
		timeout.tv_usec = 50000;

	  //num = select(max+1, &temps, 0, 0, &timeout); //文件描述符数量 Linux用法 Windows下第一个参数无意义 仅为兼容
		num = select(0, &temps, 0, 0, &timeout);
		if (num > 0) {
			unsigned int  i = 0;
			for (i = 0; i < reads.fd_count; ++i) {
				SOCKET sock = reads.fd_array[i];
				if (FD_ISSET(sock, &temps)){
					if (sock == server) {
						SOCKET client = server_handler(server);
						if (client != INVALID_SOCKET) {
							FD_SET(client, &reads);
							cout << "accept client:" << client;
						}
					} else {
						  int r = client_handler(sock);  //处理客户端请求
						  if (r == -1) {
							  FD_CLR(sock, &reads); //断开连接后，需要取消监听该资源
							  closesocket(sock);
						  }
					  }
					}
			}
		}

	}
	closesocket(server);
	WSACleanup();
	return 0;
}