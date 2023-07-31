#include <iostream>
#include <cstdio>
#include <climits>
#include <vector>
#include <queue>
using namespace std;
class Solution {
public:
    //快速幂
    double Pow(double x, long long n)
    {
        if(n == 0) return 1.0;
        printf("%d\n", n);

        //每次递归n/2次幂(向下取整), 如果n为偶数无需处理, 如果n为奇数, 需要再多乘一个x
        double y = Pow(x, n / 2);
        //printf("%lf\n", y);

        //例如 2^5 = 2^2 * 2^2 * 2;
        if(n % 2 == 0)  return y * y;
        return y * y * x;
        
        // if(n % 2 != 0)  return y * y * x;
        // return y * y;
    }

    double myPow(double x, int n) 
    {
        long long N = n;
        cout << "N = " << N << endl;
        if(N >= 0)
        {
            cout << "N >= 0"<< endl;
            Pow(x, N);
        }
        else
        {
            cout << "N < 0"<< endl;
            Pow(x, -N);
        }
    }
};

int main()
{
    Solution s;
    int n = INT_MIN;
    cout << "INT_MIN = " << INT_MIN << endl;
    double x = 2.0;
    auto ret = s.myPow(x, n);
    cout << "ret = " << ret << endl;
    return 0;
}
