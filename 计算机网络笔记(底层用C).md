#                                计算机网络笔记(底层用C|C++)

#### 主机字节序：

   不同的[CPU](https://www.baidu.com/s?wd=CPU&tn=24004469_oem_dg&rsv_dl=gh_pl_sl_csd)有不同的字节序类型，这些字节序是指整数在内存中保存的顺序，这个叫做主机序。最常见的有两种 1．Little endian：低字节存高地址，高字节存低地址 2．Big endian：低字节存低地址，高字节存高地址

#### 网络字节序

   网络字节顺序是TCP/IP中规定好的一种数据表示格式，它与具体的CPU类型、操作系统等无关，从而可以保证数据在不同主机之间传输时能够被正确解释。网络字节顺序采用big endian排序方式(而我们通常使用的系统为小端系统，所以需要在这两者之间转换)

  相应转换函数如下：

1. ```C
   1.  htons  
   
      htons 把unsigned short类型从主机序转换到网络序
   
   2. ntohs 
   
      ntohs 把unsigned short类型从网络序转换到主机序
   
   3. htonl
   
      htonl 把unsigned long类型从主机序转换到网络序
   
   4. ntohl
   
      ntohl 把unsigned long类型从网络序转换到主机序。
   头文件#include <arpa/inet.h>
   ```

​    将用点分割的IP地址转换位一个in_addr结构的地址，实际上就是一个unsigned long值。计算机内部处理IP地址可是不认识如192.1.8.84之类的数据。 

```c
unsigned long inet_addr( const char * cp ) 将相应IP转成unsigned long
举例：inet_addr("192.1.8.84")=1409810880 
int inet_aton( const char * cp，struct_in_addr* inp) 效果如上 不过有个返回值表示是否转换成功
```

如果发生错误，函数返回INADDR_NONE值。
将网络地址转换位用点分割的IP地址，是上面函数的逆函数。 

```c
char * inet_ntoa( struct in_addr in) 将unsigned long值转成相应IP
举例：
char * ipaddr=NULL
in_addr inaddr
inaddr.s_addr=1409810880
ipaddr = inet_ntoa(inaddr)
补充:需要注意的是该函数每次都将转换的结果放在同一个空间中
解决方式：使用strcpy 字符串拷贝
```

#### 客户端

```C
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
using namespace std;
int main () {
 
    int sock = 0;
    
    struct sockaddr_in addr = {0};


    sock = socket(PF_INET,SOCK_STREAM,0);
    if (sock == -1 ) {
        cout<<"socket error"<<endl;
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("192.168.43.62"); /*设置服务器IP地址*/
    addr.sin_port = htons(1000); /*设置服务器端口号*/
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1 ){
        cout<<"connect error"<<endl;
        return -1;
    }

    cout<<"connect success"<<endl;
    close(sock);
    sock = -1;
    return 0;
}
```

#### 服务端

```C
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
using namespace std;
int main () {
    int server = 0 ;
    int client = 0 ;
    struct sockaddr_in saddr = {0};
    struct sockaddr_in caddr = {0};
    socklen_t asize = 0;
    int len = 0;
    char buf[32] = {0};
    int  r = 0;
    
    server = socket(PF_INET,SOCK_STREAM,0); /*申请系统资源*/
    
    if (server == -1 ) {
        cout<<"socket error"<<endl;
        return -1;
    }

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY); /*设置服务器IP地址 这里表示 Address to accept any incoming messages. INADDR_ANY真实值为0.0.0.0 
    当然也可自己设置某IP  saddr.sin_addr.s_addr = inet_addr("192.168.43.65");*/
    saddr.sin_port = htons(8889); /*设置服务器端口号*/
    if ( bind(server, (struct sockaddr*)&saddr, sizeof(struct sockaddr)) == -1 ) {
        perror("bind");
        cout<<"server bind error"<<endl;
        return -1;
    }
    if ( listen(server,1)==-1 ) {
        perror("listen");
        cout<<"server start error"<<endl;
    }
    cout<<"server start success"<<endl;
    
    asize = sizeof(caddr);
    client = accept(server,(struct sockaddr*)&caddr, &asize);  /*accept 会阻塞该程序 */
     
 
    if (client == -1) {
        perror("accept");
        cout<<"client acccept error"<<endl;
        return -1;
    }
    
    cout<<"client: "<<client<<endl;
 
    do
    {
        int i = 0;
        r = recv(client, buf, sizeof(buf), 0); /*每次读取buf大小的数据*/
        
        if (r > 0) {
            len += r;
        }

        for (size_t i = 0; i < r; i++){
            cout<<buf[i];
        }
        
    } while (len < 64);/*当读取到64字节退出*/

    char msg[] = "Hello World";
    send(client, msg, sizeof(msg), 0); /*返回数据 参数4一般设置成0表示阻塞*/
    
    sleep(1); 
   
    close(client);
    close(server);
    client = -1;
    server = -1;
    return 0;
}
```

#### 创建echo客户端

```c
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
using namespace std;
int main () {
 
    int sock = 0;
    
    struct sockaddr_in addr = {0};
    int len = 0;
    char buf[128] = {0};
    char input[32] = {0};
    int  r = 0;
    int i  = 0;



    sock = socket(PF_INET,SOCK_STREAM,0);
    if (sock == -1 ) {
        cout<<"socket error"<<endl;
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr .s_addr = inet_addr("192.168.43.65"); /*设置服务器IP地址*/
    addr.sin_port = htons(8890); /*设置服务器端口号*/
    
    if (connect(sock,(struct sockaddr*)&addr,sizeof(addr))==-1){
        cout<<"connect error"<<endl;
        return -1;
    }

    cout<<"connect success"<<endl;

    while (1)
    {
        cout<<"Input:";
        cin>>input;
        len =send(sock, input, strlen(input)+1, 0);
        r = recv(sock, buf, sizeof(buf), 0);
        
        if (r > 0) {
            cout<<"Receive: "<<buf<<endl;
        } else {
            break;
        }
    }
    
    close(sock);
    return 0;
}
```

#### 创建echo服务端

```c
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
using namespace std;
int main () {
    int server = 0 ;
    int client = 0 ;
    struct sockaddr_in saddr = {0};
    struct sockaddr_in caddr = {0};
    socklen_t asize = 0;
    int len = 0;
    char buf[32] = {0};
    int  r = 0;
    
    server = socket(PF_INET,SOCK_STREAM,0); /*申请系统资源*/
    
    if (server == -1 ) {
        cout<<"socket error"<<endl;
        return -1;
    }

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY); /*设置服务器IP地址 这里表示 Address to accept any incoming messages. INADDR_ANY真实值为0.0.0.0 
    当然也可如何设置IP  saddr.sin_addr.s_addr = inet_addr("192.168.43.65");*/
    saddr.sin_port = htons(8890); /*设置服务器端口号*/
    if ( bind(server, (struct sockaddr*)&saddr, sizeof(struct sockaddr)) == -1 ) {
        perror("bind");
        cout<<"server bind error"<<endl;
        return -1;
    }
    if ( listen(server,1)==-1 ) {
        perror("listen");
        cout<<"server start error"<<endl;
    }
    cout<<"server start success"<<endl;
    
    while(1) {

    asize = sizeof(caddr);
    client = accept(server,(struct sockaddr*)&caddr, &asize);  /*accept 会阻塞该程序 */
     
 
    if (client == -1) {
        perror("accept");
        cout<<"client acccept error"<<endl;
        return -1;
    }
    
    cout<<"client: "<<client<<endl;
 
    do {
        int i = 0;
        r = recv(client, buf, sizeof(buf), 0); /*每次最多读取buf大小的数据*/
        
        if (r > 0) {
            cout<<"Receive: "<<buf<<endl;
            if (strcmp(buf, "quit") !=0){ /*客户端输入quit断开连接 其他情况原句发给客户端*/
                len  = send (client, buf, r, 0);
            } else {
                break;
            }
        }
    } while (r > 0);
    close(client);
    client = -1;
}

    
    sleep(1); 
   
    close(server);
    server = -1;
    return 0;
}
```

#### 相应代码函数以及知识点补充：

##### 1.socket

```c
int socket(int domain, int type, int protocol);
/*
domain：套接字使用的协议族.常用的协议族有:
PF_INET IPv4互联网协议簇
PF_INET6 IPv6互联网协议簇
PF_LOCAL 本地通信协议簇(进程间通信)
PF_PACKET 底层数据收发协议簇
AF_IPX Novell专用协议簇(互联网分组交换协议)
......
协议族决定了socket的地址类型，在通信中必须采用对应的地址(不同协议的地址表现形式可能不同,网络编程的地址类型必须和协议类型一致)
如AF_INET决定了要用ipv4地址（32位的）与端口号（16位的）的组合。

type：套接字数据传送类型。常用的协议类型有:
SOCK_STREAM 流式数据(TCP)
SOCK_DGRAM 报文式数据(UDP)
......

protocol：设备间通信使用的协议信息.常用的协议有
IPPROTO_TCP TCP传输协议
IPPTOTO_UDP UDP传输协议
IPPROTO_SCTP STCP传输协议
IPPROTO_TIPC TIPC传输协议
.......
因为type && domain 几乎可以唯一确定一种协议，不需要protocol参数了，所以大多数这里写0，即代表type && domain 指定后的默认协议
*/
```

##### 2.端口号和IP

端口号是一个2字节数据(2字节无符号整数)

0-1023作为特定端口被预定义(分配给特定程序,因此尽量使用稍微大的端口号，免得被其他占用)

IP地址一般是一个4字节地址族(5类)

IP地址保存了网络标识和主机标识两部分 前三类如此划分

1. 网络标识：标识网络主机所在的网络   
2. 主机标识：标识网络主机所在的具体地址

IP地址与子网掩码做按位与就可以区分网络标识和主机标识

设子网掩码为 M.N.P.Q  则子网可用IP地址数:
$$
n = (256-M)*(256-N)*(256-P)*(256-Q)
$$
IP 与 子网掩码按位与 可计算出子网地址 以及 广播地址 :
$$
实际可用子网地址= n-2(子网地址+广播地址) 或者更少
$$
我这里举一个例子：

```c
IP地址A: 211.99.34.33  掩码B: 255.255.255.248  
n= 1*1*1*8 = 2^3  Y=32 - 3 =29(二进制中1的个数) 因此简洁表示法：211.99.34.33/29  
A 与 B --------------> 子网地址: 211.99.34.32 广播地址: 211.99.34.39    
```

特殊IP地址:

0.0.0.0/0  保留,常用于表示缺省网络

127.0.0.0/8 回环地址,常用于本地软件回送测试

255.255.255.255/32 广播地址

私有地址(不在公网使用,只在内网使用):

10.0.0.0 - 10.255.255.255/8

172.16.0.0 - 172.31.255.255/16

192.168.0.0 - 192.168.255.255/24

### Select多路复用

Linux

一切皆文件，资源标识符叫做文件描述符号

Windows

资源表示符叫句柄

两者只是叫法不一样，但实质都是一段内存的标识符

```c++
#include<iostream>
#include <unistd.h>
using namespace std;
int main() {
    int iofd = 0; //输入输出设备标识符为0
    char s[] = "Hello World\n";
    write(iofd,s,sizeof(s));
    
    int len =read(iofd,s,5);
    s[len]=0;

    cout<<s<<endl;
    return 0;
}
其他id号可以通过网上获取
```

#### 神奇的select()函数

 select()用于监视指定的文件描述符是否产生

通过轮询的方式检测目标事件(事件产生则标记发生变化)

根据事件类型做出具体处理(如：读取数据)

```c
int select(int maxfd,
           fd_set* readset,
           fd_set* writeset,
           fd_set* exceptset,
           const struct timeval* timeout //轮询间隔时间 )
FD_ZERO(fd_set* fdset)； //将fd_set变量的所有位设置成0
FD_SET(int fd, fd_set* fdset)； //在fd_set中指定需要监听的fd
FD_CLR(int fd, fd_set* fdset)；//在fd_set中删除fd，不再监听
FD_ISSET(int fd, fd_set* fdset)； //在fd_set查看是否包含fd
```



#### 事件相关函数类型

##### 阻塞:

- 阻塞式函数:

  函数调用后需要等待某个事件发生后才会返回

  如scanf read accept ...

- 非阻塞式函数:

  函数调用后能够及时返回(仅标记等待的事件)

  事件发生后以回调方式传递

##### 轮询:

轮询指依序询问每一个相关设备是否需要服务

轮询可用于解决阻塞函数导致程序无法继续执行的问题

##### 回调:

  回调函数就是一个通过函数指针调用的函数。如果你把函数的指针（地址）作为参数传递给另一个函数，当这个指针被用来调用其所指向的函数时，我们就说这是回调函数。回调函数不是由该函数的实现方直接调用，而是在特定的事件或条件发生时由另外的一方调用的，用于对该事件或条件进行响应

### 基于多路复用的服务端

#### 问题分析:

```c
 client = accept(server,(struct sockaddr*)&caddr, &asize);  /*accept 会阻塞该程序 */
 r = recv(client, buf, sizeof(buf), 0); /*每次最多读取buf大小的数据*/
```

  传统的代码采用accept recv等阻塞方式来连接客户端和接受数据,因此在绝大多数情况，服务器端处于阻塞状态，利用率比较低下，迫切需要种解决方案去解决该问题。

#### 解决方案：阻塞变轮询

通过select()函数首先监听服务端server_fd句柄，目标事件为连接

当事件发生(客户端连接)则调用accept()接受连接(避免等待)

将client_fd加入监听范围，目标事件为”数据接收“(读)

循环查看各个被监听的文件描述符是否有事件发生

```c
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
using namespace std;
int server_handler (int server) {
    struct sockaddr_in addr = {0};
    socklen_t asize = sizeof(addr);

    return accept(server,(struct sockaddr*)&addr, &asize);
}
int client_handler (int client) {
    char buf[32] = {0};
    int ret = read(client, buf, sizeof(buf)-1);
    if ( ret > 0) {
        buf[ret] = 0; 
        cout<<buf<<endl;
        if ( strcmp(buf,"quit") != 0) {
            ret = write (client, buf, ret);
        } else {
            ret = -1;
        }
    }
    return ret;
}
int main () {
    int server = 0 ;
    struct sockaddr_in saddr = {0};
    int max = 0;  //最大文件句柄号
    int num = 0;  //用于select()函数返回值，若非0代表有监听的文件资源改变

    fd_set reads = {0};
    fd_set temps = {0};
    struct timeval timeout = {0};
    
    server = socket(PF_INET,SOCK_STREAM,0); /*申请系统资源*/
    
    if (server == -1 ) {
        cout<<"socket error"<<endl;
        return -1;
    }

    saddr.sin_family = AF_INET; /*设置tcp协议簇 这里为ip协议*/
    saddr.sin_addr.s_addr = htonl(INADDR_ANY); /*设置服务器IP地址 */
    saddr.sin_port = htons(8890); /*设置服务器端口号*/
    if ( bind(server, (struct sockaddr*)&saddr, sizeof(struct sockaddr)) == -1 ) {
        perror("bind");
        cout<<"server bind error"<<endl;
        return -1;
    }
    if ( listen(server,1)==-1 ) {   
        perror("listen");
        cout<<"server start error"<<endl;
    }
    cout<<"server start success"<<endl;
    
    FD_ZERO(&reads);
    FD_SET(server,&reads);
    max = server;
    while(1) {
        temps = reads ;

        timeout.tv_sec = 0;
        timeout.tv_usec = 50000;

        num = select(max+1, &temps, 0, 0, &timeout); //文件描述符数量

        if ( num >0 ) {
            int  i = 0;
            for (i = 1; i <=max; ++i) {
                if (FD_ISSET(i, &temps)) {
                    if (i == server) {
                          int client = server_handler(server); //处理客户端连接  返回值即句柄号
                          if ( client > -1) {
                              FD_SET(client, &reads);
                              max = (client > max) ? client : max; //设置句柄最大值
                              cout<<"accept client:" <<client;
                          }
                    } else {
                       int r  = client_handler(i);  //处理客户端请求
                       if ( r == -1 ) {
                           FD_CLR(i, &reads); //断开连接后，需要取消监听该资源
                           close(i);
                       }
                    }
                }
            }
        }
   
    }

    
    sleep(1); 
   
    close(server);
    server = -1;
    return 0;
}
```

###  TCP协议的粘包现象:

   Tcp 虽然有确认重传机制，堵塞控制，能够尽力满足数据的可靠性，但是并不意味着他会保证获取数据的发送方式，对于相互独立的一些数据，因为服务端只是一次性从接受缓冲区读取，所以便会出现粘包现象导致出现一系列本应该独立的数据在服务端的表现形式是有关联的。

```c
/*客户端*/
send(sock, "A", 1, 0);
send(sock, "B", 1, 0);
send(sock, "C", 1, 0);
/*服务端*/
recv(client, buf, sizeof(buf), 0);/*读取结果是ABC,本代表客户端不相关数据，在服务端却联系在了一起
```

解决方法:

 自定义应用层协议

什么是协议？

  协议是通信双方为数据交换而建立的规则、标准或约定的集合

协议的作用：

-   通信双方根据协议能够正确收发数据
-   通信双方能够根据协议解释数据的意义

 完整消息包含：

-   数据头：数据类型(即数据区用途 固定长度) 
-   数据长度: 数据区长度(固定长度)
-   数据区：字符数据(变长区域)

![·](img/Protocol%20Design.jpg)

