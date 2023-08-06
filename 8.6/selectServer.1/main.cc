#include <iostream>
#include <memory>
#include "log.hpp"
#include "SelectServer.hpp"
using namespace std;

int main()
{
    unique_ptr<SelectServer> p_svr(new SelectServer);
    p_svr->init_server();
    p_svr->start_server();
    cout << sizeof(fd_set) * 8 << endl;
    return 0;
}
