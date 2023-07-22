#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "log.hpp"
using namespace std;

namespace funcSet
{
    bool readFile(const string &path, string *fileContent)
    {
        // 1. 读取文件大小
        struct stat fileStat; // 输出型参数
        int n = stat(path.c_str(), &fileStat);
        if (n < 0)
        {
            logMessage(Error, "read html file failed, code: %d, error msg: %s", errno, strerror(errno));
            return false;
        }
        int fileSize = fileStat.st_size;
        // 2. 给字符串预留空间, 网页文件中不止有文本, 也有二进制内容
        fileContent->resize(fileSize);
        // 3. 从文件中读取内容到字符串中
        int fd = open(path.c_str(), O_RDONLY);
        if (fd < 0)
        {
            logMessage(Error, "open html file failed, code: %d, error msg: %s", errno, strerror(errno));
            return false;
        }
        read(fd, (char *)fileContent->c_str(), fileSize);
        close(fd);
        logMessage(Info, "read html file success");

        return true;
    }

    string readLine(string &message, const string &sep)
    {
        // 读取到第一行以后, 删除第一行
        auto pos = message.find(sep);
        if (pos == string::npos)
        {
            return "";
        }
        string ret = message.substr(0, pos);
        message.erase(0, pos + sep.size());
        return ret;
    }

    bool separateString(const string &line, string *method, string *url, string *version)
    {
        stringstream ss(line);
        ss >> *method >> *url >> *version;
        return true;
    }
}