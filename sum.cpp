#include <iostream>
using namespace std;

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
        int t = 10;
        for(length = 1; t <= n; ++length )
        t = t * 10;
        this->val = new long[length];
        t/=10;
        for(int i = 0; i < length; i++)
        {
            this->val[i] = n % 10;
            n = n / 10;
        }
    }
    /*BigInt(char *s)
    {
        for(length = 0; s[l] != 0; l++);
        this->val = new long(length);
        for(int i = 0; i < length; i++)
        val[i] = s[l - 1 - i];
    }*/
    BigInt(string s)
    {
        for(length = 0; s[length] != 0; length++);
            this->val = new long[length];
        for(long i = 0; i < length; i++)
            val[i] = s[length - 1 - i] - '0';
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
        for(long i = length - 1; i >= 0; i--)
        cout << val[i];
        cout << endl;
    }
    /*void add(BigInt &a, long int b)
    {
        //BigInt s;
        int t = 10, l,i,carry = 0;
        for(l = 1; t < b; ++l )
            t = t * 10;
        length = (a.length > l)?a.length:l;
        ++length;
        val = new long[length];
        for(i = 0; i < a.length; i++)
        {
            val[i] = a.val[i] + (b%10) + carry;
            carry = val[i] /10;
            val[i] = val[i] % 10;
            b = b / 10;
        }
        while(b > 0)
        {
            val[i] = carry + (b % 10);
            carry = val[i]/10;
            val[i] = val[i] % 10;
            b = b / 10;
            ++i;
        }
        if(carry != 0)
            val[i] = carry;
        else
            --length;
    }*/

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
        this -> length = (a.length > b.length)?a.length:b.length;
        BigInt *t = NULL;
        ++length;
        long carry = 0;
        val = new long[length];
        long l = (a.length < b.length)?a.length:b.length, i;
        for(i = 0; i < l; i++)
        {
            val[i] = a.val[i] + b.val[i] + carry;
            carry = val[i] / 10;
            val[i] = val[i] % 10;
        }
        if(i != a.length)
            t = &a;
        else if( i != b.length)
            t = &b;
        if(t == NULL)
        {
            this -> val[i] = carry;
            if(val[i] == 0)
                --length;
            return;
        }
        while(i < length - 1)
        {
            val[i] = t->val[i] + carry;
            carry = val[i] / 10;
            val[i] = val[i] % 10;
            ++i;
        }
        if(carry == 0)
            --length;
        else
            val[i] = carry;
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
                val[i + j] = val[i + j] % 10;
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
//int max(long a, long b){return (a > b)?a:b;}


int main()
{

    BigInt c(956);
    //a.display();
    BigInt b(234);
    c.mul(c,b);
    c.pl();

    //c.display();

    //c.mul(c,9);
    c.display();

    return 0;
}
