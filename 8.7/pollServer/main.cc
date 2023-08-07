#include <iostream>
#include <memory>
#include "log.hpp"
#include "PollServer.hpp"
using namespace std;

int main()
{
    unique_ptr<PollServer> p_svr(new PollServer);
    p_svr->init_server();
    p_svr->start_server();
    cout << sizeof(fd_set) * 8 << endl;
    return 0;
}
