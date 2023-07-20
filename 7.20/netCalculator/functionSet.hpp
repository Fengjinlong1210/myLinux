#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;
struct functionSet
{
    static bool splitstrings(const string &str, const string &sep, vector<string> *result)
    {
        // 分割字符串, 将分割的结果放入vector
        int start = 0;
        while (start < str.size())
        {
            int pos = str.find(sep, start);
            if (pos == string::npos)
            {
                break;
            }
            result->push_back(str.substr(start, pos - start));
            start = pos + sep.size();
        }
        // 出循环时, 还剩下最后一个字串
        if (start < str.size())
        {
            result->push_back(str.substr(start));
        }
        return true;
    }

    static int toInt(const string &str)
    {
        return atoi(str.c_str());
    }
};
