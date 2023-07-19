#include "tcp_server.hpp"
#include <memory>
using namespace Fjl;
using namespace std;

string handle(const string str)
{
    return str;
}

int main()
{
    unique_ptr<tcp_server> ptr_tcp(new tcp_server(handle, 8888));
    ptr_tcp->server_init();
    ptr_tcp->server_start();
    return 0;
}