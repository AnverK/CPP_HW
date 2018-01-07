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

    my_struct(const my_struct &) = delete;
    my_struct(my_struct&&) = delete;
};

int main() {

//    std::vector<int> a(10000, 0);
//    std::vector<int> b(20000, 4);
//    std::string s("abacabaafdskfsdajfksdajfkasjfksdajfkasjrqwehrowjklsdajf");
//    using var_t = variant< std::vector<int>, std::string>;
//    using var_t = variant<int, double, float>;

//    using var2_t = std::variant<int, double, my_struct>;
//    using var_t = variant<std::vector<int>>;
//    var_t v(s);

//    var_t v2(b);
//    v2.swap(v);
//    get<0>(v2);
//    cout << get<0>(var_t(a)).size() << endl;
//    cout << get<0>(var)).size() << endl;
//cout << v.index() << endl;
//    cout << v2.get<2>().size() << endl;
//    cout << std::is_constructible_v<var_t> << endl;
//    var_t v2(b);

//    get<0>(v);
//    v.swap(v2);

//    v.emplace<int>(0);
//    v.emplace<std::vector<int>>(a);
//    v.emplace(std::vector<int>(10000, 0));
    return 0;
}
