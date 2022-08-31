#include <iostream>
#include <WinSock2.h>
using namespace std;
int main() {

	SOCKET sock = 0;

	struct sockaddr_in addr = { 0 };
	int len = 0;
	char buf[128] = { 0 };
	char input[32] = { 0 };
	int  r = 0;
	int i = 0;

	WSADATA  wd = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wd) != 0) {
		cout << "startup error" << endl;
		return -1;
	}

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == SOCKET_ERROR) {
		cout << "socket error" << endl;
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); /*设置服务器IP地址*/
	addr.sin_port = htons(8891); /*设置服务器端口号*/

	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		cout << "connect error" << endl;
		return -1;
	}

	cout << "connect success" << endl;

	while (1)
	{
		cout << "Input:";
		cin >> input;
		len = send(sock, input, strlen(input) + 1, 0);
		r = recv(sock, buf, sizeof(buf), 0);

		if (r > 0) {
			cout << "Receive: " << buf << endl;
		}
		else {
			break;
		}
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}