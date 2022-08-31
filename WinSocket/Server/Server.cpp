#include <iostream>
#include <WinSock2.h>
using namespace std;
int main() {
	SOCKET server = 0;
	SOCKET client = 0;
	struct sockaddr_in saddr = { 0 };
	struct sockaddr_in caddr = { 0 };
	int  asize = 0;
	int len = 0;
	char buf[32] = { 0 };
	int  r = 0;

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
	}
	cout << "server start success" << endl;

	while (1) {

		asize = sizeof(caddr);
		client = accept(server, (struct sockaddr*)&caddr, &asize);  /*accept 会阻塞该程序 */


		if (client == INVALID_SOCKET) {
			perror("accept");
			return -1;
		}

		cout << "client: " << client << endl;

		do {
			int i = 0;
			r = recv(client, buf, sizeof(buf), 0); /*每次最多读取buf大小的数据*/

			if (r > 0) {
				cout << "Receive: " << buf << endl;
				if (strcmp(buf, "quit") != 0) { /*客户端输入quit断开连接 其他情况原句发给客户端*/
					len = send(client, buf, r, 0);
				}
				else {
					break;
				}
			}
		} while (r > 0);
		closesocket(client);
	}
	closesocket(server);
	WSACleanup();
	return 0;
}