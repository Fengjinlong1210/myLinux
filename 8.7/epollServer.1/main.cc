#include <iostream>
#include <memory>
#include "log.hpp"
#include "epollServer.hpp"
using namespace std;

string handler(string request)
{
    string response;
    response += "[echo]:";
    response += request + "\r\n";
    return response;
}

int main()
{
    unique_ptr<epollServer> p_svr(new epollServer(handler));
    p_svr->init_server();
    p_svr->start_server();
    return 0;
}
