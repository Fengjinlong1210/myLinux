#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <memory>
#include "log.hpp"
#include "error.hpp"
#include "socket.hpp"
#include "protocol.hpp"
using namespace std;
using namespace protocol;
enum
{
    Left,
    Operator,
    Right
};

void Usage()
{
    cout << "Usage : client IP port" << endl;
}

Request splitFormula(const string &formula)
{
    string left, right;
    char op;
    int i = 0;
    int stat = Left;
    while(i < formula.size())
    {
        switch (stat)
        {
        case Left:
            if (isdigit(formula[i]))
                left.push_back(formula[i++]);
            else
                stat = Operator;
            break;
        case Operator:
            op = formula[i++];
            stat = Right;
            break;
        case Right:
            if (isdigit(formula[i]))
                right.push_back(formula[i++]);
            break;
        default:
            break;
        }
    }
    Request req;
    req._x = functionSet::toInt(left);
    req._y = functionSet::toInt(right);
    req._op = op;
    //printf("%d%c%d\n", req._x, req._op, req._y);

    return req;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        Usage();
    }
    string server_ip = argv[1];
    uint16_t server_port = atoi(argv[2]);

    sock client;
    client.Socket();
    int n = client.Connect(server_ip, server_port);
    if (n != 0)
    {
        logMessage(Fatal, "connect server:[%s-%d] failed, code : %d, error message: %s", server_ip, server_port, errno, strerror(errno));
        exit(CONNECT_ERROR);
    }
    int client_sock = client.get_socket();
    // 循环式读取输入
    string buffer; // 缓冲区, 用于接受服务器发送的响应
    while (1)
    {
        // 1.获取客户端输入
        string formula;
        getline(cin, formula);
        Request req = splitFormula(formula);
        // cout << "num1: ";
        // cin >> req._x;
        // cout << "num2: ";
        // cin >> req._y;
        // cout << "operator: ";
        // cin >> req._op;
        //printf("%d%c%d\n", req._x, req._op, req._y);
        // 2. 读取输入后进行处理
        string send_str;
        req.serialize(&send_str);

        // 3. 添加报头并发送
        send_str = addHeader(send_str);
        send(client_sock, send_str.c_str(), send_str.size(), 0);

        // 4.获取服务器的响应
        string package;
    startRead:
        int n = readPackage(client_sock, buffer, &package);
        if (n == -1)
        {
            break;
            logMessage(Error, "server quit");
        }
        else if (n == 0)
        {
            goto startRead;
        }
        else // n就是有效载荷的长度
        {
            // 5.读取成功后, 去掉报头
            package = removeHeader(package, n);

            // 6.反序列化并输出
            Response resp;
            resp.deserialize(package);
            cout << "result: " << resp._result << ", code: " << resp._code << endl;
        }
    }
    client.Close();
    return 0;
}