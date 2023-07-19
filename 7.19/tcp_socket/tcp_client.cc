#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include "error.hpp"
using namespace std;

void Usage()
{
    cout << "Usage : "
         << "tcp_client IP port" << endl;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        Usage();
        exit(USAGE_ERR);
    }
    // 获取服务器端IP
    string server_ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cout << "create socket error: " << strerror(errno) << std::endl;
        exit(SOCKET_ERROR);
    }
    //客户端不需要自己绑定

    //连接服务器端
    sockaddr_in server;
    memset(&server, 0, sizeof(server));
    inet_aton(server_ip.c_str(), &server.sin_addr); //直接把i写入server结构体
    server.sin_port = htons(port);
    server.sin_family = AF_INET;

    int cnt = 5;
    while(connect(sock, (sockaddr*)&server, sizeof(server)) != 0)
    {
        sleep(1);
        cout << "连接失败, 正在尝试重连 :" << cnt-- << endl;
        if(cnt <= 0) break;
    }
    if(cnt <= 0)
    {
        cerr << "连接失败" << endl;
        exit(CONNECT_ERROR);
    }
    char buffer[2048];
    while(1)
    {
        //输入数据
        string str;
        cout << "请输入: ";
        getline(cin, str);
        write(sock, str.c_str(), str.size());

        int n = read(sock, buffer, sizeof(buffer));
        if(n > 0)
        {
            cout << "server: " << buffer << endl;
        }
        else if(n == 0)
        {
            cout << "server quit" << endl;
            break;
        }
        else
        {
            cout << "read error" << endl;
            break;
        }
    }
    close(sock);
    return 0;
}