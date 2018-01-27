#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>
#include "any_iterator.h"

#include <gtest/gtest.h>

struct throwing_wrapper_base
{};

std::set<throwing_wrapper_base*> throwing_wrapper_instances;
size_t number_of_copies = 0;
size_t number_of_moves = 0;

template <typename InnerIterator>
struct throwing_wrapper : throwing_wrapper_base
{
    using value_type = typename std::iterator_traits<InnerIterator>::value_type;
    using iterator_category = typename std::iterator_traits<InnerIterator>::iterator_category;
    using pointer = typename std::iterator_traits<InnerIterator>::pointer;
    using reference = typename std::iterator_traits<InnerIterator>::reference;
    using difference_type = typename std::iterator_traits<InnerIterator>::difference_type;

    throwing_wrapper()
    {
        bool inserted = throwing_wrapper_instances.insert(this).second;
        assert(inserted);
    }

    throwing_wrapper(InnerIterator inner)
        : inner(std::move(inner))
    {
        bool inserted = throwing_wrapper_instances.insert(this).second;
        assert(inserted);
    }

    throwing_wrapper(throwing_wrapper const& other)
        : inner(other.inner)
    {
        ++number_of_copies;
        bool inserted = throwing_wrapper_instances.insert(this).second;
        assert(inserted);
    }

    throwing_wrapper(throwing_wrapper&& other)
        : inner(std::move(other.inner))
    {
        ++number_of_moves;
        bool inserted = throwing_wrapper_instances.insert(this).second;
        assert(inserted);
    }

    ~throwing_wrapper()
    {
        bool erased = throwing_wrapper_instances.erase(this);
        assert(erased);
    }

    value_type& operator*() const
    {
        return *inner;
    }

    value_type* operator->() const
    {
        return inner.operator->();
    }

    friend throwing_wrapper& operator++(throwing_wrapper& arg)
    {
        ++arg.inner;
        return arg;
    }

    friend throwing_wrapper operator++(throwing_wrapper& arg, int)
    {
        throwing_wrapper copy = arg;
        ++arg;
        return copy;
    }

    friend throwing_wrapper& operator--(throwing_wrapper& arg)
    {
        --arg.inner;
        return arg;
    }

    friend throwing_wrapper operator--(throwing_wrapper& arg, int)
    {
        throwing_wrapper copy = arg;
        --arg;
        return copy;
    }

    friend bool operator==(throwing_wrapper const& lhs, throwing_wrapper const& rhs)
    {
        return lhs.inner == rhs.inner;
    }

    friend bool operator!=(throwing_wrapper const& lhs, throwing_wrapper const& rhs)
    {
        return lhs.inner == rhs.inner;
    }

    friend throwing_wrapper& operator+=(throwing_wrapper& lhs, std::ptrdiff_t n)
    {
        lhs.inner += n;
        return lhs;
    }

    friend throwing_wrapper& operator-=(throwing_wrapper& lhs, std::ptrdiff_t n)
    {
        lhs.inner -= n;
        return lhs;
    }

    friend throwing_wrapper operator+(throwing_wrapper const& lhs, std::ptrdiff_t n)
    {
        return throwing_wrapper(lhs.inner + n);
    }

    friend throwing_wrapper operator+(std::ptrdiff_t n, throwing_wrapper const& lhs)
    {
        return throwing_wrapper(lhs.inner + n);
    }

    friend throwing_wrapper operator-(throwing_wrapper const& lhs, std::ptrdiff_t n)
    {
        return throwing_wrapper(lhs.inner + n);
    }

    friend std::ptrdiff_t operator-(throwing_wrapper const& lhs, throwing_wrapper const& rhs)
    {
        return lhs.inner - rhs.inner;
    }

    friend bool operator<(throwing_wrapper const& lhs, throwing_wrapper const& rhs)
    {
        return lhs.inner < rhs.inner;
    }

    friend bool operator<=(throwing_wrapper const& lhs, throwing_wrapper const& rhs)
    {
        return lhs.inner <= rhs.inner;
    }

