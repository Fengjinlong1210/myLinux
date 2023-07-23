#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "http_server.hpp"
#include "funcSet.hpp"

using namespace std;
const string SEP = "\r\n";
const string webRoot = "./wwwroot"; // 根目录
const string notfound = "404.html";
const string defaultPage = "index.html"; //  每个目录下的默认界面

class HttpRequest       //提取的网络请求
{
public:
    HttpRequest(string homepage = webRoot)
        : _path(homepage)
    {
    }
    void print()
    {
        logMessage(Debug, "request--method: %s, url: %s, version: %s",\
        _method.c_str(), _url.c_str(), _http_version.c_str());
        logMessage(Debug, "-path: %s", _path.c_str());
    }
public:
    string _method;     //方法
    string _url;        //
    string _http_version;
    vector<string> _body; 
    string _path;       //客户端要访问的路径
    string _suffix;
};

HttpRequest Desarialize(string &message)
{
    //对客户端请求进行反序列化

    HttpRequest req;
    // 按照分隔符读取一行输入
    string line = funcSet::readLine(message, SEP);
    // 将一行输入按照空格分割
    // 读取第一行输入
    funcSet::separateString(line, &req._method, &req._url, &req._http_version);

    //填充请求信息
    while (!message.empty())        //将剩下的读取完
    {
        line = funcSet::readLine(message, SEP);
        req._body.push_back(line);
    }
    req._path += req._url;  //path默认添加了web根目录
    if (req._path[req._path.size() - 1] == '/')
    {
        req._path += defaultPage;   //如果客户访问的是目录, 就自动添加一个默认主页
                                    //不能让用户随意访问web目录
    }
    auto pos = req._path.rfind('.');
    if (pos == string::npos)
    {
        req._suffix = ".html";
    }
    else
    {
        req._suffix = req._path.substr(pos);    //根据要访问的路径判断文件后缀
    }
    return req;
};

string getContentType(const string &suffix)
{
    string content_type = "Content_Type: ";
    if (suffix == ".html" || suffix == ".htm")
        content_type += "text/html";
    else if (suffix == ".jpg")
        content_type += "image/jpeg";
    return content_type + SEP;
}

string handlerHttpRequest(string message) // 该函数用来处理请求
{
    // 1. 接受服务器发送过来的http请求
    cout << "------------------------" << endl;
    // cout << message << endl;

    // 不能把网页文件写在代码中, 要通过接口获取
    // string body = "<html> <body> <h1>hahahaha</ h1></ body></ html>";

    // 2. 对请求进行反序列化, 提取请求中的信息
    HttpRequest req = Desarialize(message);
    req.print();

    // 服务器回应的主体, 有效载荷
    string body;
    // 以字符串形式返回
    string response;
    if (funcSet::readFile(req._path, &body)) // 如果客户端要读取的文件存在
    {
        response = "HTTP/1.0 200 OK" + SEP;     //根据请求返回不同的状态
    }
    else
    {
        response = "HTTP/1.0 404 Not Found" + SEP;  
    }

    // 3. 响应的一些基本信息, 要添加有效载荷的长度, 响应的类型, 再添加分割符和有效载荷
    response += "Content-Length: " + to_string(body.size()) + SEP; 
    response += getContentType(req._suffix);
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