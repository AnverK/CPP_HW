#include "big_integer.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
using std::cout;
using std::endl;

const static uint64_t MAX_uint64 = 18446744073709551615u;
const static uint32_t MAX_uint32= 4294967295u;
const static int MAX_INT32 = 2147483647;
const static uint64_t BASE = 4294967296u;

big_integer::big_integer()
{
    number.push_back(0);
    sign = 0;
}

big_integer::big_integer(int n)
{
    if(n >= 0)
    {
        number.push_back((uint32_t) n);
        sign = 0;
    }
    else
    {
        number.push_back(-(uint32_t)n);
        sign = 1;
    }
}

big_integer::big_integer(std::string const& s)
{
    if(s.size() == 0)
    {
        big_integer();
        return;
    }
    size_t pos = 0;
    if(s[0] == '-')
    {
        sign = 1;
        pos++;
    }
    else
    {
        sign = 0;
    }
    vector <char> tmp;

    for(size_t i = pos; i < s.size(); i++)
    {
        tmp.push_back((char)s[i]-'0');
    }
    std::reverse(tmp.begin(), tmp.end());
    while((tmp.size() != 1 || tmp[0] != 0) && tmp.size() != 0)
    {
        uint32_t mod = (uint32_t)long_mod_short(tmp, BASE, 10);
        number.push_back(mod);
    }
    clear_zero(number);
}

big_integer::big_integer(big_integer const& obj)
{
    number = obj.number;
    sign = obj.sign;
}

big_integer::big_integer(vector_uint32 const& obj, bool sign)
{
    number = obj;
    this->sign = sign;
    check_sign();
}

big_integer::~big_integer()
{

}

template<typename T>
uint64_t long_mod_short(vector <T> &a, uint64_t module_base, uint64_t input_base)
{
    uint64_t cur = 0;
    for (size_t i = a.size() - 1; i < MAX_uint64; i--)
    {
        cur = cur * input_base + a[i];
        a[i] = cur / module_base;
        cur %= module_base;
    }
    clear_zero(a);
    return cur;
}

uint64_t long_mod_short(vector_uint32 &a, uint64_t module_base, uint64_t input_base)
{
    uint64_t cur = 0;
    for (size_t i = a.size() - 1; i < MAX_uint64; i--)
    {
        cur = cur * input_base + a[i];
        a[i] = cur / module_base;
        cur %= module_base;
    }
    clear_zero(a);
    return cur;
}

template <typename T>
void clear_zero(vector <T> &a)
{
    if(a.size() == 0)
    {
        a.push_back(0);
        return;
    }
    size_t pos = a.size() - 1;
    while(pos < MAX_uint64 && a[pos] == 0)
    {
        pos--;
    }
    a.resize(pos + 1);
    if(a.size() == 0)
    {
        a.push_back(0);
    }
}

void clear_zero(vector_uint32 &a)
{
    if(a.size() == 0)
    {
        a.push_back(0);
        return;
    }
    size_t pos = a.size() - 1;
    while(pos < MAX_uint64 && a[pos] == 0)
    {
        pos--;
    }
    a.resize(pos + 1);
    if(a.size() == 0)
    {
        a.push_back(0);
    }
}


void big_integer::print_number(const vector_uint32 &a)
{
    for(size_t i = 0; i < a.size(); i++)
    {
        cout << a[i] << " ";
    }
    cout << endl;
}

std::string to_string(big_integer const &a)
{
    if(a.number.size() == 0)
    {
        return "0";
    }
    vector_uint32 tmp(a.number);

    std::string str;

    while(tmp.size() != 1 || tmp[0] != 0)
    {
        uint32_t mod = (char)long_mod_short(tmp, 10, BASE);
        str.push_back((char)(mod+'0'));
    }

    while(!str.empty() && str[str.size()-1] == '0')
    {
        str.pop_back();
    }
    if(str.size() == 0)
    {
        return "0";
    }
    if(a.sign == 1)
    {
        str.push_back('-');
    }
    std::reverse(str.begin(), str.end());
    return str;
}

void big_integer::swap(big_integer &a)
{
    std::swap(this->number, a.number);
    std::swap(this->sign, a.sign);
}

big_integer& big_integer::operator=(big_integer const& a)
{
    big_integer cur(a);
    swap(cur);
    return *this;
}

int compare(const vector_uint32 &a, const vector_uint32 &b)
{
    if(a.size() != b.size())
    {
        return (int)a.size() - b.size();
    }
    else
    {
        for(size_t i = a.size() - 1; i < MAX_uint64; i--)
        {
            if(a[i] != b[i])
            {
                return a[i] < b[i] ? -1 : 1;
            }
        }
    }
    return 0;
}

