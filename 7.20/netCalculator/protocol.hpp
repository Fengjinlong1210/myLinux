#pragma once
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include "functionSet.hpp"
using namespace std;

namespace protocol
{
    #define SEP " "
    #define SEP_LEN strlen(SEP)
    class Request       //对请求进行序列化与反序列化
    {
    public:
        Request() {}
        Request(int x, int y, char op)
            : _x(x), _y(y), _op(op)
        {
        }

        bool serialize(string* outStr)   //struct->string
        {
            *outStr = "";
            string x_str = to_string(_x);
            string y_str = to_string(_y);

            *outStr = x_str + SEP + _op + SEP + y_str;
            return true;
        }

        bool deserialize(string& instr)     //string->struct
        {
            vector<string> v;
            functionSet::splitstrings(instr, SEP, &v);
            if(v.size() != 3) return false;
            if(v[1].size() != 1) return false;
            _x = functionSet::toInt(v[0]);
            _y = functionSet::toInt(v[2]);
            _op = v[1][0];
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

        bool serialize(string* outStr)   //result/code->string
        {
            *outStr = "";
            string res_str = to_string(_result);
            string code_str = to_string(_code);

            *outStr = res_str + SEP + code_str;
            return true;
        }

        bool deserialize(string& instr)    //string->result/code
        {
            vector<string> v;
            functionSet::splitstrings(instr, SEP, &v);
            if(v.size() != 2) return false;

            _result = functionSet::toInt(v[0]);
            _code = functionSet::toInt(v[1]);
            return true;
        }
    public:
        int _result;
        int _code;
    };
}
