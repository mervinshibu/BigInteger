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
    void add(BigInt &a, long int b)
    {
        BigInt B(b);
        add(a,B);
    }

    void add(BigInt &a, string b)
    {
        BigInt B(b);
        add(a,B);
    }

    void add(BigInt a, BigInt b)
    {
        long l, o1,o2;
        this -> length = (a.length > b.length)?a.length:b.length;
        l = length;
        BigInt *t = NULL;
        ++length;
        long carry = 0,i;
        val = new long[length];
        for(long i = 0; i < length; ++i)
        {
            if(i < a.length)
                o1 = a.val[i];
            else o1 = 0;
            if(i < b.length)
                o2 = b.val[i];
            else o2 = 0;
            val[i] = o1 + o2 + carry;
            carry = val[i]/10;
            val[i] = mod(val[i],10);
        }
        while(val[length - 1] == 0)
            --length;
        short int sign = (val[length - 1] < 0)?-1:1;
        val[0] = val[0] + (10 * sign);
        carry = val[0]/10;
        val[0] = mod(val[0],10);
        for(i = 1; i < length - 1; ++i)
        {
            val[i] = val[i] + (9 * sign) + carry;
            carry = val[i] / 10;
            val[i] = mod(val[i],10);
        }
        val[i] = val[i] - sign + carry;
        if(val[i] == 0)
        --length;
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
};


int main()
{

    BigInt a("1024");
    BigInt b("-768");
    BigInt c;
    c.mul(a,b);
    c.display();
    return 0;
}
