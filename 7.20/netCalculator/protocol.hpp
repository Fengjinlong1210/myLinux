#pragma once
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include "functionSet.hpp"
#include "jsoncpp/json/json.h"
// #define MYPROTOCOL 0
using namespace std;

namespace protocol
{
#define SEP " "
#define SEP_LEN strlen(SEP)
#define HEAD_SEP "\r\n"
#define HEAD_LEN strlen(HEAD_SEP)

    int readPackage(int sock, string &inbuffer, string *package)
    { // 大于0表示读取成功, 小于0读取失败, 等于0读取不完整, 需要继续读取
        // 从sock中读取数据放到inbuffer中, 如果数据完整就存入package
        // 否则就继续读取
        cout << "read 之前 inbuffer: " << inbuffer << endl;
        char buffer[2048];
        int n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0)
            return -1; // 读取错误
        logMessage(Debug, "return 1");
        buffer[n] = 0;
        inbuffer += buffer; // 把新读取到的内容放在外面的缓冲区中
        cout << "read 中 inbuffer: " << inbuffer << endl;

        auto pos = inbuffer.find(HEAD_SEP);
        if (pos == string::npos)
        {
            logMessage(Debug, "not fount HEAD_SEP");
            return 0;
        }
        logMessage(Debug, "return 2");

        // 创建一个字符串用来表示有效载荷的长度
        string len_str = inbuffer.substr(0, pos);
        int len_payload = functionSet::toInt(len_str); // 该整数是有效载荷的长度
        int len_targetPackage = len_payload + len_str.size() + HEAD_LEN * 2;
        if (len_targetPackage > inbuffer.size())
            return 0; // 读取不完整

        // 将完整的报文通过输出型参数输出
        *package = inbuffer.substr(0, len_targetPackage);
        inbuffer.erase(0, len_targetPackage);
        cout << "read 之后 inbuffer: " << inbuffer << endl;

        return len_payload; // 返回有效载荷长度, 让外面可以截取有效载荷
    }

    string removeHeader(const string &package, int length)
    {
        // package = HEAD + \r\n + content + \r\n
        // 截取中间的content作为返回值
        int n = package.size();
        string ret = package.substr(n - length - HEAD_LEN, length);
        return ret;
    }

    string addHeader(const string &str)
    {
        string ret = to_string(str.size());
        ret += HEAD_SEP + str + HEAD_SEP;
        return ret;
    }

    class Request // 对请求进行序列化与反序列化
    {
    public:
        Request() {}
        Request(int x, int y, char op)
            : _x(x), _y(y), _op(op)
        {
        }

        bool serialize(string *outStr) // struct->string
        {
            *outStr = "";
#ifdef MYPROTOCOL
            string x_str = to_string(_x);
            string y_str = to_string(_y);

            *outStr = x_str + SEP + _op + SEP + y_str;
            cout << "序列化后" << *outStr << endl;
#else
            Json::Value root;
            // 建立kv
            root["x"] = _x;
            root["y"] = _y;
            root["op"] = _op;
            // 序列化
            Json::StyledWriter writer;
            *outStr = writer.write(root);
#endif
            return true;
        }

        bool deserialize(string &instr) // string->struct
        {
#ifdef MYPROTOCOL
            vector<string> v;
            functionSet::splitstrings(instr, SEP, &v);
            if (v.size() != 3)
                return false;
            if (v[1].size() != 1)
                return false;
            _x = functionSet::toInt(v[0]);
            _y = functionSet::toInt(v[2]);
            _op = v[1][0];
#else
            Json::Value root;
            Json::Reader reader;
            reader.parse(instr, root);
            // 读取
            _x = root["x"].asInt();
            _y = root["y"].asInt();
            _op = root["op"].asInt();
#endif
            return true;
        }

    public:
        int _x;
        int _y;
        char _op;
    };

    class Response
    {
    public:
        Response() {}
        Response(int result, int code)
            : _result(result), _code(code)
        {
        }

        bool serialize(string *outStr) // result/code->string
        {
            *outStr = "";
#ifdef MYPROTOCOL

            string res_str = to_string(_result);
            string code_str = to_string(_code);

            *outStr = res_str + SEP + code_str;
#else
            Json::Value root;
            // 建立kv
            root["res"] = _result;
            root["code"] = _code;
            // 序列化
            Json::StyledWriter writer;
            *outStr = writer.write(root);
#endif
            return true;
        }

        bool deserialize(string &instr) // string->result/code
        {
#ifdef MYPROTOCOL
            vector<string> v;
            functionSet::splitstrings(instr, SEP, &v);
            if (v.size() != 2)
                return false;

            _result = functionSet::toInt(v[0]);
            _code = functionSet::toInt(v[1]);
#else
            Json::Value root;
            Json::Reader reader;
            reader.parse(instr, root);

            _result = root["res"].asInt();
            _code = root["code"].asInt();

#endif
            return true;
        }

    public:
        int _result;
        int _code;
    };
}
