#include <iostream>
#include <string>
#include "udp_server.hpp"
using namespace std;
using namespace Fjl;

void usage()
{
    cout << "Usage : ./udp_server port" << endl;
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        usage();
    }
    uint16_t port = atoi(argv[1]);
    Fjl::udp_server server(port);
    server.server_init();
    server.server_start();
    return 0;
}