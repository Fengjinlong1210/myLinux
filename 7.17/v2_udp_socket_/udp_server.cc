#include <iostream>
#include <string>
#include <memory>
#include "udp_server.hpp"
using namespace std;
using namespace Fjl;

void usage()
{
    cout << "udp_server port" << endl;
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        usage();
    }
    uint16_t port = atoi(argv[1]);
    
    unique_ptr<udp_server> p_server(new udp_server(port));

    p_server->server_start();
    return 0;
}