    friend bool operator>=(throwing_wrapper const& lhs, throwing_wrapper const& rhs)
    {
        return lhs.inner >= rhs.inner;
    }

    friend bool operator>(throwing_wrapper const& lhs, throwing_wrapper const& rhs)
    {
        return lhs.inner > rhs.inner;
    }

    value_type& operator[](std::ptrdiff_t n) const
    {
        return inner[n];
    }

private:
    InnerIterator inner;
};



template <typename InnerIterator>
throwing_wrapper<InnerIterator> make_throwing_wrapper(InnerIterator inner)
{
    return throwing_wrapper<InnerIterator>(inner);
}

TEST(correctness, empty)
{
    any_forward_iterator<int> a;
    any_bidirectional_iterator<int> b;
    any_random_access_iterator<int> c;
}

TEST(correctness, empty_copy)
{
    any_forward_iterator<int> a;
    (any_forward_iterator<int>(a));
    any_bidirectional_iterator<int> b;
    (any_bidirectional_iterator<int>(b));
    any_random_access_iterator<int> c;
    (any_random_access_iterator<int>(c));
}

TEST(correctness, empty_move)
{
    any_forward_iterator<int> a;
    (any_forward_iterator<int>(std::move(a)));
    any_bidirectional_iterator<int> b;
    (any_bidirectional_iterator<int>(std::move(b)));
    any_random_access_iterator<int> c;
    (any_random_access_iterator<int>(std::move(c)));
}

TEST(correctness, assign)
{
    any_forward_iterator<int> a, b;
    a = b;
}

TEST(correctness, self_assignment)
{
    std::list<int> x = {1, 2, 3};
    any_forward_iterator<int> a = make_throwing_wrapper(x.begin());
    a = a;
    a = std::move(a);
    EXPECT_EQ(1, *a);
}

TEST(correctness, ctor_big)
{
    int a[5] = {1, 2, 3, 4, 5};
    any_forward_iterator<int> i = make_throwing_wrapper(a + 0);
    //any_forward_iterator<int> end = make_throwing_wrapper(a + 5);
}

TEST(correctness, ctor_move)
{
    std::vector<int> a = {1, 2, 3, 4, 5};

    any_forward_iterator<int> i = a.begin() + 3;
    any_forward_iterator<int> j = std::move(i);
    EXPECT_EQ(4, *j);
}

TEST(correctness, relative_ops)
{
    std::vector<int> a = {1, 2, 3, 4, 5};

    any_random_access_iterator<int> const i = a.begin() + 1;
    any_random_access_iterator<int> const j = a.begin() + 3;

    EXPECT_LT(i, j);
    EXPECT_LE(i, i);
    EXPECT_GT(j, i);
    EXPECT_GE(j, j);

    EXPECT_EQ(i, i);
    EXPECT_NE(i, j);
}

TEST(correctness, difference)
{
    std::vector<int> a = {1, 2, 3, 4, 5};

    any_random_access_iterator<int> i = a.begin();
    any_random_access_iterator<int> j = a.end();
    EXPECT_EQ(5, j - i);
    ++i;
    --j;
    EXPECT_EQ(3, j - i);
}

TEST(correctness, ctor_lvalue)
{
    std::vector<int> a = {1, 2, 3, 4, 5};
    auto x = make_throwing_wrapper(a.begin());

    size_t old_noc = number_of_copies;
    std::cout << number_of_moves << " " << number_of_copies << std::endl;
    any_random_access_iterator<int> y(x);
//   std::cout << *x << std::endl;
    std::cout << number_of_moves << " " << number_of_copies << std::endl;
    EXPECT_EQ(old_noc + 1, number_of_copies);
}

TEST(correctness, ctor_rvalue)
{
    std::vector<int> a;
    auto x = make_throwing_wrapper(a.begin());
    size_t old_nom = number_of_moves;
    any_random_access_iterator<int> y(std::move(x));
    EXPECT_EQ(old_nom + 1, number_of_moves);
}

