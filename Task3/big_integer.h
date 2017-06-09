#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <vector>
#include "vector/vector_uint32.h"

class big_integer
{
public:
    big_integer();
    big_integer(big_integer const& obj);
    big_integer(int n);
    explicit big_integer(std::string const& s);
    ~big_integer();

    big_integer& operator=(big_integer const& obj);

    big_integer& operator++();
    big_integer& operator--();

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    friend std::string to_string(big_integer const& a);

    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);
    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);

    bool get_sign() const;
    vector_uint32 get_number();

private:
    vector_uint32 number;
    bool sign;

    const static uint64_t MAX_uint64 = 18446744073709551615u;
    const static uint32_t MAX_uint32 = 4294967295u;
    const static uint64_t BASE = 4294967296u;

    void swap(big_integer &a);
    friend int compare(const vector_uint32 &a, const vector_uint32 &b);
    friend int compare(big_integer const& a, big_integer const& b, bool abs);
    friend bool compare_equal_vector(const vector_uint32 &a, const vector_uint32 &b);
    void subtract_equal_vector(vector_uint32 &a, const vector_uint32 &b);

    big_integer(vector_uint32 const& obj, bool sign);

    void add(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b);
    void subtract(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b);
    void multiply(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b);
    void divide(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b);

    void long_and(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b);
    void long_or(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b);
    void long_xor(vector_uint32 &res, vector_uint32 const &a, vector_uint32 const &b);
    void long_not(vector_uint32 &res, vector_uint32 const &a);

    vector_uint32 big_integer_to_byte_vector(big_integer const &a);
    big_integer byte_vector_to_big_integer(vector_uint32 const &a);

    vector_uint32 pow_base(uint32_t b);

    void long_mul_short(vector_uint32 &res, vector_uint32 const &a, const unsigned int b);
    void long_div_short(vector_uint32 &res, vector_uint32 const &a, const uint32_t b);
    void long_mod_short2(vector_uint32 &res, vector_uint32 const &a, const uint32_t b);

    void right_shift(vector_uint32 &res, vector_uint32 const &a, const int b);
    void left_shift(vector_uint32 &res, vector_uint32 const &a, const int b);

    void print_number(vector_uint32 const&a);
    void check_sign();
};


big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

template <typename T>
void clear_zero(std::vector<T> &a);

void clear_zero(vector_uint32 &a);

template <typename T>
uint64_t long_mod_short(std::vector<T> &a, uint64_t module_base, uint64_t input_base);

uint64_t long_mod_short(vector_uint32 &a, uint64_t module_base, uint64_t input_base);



#endif // BIG_INTEGER_H
