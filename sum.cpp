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
        for(length = 1; t < n; ++length )
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
        for(int i = 0; i < length; i++)
            val[i] = s[length - 1 - i] - '0';
    }

    BigInt(const BigInt &num)
    {
        this -> length = num.length;
        this -> val = new long[length];
        for(int i = 0; i < length; i++)
        val[i] = num.val[i];
    }
    void display()
    {
        for(int i = length - 1; i >= 0; i--)
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

    void add(BigInt &a, BigInt &b)
    {
        this -> length = (a.length > b.length)?a.length:b.length;
        BigInt *t = NULL;
        ++length;
        long carry = 0;
        val = new long[length];
        int l = (a.length < b.length)?a.length:b.length, i;
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
};
//int max(long a, long b){return (a > b)?a:b;}


int main()
{
    BigInt a("99999");
    //a.display();

    BigInt b("999999");
    long int n = 999;
    BigInt c;
    c.add(a,b);
    c.display();


    return 0;
}