int compare(big_integer const& a, big_integer const& b, bool abs = false)
{
    if(abs == 0 && a.sign != b.sign)
    {
        return a.sign - b.sign;
    }
    return compare(a.number, b.number);
}

bool operator<(big_integer const& a, big_integer const& b)
{
    return compare(a, b) < 0;
}
bool operator<=(big_integer const& a, big_integer const& b)
{
    return compare(a, b) <= 0;
}
bool operator>(big_integer const& a, big_integer const& b)
{
    return compare(a, b) > 0;
}
bool operator>=(big_integer const& a, big_integer const& b)
{
    return compare(a, b) >= 0;
}
bool operator==(big_integer const& a, big_integer const& b)
{
    return compare(a, b) == 0;
}
bool operator!=(big_integer const& a, big_integer const& b)
{
    return compare(a, b) != 0;
}

big_integer& big_integer::operator++()
{
    vector_uint32 tmp;
    tmp.push_back(1);
    const big_integer tmp_big(tmp, 0);
    return *this+=tmp_big;
}

big_integer& big_integer::operator--()
{
    const big_integer tmp_big(1);
    return *this-=tmp_big;
}

big_integer big_integer::operator+() const
{
    return *this;
}

big_integer big_integer::operator-() const
{
    return big_integer(number, !sign);
}

big_integer big_integer::operator~() const
{
    const big_integer tmp_big(1);
    return -*this - tmp_big;
}

big_integer& big_integer::operator+=(big_integer const& rhs)
{
    if(sign == rhs.sign)
    {
        add(number, number, rhs.number);
        return *this;
    }
    number.change_ptr();
    int comp = compare(*this, rhs, true);
    if(comp == 0)
    {
        number.resize(1);
        number[0] = 0;
        sign = 0;
        return *this;
    }
    if (comp > 0)
    {
        subtract(number, number, rhs.number);
        return *this;
    }
    subtract(number, rhs.number, number);
    sign = rhs.sign;
    check_sign();
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs)
{
    return *this += -rhs;
}

big_integer& big_integer::operator*=(big_integer const& rhs)
{
    sign = sign ^ rhs.sign;
    big_integer copy = *this;
    multiply(number, copy.number, rhs.number);
    check_sign();
    return *this;
}

big_integer& big_integer::operator/=(big_integer const& rhs)
{
    sign = sign ^ rhs.sign;
    big_integer copy = *this;
    divide(number, copy.number, rhs.number);
    check_sign();
    return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs)
{
    vector_uint32 quotient;
    divide(quotient, number, rhs.number);
    vector_uint32 mul;
    multiply(mul, quotient, rhs.number);
    subtract(number, number, mul);
    check_sign();
    return *this;
}

big_integer& big_integer::operator &=(big_integer const& rhs)
{
    long_and(number, big_integer_to_byte_vector(*this), big_integer_to_byte_vector(rhs));
    return *this = byte_vector_to_big_integer(number);
}

big_integer& big_integer::operator |=(big_integer const& rhs)
{
    long_or(number, big_integer_to_byte_vector(*this), big_integer_to_byte_vector(rhs));
    return *this = byte_vector_to_big_integer(number);
}

big_integer& big_integer::operator ^=(big_integer const& rhs)
{
    long_xor(number, big_integer_to_byte_vector(*this), big_integer_to_byte_vector(rhs));
    return *this = byte_vector_to_big_integer(number);
}

big_integer& big_integer::operator <<=(int rhs)
{
    left_shift(number, big_integer_to_byte_vector(*this),  rhs);
    return *this = byte_vector_to_big_integer(number);
}

big_integer& big_integer::operator >>=(int rhs)
{
    right_shift(number, big_integer_to_byte_vector(*this),  rhs);
    return *this = byte_vector_to_big_integer(number);
}

big_integer operator+(big_integer a, big_integer const& b)
{
    return a += b;
}

big_integer operator-(big_integer a, big_integer const& b)
{
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b)
{
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b)
{
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b)
{
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b)
{
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b)
{
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b)
{
    return a ^= b;
}

big_integer operator<<(big_integer a, const int b)
{
    return a <<= b;
}

big_integer operator>>(big_integer a, const int b)
{
    return a >>= b;
}

void big_integer::add(vector_uint32 &res, const vector_uint32 &a, const vector_uint32 &b)
{
    size_t length = std::max(a.size(), b.size()) + 1;
    size_t a_size = a.size(), b_size = b.size();
    res.resize(length);
    res.change_ptr();
    uint64_t carry = 0;

    for (size_t i = 0; i < length; i++)
    {
        uint64_t tmp = (uint64_t)(i < a_size ? a[i] : 0) + (i < b_size ? b[i] : 0) + carry;
        carry = tmp/BASE;
        res[i] = tmp % BASE;
    }
    clear_zero(res);
}

