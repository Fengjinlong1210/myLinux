#include <iostream>
using namespace std;
enum err
{
    USAGE_ERR = 1,
    SOCKET_ERROR,
    BIND_ERROR,
    LISTEN_ERROR,
    CONNECT_ERROR,
    SETSID_ERROR
};