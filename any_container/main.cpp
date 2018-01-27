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
    my_vector<int> a;
}

TEST(correctness, empty_copy)
{
    my_vector<int> a;
    (my_vector<int>(a));
    my_list<int> b;
    (my_list<int>(b));
    my_deque<int> c;
    (my_deque<int>(c));
}

TEST(correctness, empty_move)
{
    my_vector<int> a;
    (my_vector<int>(std::move(a)));
    my_list<int> b;
    (my_list<int>(std::move(b)));
    my_deque<int> c;
    (my_deque<int>(std::move(c)));
}

TEST(correctness, empty_assign)
{
    my_vector<int> a, b;
    a = b;
}

TEST(correctness, self_assignment)
{
    std::vector<int> x = {1};
    std::deque<size_t> y = {1, 2, 3, 4};
    my_vector<int> a = x;
    my_deque<size_t> b = y;
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

    my_vector <int> v = a;
    my_deque <int> l = b;

    my_vector <int> vv = std::move(v);
    my_deque <int> ll = std::move(l);
    EXPECT_EQ(4, *(vv.begin()+3));
    EXPECT_EQ(2, *(++ll.begin()));
}

TEST(correctness, insertion){
    std::vector<int> a;
    my_vector<int> v = a;

    std::deque <int> b;
    my_deque<int> l = b;

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
    my_vector<int> v = a;

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
    my_vector<int> v;
    v = a;
}

TEST(correctness, iterators_comprasion){
    vector <int> a = {1, 2, 3};
    my_vector <int> v(a);
    EXPECT_TRUE(v.begin() != v.end());
    EXPECT_TRUE(v.begin() < v.end());
    EXPECT_TRUE(v.begin() <= v.end());
    EXPECT_FALSE(v.begin() >= v.end());
    EXPECT_FALSE(v.begin() > v.end());
    EXPECT_FALSE(v.begin() == v.end());
}

TEST(correctness, iterators_operations){
    vector <int> a = {1, 2, 3};
    my_vector <int> v(a);
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
    static_assert(!std::is_convertible_v<vector<int>, my_list<int>>);
    static_assert(!std::is_convertible_v<vector<int>, my_vector<char>>);
    vector <int> a = {1, 2, 3, 4, 5};
    list <int> b = {1, 2, 3};
    my_list<int> l(b);
    any_iterator<int, std::bidirectional_iterator_tag> it = a.begin();
//    iter cit = a.cbegin();  //shouldn't work! (no conversion from const to non-const)
//    l.insert(it, 5);        //shouldn't work! (assert)
//    l.erase(it);            //shouldn't work! (assert)
//    any_random_access_iterator<int> it2 = a.cbegin();
//    any_random_access_iterator<const int> cit2 = a.cbegin();
//    cout << (it2 == cit2) << endl;
//    EXPECT_EQ(it2, cit2);
//    EXPECT_EQ(cit2, it2);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
