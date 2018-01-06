#include <iostream>
#include "variant.h"
#include <variant>
#include <vector>
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::in_place_index_t;
using std::in_place_type_t;

struct my_struct
{
    int x;
    my_struct() : x(228) {}

    my_struct(my_struct const&) = delete;
};

int main() {
//    int n = 5;
//    std::string(4.3);
//    std::vector <int> v({1, 2, 3});
//    int x = 10;
////    short p;
//    size_t st = 1;
//    variant<int, short, size_t, double, float> t(std::in_place_index_t<0>{}, 1);

//    cout << t.get<int>() << endl;
//    t.set<double>(1.2);
//    t.get<int>();
//    cout << t.get<int>()<< endl;
    std::string s;
    variant<int, std::string> v2;
    v2.set<std::string>(s);
//    cout << v2.index() << endl;
//    cout << t.get<0>().size() << endl;
//    auto tt(t);
//    variant<string, char, int> tt(t);
//    t.set<std::vector<int>>(v);
//    tt.set<1>("abcd");
//    std::string a;
//    a = t.get<1>();
//    cout << tt.get<1>().size() << endl;
    return 0;
}
