#include "udp_client.hpp"
using namespace std;

void Usage()
{
    cout << "Usage : "
         << "udp_client clientip clientport" << endl;
}

int main(int argc, char **args)
{
    if (argc != 3)
    {
        Usage();
    }
    string server_ip = args[1];
    uint16_t server_port = atoi(args[2]);
    // 1. 明确要发送信息的服务器

    // 客户端不需要自己手动bind ip和端口号, OS会在第一次调用系统调用发送信息时自动bind, 然后发送数据报
    // 因为要防止客户端出现端口号冲突
    // 但是服务器端的ip和端口号必须要手动绑定, 且不能随意改变, 因为要指明唯一的服务器

    // 2. 创建套接字
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cout << "create socket error : " << strerror(errno) << std::endl;
        exit(SOCKET_ERROR);
    }
    sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server.sin_port = htons(server_port);
    server.sin_family = AF_INET;

    // 3. 发送信息并接受
    char buffer[2048];

    while (1)
    {
        // 获取输入
        string message;
        cout << "please enter: ";
        getline(cin, message);
        // cin >> message;
        //  向服务器发送
        sendto(sock, message.c_str(), message.size(), 0, (sockaddr *)&server, sizeof(server));

        // 接受服务器返回的值
        struct sockaddr_in temp;
        socklen_t length = sizeof(temp);
        // cout << "recvfrom" << endl;
        // cout << "sock = " << sock << endl;
        int n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&temp, &length);
        //int n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
        cout << "n = " << n << endl;
        if (n > 0)
        {
            buffer[n] = 0;
            cout << "server echo: " << buffer << endl;
        }
    }
    return 0;
}