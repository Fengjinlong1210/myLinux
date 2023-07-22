#include <memory>
#include "http_server.hpp"

using namespace std;

string handlerHttpRequest(string request)
{
    return "hello";
}

int main(int argc, char** argv)
{
    // if(argc != 2)
    // {
    //     cout << "Usage error" << endl;
    //     exit(USAGE_ERR);
    // }
    unique_ptr<httpServer> p_svr(new httpServer(handlerHttpRequest, 8888));
    p_svr->init_server();
    p_svr->start_server();
    return 0;
}