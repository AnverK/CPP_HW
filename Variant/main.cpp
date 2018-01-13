#include "variant.h"
#include <vector>
#include <iomanip>

template<class T> struct always_false : std::false_type {};
using var_t = variant<int, long, double, std::string>;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

int main() {
    std::vector<var_t> vec = {10,  1.5, "hello"};
    for(auto& v: vec) {
        // void visitor, only called for side-effects
        visit([](auto&& arg){std::cout << arg;}, v);

        // value-returning visitor. A common idiom is to return another variant
        var_t w = visit([](auto&& arg) -> var_t {return arg + arg;}, v);

        std::cout << ". After doubling, variant holds ";
        // type-matching visitor: can also be a class with 4 overloaded operator()'s
        visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>)
                std::cout << "int with value " << arg << '\n';
            else if constexpr (std::is_same_v<T, long>)
                std::cout << "long with value " << arg << '\n';
            else if constexpr (std::is_same_v<T, double>)
                std::cout << "double with value " << arg << '\n';
            else if constexpr (std::is_same_v<T, std::string>)
                std::cout << "std::string with value " << std::quoted(arg) << '\n';
            else
                static_assert(always_false<T>::value, "non-exhaustive visitor!");
        }, w);
    }

    for (auto& v: vec) {
        visit(overloaded {
            [](auto arg) { std::cout << arg << ' '; },
            [](double arg) { std::cout << std::fixed << arg << ' '; },
            [](const std::string& arg) { std::cout << std::quoted(arg) << ' '; },
        }, v);
    }


    variant<int, double> a;
    var_t v1(5);
    cout << get<int>(a) << endl;
    var_t v2("ab");
    v1 = 41;
    cout << get<0>(v1) << endl;

    v2 = "ab";
    cout << (v1 < v2) << endl;
    //v1 < 5;
    return 0;
}

#include <variant>
void f()
{
    std::variant<int, std::string> vv;
    vv < 5;

}