void big_integer::subtract(vector_uint32 &res, const vector_uint32 &a, const vector_uint32 &b)
{
    size_t length = std::max(a.size(), b.size());
    size_t b_size = b.size();
    res.resize(length);
    uint64_t carry = 0;
    for (size_t i=0; i<b_size || carry; ++i) {
        uint64_t tmp = (uint64_t) carry + (i < b_size ? b[i] : 0);
        if(a[i] < tmp)
        {
            res[i] = (uint64_t)BASE - tmp + a[i];
            carry = 1;
        }
        else
        {
            carry = 0;
            res[i] = a[i] - tmp;
        }
    }
    clear_zero(res);
}

void big_integer::multiply(vector_uint32 &res, const vector_uint32 &a, const vector_uint32 &b)
{
    size_t length = a.size() + b.size();
    res.clear();
    res.resize(length);    
    for (size_t i = 0; i < a.size(); i++)
    {
        uint64_t carry = 0;
        for (size_t j = 0; j < b.size() || carry; j++)
        {
            uint64_t tmp = (uint64_t) res[i+j] + (uint64_t) a[i] * (j < b.size()? b[j] : 0) + carry;
            res[i+j] = tmp % BASE;
            carry = tmp / BASE;
        }
    }
    clear_zero(res);
}

void big_integer::long_mul_short(vector_uint32 &res, const vector_uint32 &a, const unsigned int b)
{
    size_t a_size = a.size();
    res.resize(a.size() + 1);
    uint32_t carry = 0;
    for(size_t i = 0; i < a_size; i++)
    {
        uint64_t tmp = (uint64_t) a[i] * b + carry;
        res[i] = tmp % BASE;
        carry = tmp / BASE;
    }
    res[a_size] = carry;
}

void big_integer::long_div_short(vector_uint32 &res, const vector_uint32 &a, const unsigned int b)
{
    size_t a_size = a.size();
    res.resize(a.size());
    uint32_t carry = 0;
    for(size_t i = a_size - 1; i < MAX_uint64; i--)
    {
        uint64_t tmp = (uint64_t) carry*BASE + a[i];
        res[i] = tmp / b;
        carry = tmp % b;
    }
    clear_zero(res);
}

void big_integer::long_mod_short2(vector_uint32 &res, const vector_uint32 &a, const unsigned int b)
{
    uint32_t carry = 0;
    for(size_t i = a.size() - 1; i < MAX_uint64; i--)
    {
        carry = ((uint64_t)carry * BASE + a[i]) % b;
    }
    res.clear();
    res.push_back(carry);
}

bool compare_equal_vector(const vector_uint32 &a, const vector_uint32 &b)
{
    for (size_t i = a.size(); i > 0; i--)
    {
        if (a[i - 1] != b[i - 1])
        {
            return a[i - 1] < b[i - 1];
        }
    }
    return 0;
}

void big_integer::subtract_equal_vector(vector_uint32 &a, const vector_uint32 &b)
{
    uint64_t carry = 0;
    for (size_t i=0; i < b.size() || carry; ++i)
    {
        uint64_t tmp = (uint64_t) carry + (i < b.size() ? b[i] : 0);
        if(a[i] < tmp)
        {
            a[i] = (uint64_t)BASE - tmp + a[i];
            carry = 1;
        }
        else
        {
            carry = 0;
            a[i] -= tmp;
        }
    }
}

