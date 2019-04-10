#include <iostream>
using namespace std;
long mod(long a, long b)
{
    if(a < 0)
        return -(-a % b);
    else return a % b;
}
class BigInt{
    long *val;
    long length;
public:
    BigInt()
    {
        val = NULL;
        length = 0;
    }
    BigInt(long n)
    {
        int sign = 1;
        if(n < 0)
        {
            sign = -1;
            n *= sign;
        }
        int t = 10;
        for(length = 1; t <= n; ++length )
        t = t * 10;
        this->val = new long[length];
        t/=10;
        for(int i = 0; i < length; i++)
        {
            this->val[i] = n % 10 * sign;
            n = n / 10;
        }
    }
    BigInt(string s)
    {
        short int sign = 1;
        if(s[0] == '-')
            sign = -1;
        for(length = 0; s[length] != 0; length++);
            this->val = new long[length];
        for(long i = length - 1; i >= 0; --i)
        {
            if(s[i] == '-')
            {
                --length;
                break;
            }
            val[length - 1 - i] = (s[i] - '0') * sign;
        }
        while(val[length - 1] == 0)
            --length;
    }

    BigInt(const BigInt &num)
    {
        this -> length = num.length;
        this -> val = new long[length];
        for(long i = 0; i < length; i++)
        val[i] = num.val[i];
    }
    void display()
    {
        if(length == 0)
        {
            cout << "0\n";
            return;
        }
        cout << val[length - 1];
        short int sign = (val[length -1] < 0)?-1:1;
        for(long i = length - 2; i >= 0; i--)
        cout << val[i] * sign;
        cout << endl;
    }

    void mul(BigInt a, long b)
    {
        BigInt B(b);
        mul(a,B);
    }

    void mul(BigInt a, string b)
    {
        BigInt B(b);
        mul(a,B);
    }
    void mul(BigInt a, BigInt b)
    {
        length = a.length + b.length;
        val = new long[length];
        long carry = 0;
        long i, j;
        for(i = 0; i < b.length;i++)
        {
            carry = 0;
            for(j = 0; j < a.length; j++)
            {
                val[i + j] += carry + (b.val[i] * a.val[j]);
                carry = val[i + j] / 10;
                val[i + j] = mod(val[i + j], 10);
            }
            val[i + j] = carry;
        }
        while(val[length - 1] == 0)
        --length;

    }

    void pl()
    {
        cout << length << endl;
    }

    void operator = (string s)
    {
        (*this) = BigInt(s);
    }
    void operator = (long n)
    {
        (*this) = BigInt(n);
    }
    friend istream& operator >> ( istream &din, BigInt &a)
    {
        string s;
        din >> s;
        a = BigInt(s);
        return din;
    }

    friend ostream& operator << (ostream &dout, BigInt &a)
    {
        if(a.length == 0)
        {
            cout << "0";
            return dout;
        }
        cout << a.val[a.length - 1];
        short int sign = (a.val[a.length -1] < 0)?-1:1;
        for(long i = a.length - 2; i >= 0; i--)
        cout << a.val[i] * sign;
        return dout;
    }

    friend BigInt operator + (BigInt a, BigInt b)
    {
        BigInt sum;
        long l, o1,o2;
        sum.length = (a.length > b.length)?a.length:b.length;
        l = sum.length;
        BigInt *t = NULL;
        ++sum.length;
        long carry = 0,i;
        sum.val = new long[sum.length];
        for(long i = 0; i < sum.length; ++i)
        {
            if(i < a.length)
                o1 = a.val[i];
            else o1 = 0;
            if(i < b.length)
                o2 = b.val[i];
            else o2 = 0;
            sum.val[i] = o1 + o2 + carry;
            carry = sum.val[i]/10;
            sum.val[i] = mod(sum.val[i],10);
        }
        while(sum.val[sum.length - 1] == 0)
            --sum.length;
        short int sign = (sum.val[sum.length - 1] < 0)?-1:1;
        sum.val[0] = sum.val[0] + (10 * sign);
        carry = sum.val[0]/10;
        sum.val[0] = mod(sum.val[0],10);
        for(i = 1; i < sum.length - 1; ++i)
        {
            sum.val[i] = sum.val[i] + (9 * sign) + carry;
            carry = sum.val[i] / 10;
            sum.val[i] = mod(sum.val[i],10);
        }
        sum.val[i] = sum.val[i] - sign + carry;
        if(sum.val[i] == 0 && sum.length > 1)
        --sum.length;
        return sum;
    }
    friend BigInt operator+( BigInt &a, long b)
    {
        BigInt B = b;
        return a+B;
    }
    friend BigInt operator+( BigInt &a, string b)
    {
        BigInt B = b;
        return a+B;
    }

    friend BigInt operator*(BigInt a, BigInt b)
    {
        BigInt s;
        s.length = a.length + b.length;
        s.val = new long[s.length];
        long carry = 0;
        long i, j;
        for(i = 0; i < b.length;i++)
        {
            carry = 0;
            for(j = 0; j < a.length; j++)
            {
                s.val[i + j] += carry + (b.val[i] * a.val[j]);
                carry = s.val[i + j] / 10;
                s.val[i + j] = mod(s.val[i + j], 10);
            }
            s.val[i + j] = carry;
        }
        while(s.val[s.length - 1] == 0)
        --s.length;
        return s;
    }

    friend BigInt operator* (BigInt &a, long b)
    {
        BigInt B = b;
        return a*B;
    }

    friend BigInt operator* (BigInt &a, string b)
    {
        BigInt B = b;
        return a*B;
    }
};


int main()
{

    BigInt a = 1;
    BigInt b = 3;

    for(int i = 1; i <= 1000 ; ++i)
    {
        a = a * i;
    }

    cout << a << endl;
    return 0;
}
