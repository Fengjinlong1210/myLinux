#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "http_server.hpp"
#include "funcSet.hpp"

using namespace std;
const string SEP = "\r\n";
const string webRoot = "./wwwroot"; //根目录
const string defaultHomePage = "index.html";    //  每个目录下的默认界面

class HttpRequest
{
public:
    HttpRequest(string homepage = webRoot)
        : _path(homepage)
    {
    }

    void print()
    {
        logMessage(Debug, "request--method: %s, url: %s, version: %s", _method.c_str(), _url.c_str(), _http_version.c_str());

        // for(const auto& str : _body)
        // {
        //     logMessage(Debug, "--%s", str.c_str());
        // }
        // cout << "end" << endl;
        logMessage(Debug, "-path: %s", _path.c_str());
    }

public:
    string _method;
    string _url;
    string _http_version;
    vector<string> _body;
    string _path;
    string _suffix;
};

HttpRequest Desarialize(string &message)
{
    HttpRequest req;
    // 按照分隔符读取一行输入
    string line = funcSet::readLine(message, SEP);
    // 将一行输入按照空格分割
    funcSet::separateString(line, &req._method, &req._url, &req._http_version);

    while (!message.empty())
    {
        line = funcSet::readLine(message, SEP);
        req._body.push_back(line);
    }
    req._path += req._url;
    if (req._path[req._path.size() - 1] == '/')
    {
        req._path += defaultHomePage;
    }
    auto pos = req._path.rfind('.');
    if(pos == string::npos) 
    {
        req._suffix = ".html";
    }
    else
    {
        req._suffix = req._path.substr(pos);
    }
    return req;
}

string getContentType(const string& suffix)
{
    string content_type = "Content_Type: ";
    if(suffix == ".html" || suffix == ".htm") 
        content_type += "text/html";
    else if(suffix == ".jpg")
        content_type += "image/jpeg";
    return content_type;
}

string handlerHttpRequest(string message)
{
    cout << "------------------------" << endl;
    // cout << message << endl;

    // 不能把网页文件写在代码中, 要通过接口获取
    // string body = "<html> <body> <h1>hahahaha</ h1></ body></ html>";
    HttpRequest req = Desarialize(message);
    req.print();
    string body;
    // string path = webRoot;
    // path += req._url;
    //cout <<  req._path.c_str() << endl;
    funcSet::readFile(req._path, &body);
    string response = "HTTP/1.0 200 OK" + SEP;
    response += "Content-Length: " + to_string(body.size()) + SEP;
    response += getContentType(req._suffix);
    response += SEP;

    //cout << response << endl;
    //response += "Content-Type: text/html" + SEP;
    // response += "Accept-Encoding: gzip, deflate" + SEP;
    // response += "Accept-Language: zh-CN,zh;q=0.9" + SEP;
    response += SEP;
    response += body;
    return response;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "Usage error" << endl;
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    unique_ptr<httpServer> p_svr(new httpServer(handlerHttpRequest, port));
    p_svr->init_server();
    p_svr->start_server();
    return 0;
}