TEST(correctness, conversions)
{
    std::vector<int> a = {1, 2, 3};
    any_random_access_iterator<int> i = a.begin();
    any_bidirectional_iterator<int> j = i;
    any_forward_iterator<int> k = j;
    EXPECT_EQ(1, *k);
    ++k;
    EXPECT_EQ(2, *k);
    ++k;
    EXPECT_EQ(3, *k);
    static_assert(!std::is_convertible<any_forward_iterator<int>, any_bidirectional_iterator<int>>::value);
    static_assert(!std::is_convertible<any_bidirectional_iterator<int>, any_random_access_iterator<int>>::value);
}

TEST(correctness, incdec_big)
{
    std::vector<int> a = {1, 2, 3, 4, 5};
    any_bidirectional_iterator<int> i = make_throwing_wrapper(a.begin());
    EXPECT_EQ(1, *i);
    EXPECT_EQ(2, *++i);
    EXPECT_EQ(2, *i);
    EXPECT_EQ(2, *i++);
    EXPECT_EQ(3, *i);
    EXPECT_EQ(2, *--i);
    EXPECT_EQ(2, *i);
    EXPECT_EQ(2, *i--);
    EXPECT_EQ(1, *i);
}

TEST(correctness, postinc_1)
{
    std::list<int> a = {1, 2, 3};

    any_bidirectional_iterator<int> i = a.begin();
    EXPECT_TRUE(*i++ == 1);
    EXPECT_TRUE(*i++ == 2);
    EXPECT_TRUE(*i++ == 3);
    EXPECT_TRUE(i == any_bidirectional_iterator<int>(a.end()));
}

TEST(correctness, predec_1)
{
    std::vector<int> a = {1, 2, 3};
    any_random_access_iterator<int> i = a.end();
    --i;
    EXPECT_EQ(3, *i);
    --i;
    EXPECT_EQ(2, *i);
    --i;
    EXPECT_EQ(1, *i);
}

TEST(correctness, assignment)
{
    std::forward_list<int> a = {1, 2, 3, 4, 5};

    any_forward_iterator<int> i = make_throwing_wrapper(a.begin());
    ++i;
    any_forward_iterator<int> old = i;
    ++i;
    ++i;
    i = old;
    EXPECT_EQ(*i, 2);
}

TEST(correctness, subscript)
{
    std::vector<int> a = {1, 2, 3, 4, 5};

    any_random_access_iterator<int> i = make_throwing_wrapper(a.begin() + 2);
    EXPECT_EQ(1, i[-2]);
    EXPECT_EQ(2, i[-1]);
    EXPECT_EQ(3, i[0]);
    EXPECT_EQ(4, i[1]);
    EXPECT_EQ(5, i[2]);

    any_random_access_iterator<int> const j = i;
    EXPECT_EQ(1, j[-2]);
    EXPECT_EQ(2, j[-1]);
    EXPECT_EQ(3, j[0]);
    EXPECT_EQ(4, j[1]);
    EXPECT_EQ(5, j[2]);
}

TEST(correctness, add_sub)
{
    std::vector<int> a = {1, 2, 3, 4, 5};

    any_random_access_iterator<int> i = make_throwing_wrapper(a.begin());

    EXPECT_EQ(&i, &((i += 2) += 2));
    EXPECT_EQ(5, *i);

    EXPECT_EQ(&i, &((i -= 2) -= 2));
    EXPECT_EQ(1, *i);
}

TEST(correctness, list_1)
{
    std::forward_list<int> a = {1, 2, 3, 4, 5};

    std::forward_list<int> b;
    std::copy(any_forward_iterator<int>(a.begin()),
              any_forward_iterator<int>(a.end()),
              std::front_inserter(b));

    std::forward_list<int> c = {5, 4, 3, 2, 1};
    EXPECT_TRUE(b == c);
}

TEST(correctness, list_2)
{
    std::list<int> a = {1, 2, 3, 4, 5};

    std::list<int> b;
    std::copy(any_forward_iterator<int>(a.begin()),
              any_forward_iterator<int>(a.end()),
              std::back_inserter(b));

    EXPECT_TRUE(a == b);
}

