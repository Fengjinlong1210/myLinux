#include <iostream>
#include <string>
#include "udp_server.hpp"
using namespace std;
using namespace Fjl;

void usage()
{
    cout << "Usage : ./udp_server port" << endl;
}

string handle_func1(string str)
{
    cout << "str = " << str << endl;
    string ret;
    char c;
    for(auto ch : str)
    {
        if(ch >= 'a' && ch <= 'z')
        {
            c = toupper(ch);
        }
        else
        {
            c = ch;
        }
        ret += c;
    }
    cout << ret << endl;
    return ret;
}

string handle_func2(string command)
{
    FILE* res = popen(command.c_str(), "r");
    char buffer[1024];
    string ret;
    while(fgets(buffer, sizeof(buffer), res))
    {
        ret += buffer;
    }
    pclose(res);
    return ret;
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        usage();
    }
    uint16_t port = atoi(argv[1]);
    //Fjl::udp_server server(handle_func1, port);
    Fjl::udp_server server(handle_func2, port);
    server.server_init();
    server.server_start();
    return 0;
}