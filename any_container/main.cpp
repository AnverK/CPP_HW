#include <iostream>
#include "any_container.h"
#include <gtest/gtest.h>
#include <vector>
#include <deque>
using namespace std;

typedef any_iterator<int,  std::random_access_iterator_tag> iter;
typedef any_iterator<const int,  std::random_access_iterator_tag> const_iter;

TEST(correctness, empty)
{
    any_container<int> a;
}

TEST(correctness, empty_copy)
{
    any_container<int> a;
    (any_container<int>(a));
    any_container<int> b;
    (any_container<int>(b));
    any_container<int> c;
    (any_container<int>(c));
}

TEST(correctness, empty_move)
{
    any_container<int> a;
    (any_container<int>(std::move(a)));
    any_container<int> b;
    (any_container<int>(std::move(b)));
    any_container<int> c;
    (any_container<int>(std::move(c)));
}

TEST(correctness, empty_assign)
{
    any_container<int> a, b;
    a = b;
}

TEST(correctness, self_assignment)
{
    std::vector<int> x = {1};
    std::deque<size_t> y = {1, 2, 3, 4};
    any_container<int> a = x;
    any_container<size_t> b = y;
    EXPECT_EQ(*a.begin(), 1);
    EXPECT_EQ(*b.begin(), 1);
    a = a;
    b = b;
    EXPECT_EQ(*a.begin(), 1);
    EXPECT_EQ(*b.begin(), 1);
    a = std::move(a);
    b = std::move(b);
    EXPECT_EQ(*a.begin(), 1);
    EXPECT_EQ(*b.begin(), 1);
}

TEST(correctness, ctor_move)
{
    std::vector<int> a = {1, 2, 3, 4, 5};
    std::deque<int> b = {1, 2, 3, 4, 5};

    any_container <int> v = a;
    any_container <int> l = b;

    any_container <int> vv = std::move(v);
    any_container <int> ll = std::move(l);
    EXPECT_EQ(4, *(vv.begin()+3));
    EXPECT_EQ(2, *(++ll.begin()));
}

TEST(correctness, insertion){
    std::vector<int> a;
    any_container<int> v = a;

    std::deque <int> b;
    any_container<int> l = b;

    EXPECT_TRUE(v.cbegin().is_same_type(v.begin()));
    for(int i = 0; i < 10; i++){
        v.insert(v.begin(), i+1);
        l.insert(l.begin(), 10-i);
    }

    EXPECT_EQ(10, v.size());
    EXPECT_EQ(10, l.size());
    v.insert(v.begin()+1, -3);
    l.insert(++l.begin(), -5);
    EXPECT_EQ(-3, *(v.begin()+1));
    EXPECT_EQ(-5, *(++l.begin()));
}

TEST(correctness, erasion){
    std::vector<int> a;
    any_container<int> v = a;

    for(int i = 0; i < 10; i++){
        v.insert(v.begin(), i+1);
    }
    for(int i = 0; i < 9; i++){
        v.erase(v.end()-1);
    }
    EXPECT_EQ(10, *v.begin());
    EXPECT_EQ(1, v.size());
    v.erase(v.begin());
    EXPECT_TRUE(v.is_empty());
}

TEST(correctness, assignment_by_value){
    vector <int> a;
    any_container<int> v;
    v = a;
}

TEST(correctness, iterators_comprasion){
    vector <int> a = {1, 2, 3};
    any_container <int> v(a);
    EXPECT_TRUE(v.begin() != v.end());
    EXPECT_TRUE(v.begin() < v.end());
    EXPECT_TRUE(v.begin() <= v.end());
    EXPECT_FALSE(v.begin() >= v.end());
    EXPECT_FALSE(v.begin() > v.end());
    EXPECT_FALSE(v.begin() == v.end());
}

TEST(correctness, iterators_operations){
    vector <int> a = {1, 2, 3};
    any_container <int> v(a);
    EXPECT_TRUE(static_cast<ptrdiff_t>(v.size()) == v.end()-v.begin());
    EXPECT_TRUE(v.size() == 3);
    iter end = v.end();
    iter beg = v.begin()+1;

    EXPECT_EQ(3, *--end);
    EXPECT_EQ(2, *beg--);
    EXPECT_EQ(1, *beg++);

    swap(beg, end);
    EXPECT_EQ(3, *beg);
    EXPECT_EQ(2, *end);

    beg = end;
    EXPECT_EQ(2, *beg);
    beg = std::move(end);
    EXPECT_EQ(2, *beg);

    const const_iter cit = v.begin();
    EXPECT_EQ(1, *cit);

    beg = a.begin();
    EXPECT_EQ(1, *beg);
}

TEST(correctness, some_asserts)
{
    deque <int> a = {1, 2, 3, 4, 5};
    list <int> b = {6, 7, 8};
    any_container<int, std::forward_iterator_tag> l(b);
    EXPECT_EQ(7, *(++l.begin()));
    l = a;
    EXPECT_EQ(2, *(++l.begin()));
//    iter cit = a.cbegin();  //shouldn't work! (no conversion from const to non-const)
//    l.insert(a.begin(), 5);        //shouldn't work!
//    l.erase(a.begin());            //shouldn't work!
    any_container<const int, std::forward_iterator_tag> cl = l;
    any_container<int, std::forward_iterator_tag> copyl = l;
    copyl.erase(copyl.begin());
    EXPECT_EQ(2, *copyl.begin());
    EXPECT_EQ(1, *l.begin());
}

int main(int argc, char *argv[])
{
//    correctness_empty_Test();
//    correctness_some_asserts_Test();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