TEST(correctness, list_3)
{
    std::list<int> a = {1, 2, 3, 4, 5};

    std::list<int> b;
    using reverse_iterator = std::reverse_iterator<any_bidirectional_iterator<int> >;
    std::copy(reverse_iterator(any_bidirectional_iterator<int>(a.end())),
              reverse_iterator(any_bidirectional_iterator<int>(a.begin())),
              std::front_inserter(b));

    EXPECT_TRUE(a == b);
}

TEST(correctness, list_4)
{
    std::list<int> a = {1, 2, 3, 4, 5};

    std::list<int> b;
    using reverse_iterator = std::reverse_iterator<any_bidirectional_iterator<int> >;
    {
        std::copy(reverse_iterator(any_bidirectional_iterator<int>(make_throwing_wrapper(a.end()))),
                  reverse_iterator(any_bidirectional_iterator<int>(make_throwing_wrapper(a.begin()))),
                  std::front_inserter(b));
    }

    EXPECT_TRUE(a == b);
    EXPECT_TRUE(throwing_wrapper_instances.empty());
}

TEST(correctness, vector_1)
{
    std::vector<int> a = {5, 3, 2, 4, 1};
    std::sort(any_random_access_iterator<int>(a.begin()),
              any_random_access_iterator<int>(a.end()));

    std::vector<int> b = {1, 2, 3, 4, 5};
    EXPECT_TRUE(a == b);
}

TEST(correctness, vector_2)
{
    std::vector<int> a = {1, 2, 3, 4, 5};
    any_random_access_iterator<int> i = a.begin();
    EXPECT_TRUE(i[0] == 1);
    EXPECT_TRUE(i[1] == 2);
    EXPECT_TRUE(i[2] == 3);
    EXPECT_TRUE(i[3] == 4);
    EXPECT_TRUE(i[4] == 5);
}

TEST(correctness, const_to_nonconst){
    std::vector<int> a;
    any_random_access_iterator<int> it = a.begin();
    any_random_access_iterator<const int> cit = it;
    cit = it;
}

TEST(correctness, bool_conversion){
    std::vector<int> a;
    any_random_access_iterator<int> it = a.begin();
    EXPECT_TRUE(bool(it));
}

TEST(correctness, get_pointer){
    std::vector<int> a = {1, 2, 3};
    any_random_access_iterator<int> it = a.begin();
    int* p = it.operator ->();
    EXPECT_TRUE(*p == 1);
}

TEST(correctness, same_types){
    std::vector<int> a = {1, 2, 3};
    std::vector<int> b = {1, 2, 3};
    any_random_access_iterator<int> it = a.begin();
    any_random_access_iterator<int> it2 = b.end();
    EXPECT_TRUE(it.is_same_type(it2));
}

TEST(correctness, minus_n){
    std::vector<int> a = {1, 2, 3};
    any_random_access_iterator<int> it = a.end();
    EXPECT_EQ(3, *(it-1));
}

TEST(correctness, move_other_category){
    std::vector<int> a = {1, 2, 3};
    any_random_access_iterator<int> it = a.begin();
    any_bidirectional_iterator<int> it2 = std::move(it);
}

TEST(correctness, const_any_iterator){
    std::vector<int> a = {1, 2, 3};
    any_forward_iterator<int> it2 = a.begin();
    any_forward_iterator<const int> it = a.cbegin();

//    any_forward_iterator<int> it3 = a.cbegin();
//    it3 = it;

//    any_forward_iterator<const int> it3 = std::move(it);
//    EXPECT_EQ(1, it.is_same_type(it2));
}


template struct any_iterator<int, std::forward_iterator_tag>;
template struct any_iterator<int, std::bidirectional_iterator_tag>;
template struct any_iterator<int, std::random_access_iterator_tag>;

int main(int argc, char *argv[])
{
//    std::vector<int> a = {1, 2, 3};
//    any_forward_iterator<int> it3 = a.cbegin();

//    correctness_const_any_iterator_Test();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
