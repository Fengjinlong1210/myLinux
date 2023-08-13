#include <iostream>
#include <memory>
#include "log.hpp"
#include "epollServer.hpp"
#include "protocol.hpp"
using namespace protocol;
using namespace std;

Response calculateHandler(const Request &req)
{
    Response resp(0, 0);
    switch (req._op)
    {
    case '+':
        resp._result = req._x + req._y;
        break;
    case '-':
        resp._result = req._x - req._y;
        break;
    case '*':
        resp._result = req._x * req._y;
        break;
    case '/':
        if (req._y == 0)
        {
            resp._code = 1;
        }
        else
        {
            resp._result = req._x / req._y;
        }
        break;
    case '%':
        if (req._y == 0)
        {
            resp._code = 2;
        }
        else
        {
            resp._result = req._x % req._y;
        }
        break;
    default:
        resp._code = 3;
        break;
    }
    return resp;
}

void calculate(connection* conn, const Request& request)
{
    Response resp = calculateHandler(request);
    // 对响应进行序列化
    string sendStr;
    resp.serialize(&sendStr);
    // 添加报头
    sendStr = addHeader(sendStr);
    
    // 将结果放入连接对应的发送缓冲区
    conn->_outbuffer += sendStr;
    conn->_r_ptr->EnableReadWrite(conn, true, true);
}

int main()
{
    unique_ptr<epollServer> p_svr(new epollServer(calculate));
    p_svr->Init_Server();
    p_svr->Dispatcher();
    return 0;
}

// int main()
// {
//     printf("%u\n", 1u << 31);
// }