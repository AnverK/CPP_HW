#ifndef VARIANT_ALTERNATIVE_H
#define VARIANT_ALTERNATIVE_H

template <typename T0, typename ... Ts> struct variant;

template<size_t I, typename T, bool ok = 1>
struct variant_alternative;

template <size_t I, typename T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

template<size_t I, typename T>
struct variant_alternative<I, const T>
{
    typedef std::add_const_t<typename variant_alternative<I, T>::type> type;
};

template<size_t I, typename T>
struct variant_alternative<I, volatile T>
{
    typedef std::add_volatile_t<typename variant_alternative<I, T>::type> type;
};

template<size_t I, typename T>
struct variant_alternative<I, const volatile T>
{
    typedef std::add_volatile_t<std::add_const_t<typename variant_alternative<I, T>::type>> type;
};

template<size_t I, typename T0, typename ... Ts>
struct variant_alternative<I, variant<T0, Ts...>, 1>
{
    typedef typename variant_alternative<I - 1, variant<Ts...>, (I <= sizeof...(Ts))>::type type;
};

template<typename T0, typename ... Ts>
struct variant_alternative<0, variant<T0, Ts...>>
{
    typedef T0 type;
};

struct empty;

template<size_t I, typename ... Ts>
struct variant_alternative<I, variant<Ts...>, 0>
{
    typedef empty type;
};

#endif // VARIANT_ALTERNATIVE_H
