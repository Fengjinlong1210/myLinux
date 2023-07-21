#include <memory>
#include "cal_server.hpp"
using namespace Fjl;
Response calculate(const Request &req)
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

int main()
{
    unique_ptr<tcp_server> p_server(new tcp_server(calculate, 8888));
    p_server->init_server();
    p_server->start_server();
    return 0;
}