#pragma once
#include <iostream>
#include <string>
using namespace std;

class Task
{
public:
    Task() // 默认构造函数, 消费者需要使用
    {
    }

    Task(int num1, int num2, char op)
        : _num1(num1), _num2(num2), _operator(op), _result(0), _exitcode(0)
    {
    }

    int operator()()
    {
        switch (_operator)
        {
        case '+':
            _result = _num1 + _num2;
            break;
        case '-':
            _result = _num1 - _num2;
            break;
        case '*':
            _result = _num1 * _num2;
            break;
        case '/':
            if (_num2 == 0)
            {
                _exitcode = -1;
            }
            else
            {
                _result = _num1 / _num2;
            }
            break;
        case '%':
            if (_num2 == 0)
            {
                _exitcode = -2;
            }
            else
            {
                _result = _num1 % _num2;
            }
            break;
        default:
            break;
        }
    }

    string formatArgs()
    {
        string args = to_string(_num1) + _operator + to_string(_num2) + '=';
        return args;
    }

    string formatResult()
    {
        string Result = to_string(_result) + " (" + to_string(_exitcode) + ")";
        return Result;
    }

private:
    int _num1;
    int _num2;
    char _operator;
    int _result;
    int _exitcode;
};