void big_integer::divide(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b)
{
    int sign = compare(a, b);
    res.clear();

    if(sign < 0)
    {
        res.push_back(0);        
        return;
    }
    if(sign == 0)
    {
        res.push_back(1);
        return;
    }
    if(b.size() == 1)
    {
        long_div_short(res, a, b[0]);
        return;
    }


    uint32_t d = (uint32_t)(BASE / (b[b.size() - 1] + 1));


//    vector_uint32 u(a), v(b);
    vector_uint32 u(a.size()), v(b.size());
    for(size_t i = 0; i < a.size(); i++)
    {
        u[i] = a[i];
    }
    for(size_t i = 0; i < b.size(); i++)
    {
        v[i] = b[i];
    }

    long_mul_short(u, a, d);
    long_mul_short(v, b, d);
    clear_zero(u);
    clear_zero(v);
    size_t n = u.size(), m = v.size(), len = n - m + 1;
    res.resize(len);

    vector_uint32 dividend(m+1), divider(m+1);
    for(size_t i = 0; i < m; i++)
    {
        dividend[i] = u[n + i - m];
    }

    dividend[m] = n < u.size() ? u[n] : 0;
    for(size_t i = 0; i < len; i++)
    {
        dividend[0] = u[n - m - i];
        size_t cur_pos = len - 1 - i;

        uint32_t tmp = std::min( (uint64_t) ((uint64_t)(m < dividend.size() ? dividend[m] : 0)* BASE + (m - 1 < dividend.size() ? dividend[m - 1] : 0)) / v.back(), BASE - 1);

        long_mul_short(divider, v, tmp);

        while(compare_equal_vector(dividend, divider))
        {
            long_mul_short(divider, v, --tmp);
        }
        subtract_equal_vector(dividend, divider);
        for(size_t j = m; j > 0; j--)
        {
            dividend[j] = dividend[j - 1];
        }
        res[cur_pos] = tmp;
    }
    clear_zero(res);
}

void big_integer::long_and(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b)
{
    size_t len = std::max(a.size(), b.size());
    size_t a_size = a.size(), b_size = b.size();
    res.resize(len);
    for(size_t i = 0; i < len; i++)
    {
        res[i] = a[std::min(a_size-1, i)] & b[std::min(b_size-1, i)];
    }
}

void big_integer::long_or(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b)
{
    size_t len = std::max(a.size(), b.size());
    size_t a_size = a.size(), b_size = b.size();
    res.resize(len);
    for(size_t i = 0; i < len; i++)
    {
        res[i] = a[std::min(a_size-1, i)] | b[std::min(b_size-1, i)];
    }
}

void big_integer::long_xor(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b)
{
    size_t len = std::max(a.size(), b.size());
    size_t a_size = a.size(), b_size = b.size();
    res.resize(len);
    for(size_t i = 0; i < len; i++)
    {
        res[i] = a[std::min(a_size-1, i)] ^ b[std::min(b_size-1, i)];
    }
}

void big_integer::long_not(vector_uint32 &res, vector_uint32 const &a)
{
    size_t len = a.size();
    res.resize(len);
    for(size_t i = 0; i < len; i++)
    {
        res[i] = ~res[i];
    }
}

void big_integer::right_shift(vector_uint32 &res, vector_uint32 const &a, const int b)
{
    uint32_t shift_in = b % 32;
    uint32_t shift_out = b / 32;
    vector_uint32 tmp;
    uint64_t cur = 0;
    uint32_t sign = a[a.size()-1];
    cur = a[shift_out] >> shift_in;
    for(size_t i = shift_out; i < a.size() - 1; i++)
    {
        uint64_t buf = (uint64_t) a[i+1] << (32 - shift_in);
        cur |= buf;
        tmp.push_back(cur & 0xffffffff);
        cur >>= 32;
    }
    tmp.push_back(sign);
    res = tmp;
}

void big_integer::left_shift(vector_uint32 &res, vector_uint32 const &a, const int b)
{
    uint32_t shift_in = b % 32;
    uint32_t shift_out = b / 32;
    vector_uint32 tmp(shift_out);
    uint64_t cur = 0;
    uint32_t sign = a[a.size()-1];
    for(size_t i = 0; i < a.size(); i++)
    {
        uint64_t buf = a[i];
        cur |= (buf << shift_in);
        tmp.push_back(cur & 0xffffffff);
        cur >>= 32;
    }
    tmp.push_back(sign);
    res = tmp;
}

vector_uint32 big_integer::big_integer_to_byte_vector(const big_integer &a)
{
    vector_uint32 res(a.number);
    res.push_back(0);
    if(a.sign == 0)
    {
        return res;
    }
    for(size_t i = 0; i < res.size(); i++)
    {
        res[i] = ~res[i];
    }

    vector_uint32 buf;
    buf.push_back(1);
    add(res, res, buf);
    return res;
}

big_integer big_integer::byte_vector_to_big_integer(const vector_uint32 &a)
{
    vector_uint32 res(a);
    int sign = 0;
    if(a[a.size()-1] == MAX_uint32)
    {
        res.pop_back();
        sign = 1;
        long_not(res, res);
        vector_uint32 one;
        one.push_back(1);
        add(res, res, one);
    }
    clear_zero(res);
    return big_integer(res, sign);
}

void big_integer::check_sign()
{
    if(number.size() == 1 && number[0] == 0)
    {
        sign = 0;
    }
}

vector_uint32 big_integer::pow_base(uint32_t b)
{
    vector_uint32 res(b);
    res.push_back(1);
    return res;
}
