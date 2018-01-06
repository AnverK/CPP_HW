#ifndef VARIANT_H
#define VARIANT_H
#include <iostream>
#include <utility>
#include <typeinfo>
#include <type_traits>
using std::cout;
using std::endl;

template <size_t arg1, size_t ... others>
struct static_max;

template <size_t arg>
struct static_max<arg>
{
    static const size_t value = arg;
};

template <size_t arg1, size_t arg2, size_t ... others>
struct static_max<arg1, arg2, others...>
{
    static const size_t value = arg1 >= arg2 ? static_max<arg1, others...>::value :
        static_max<arg2, others...>::value;
};

struct monostate
{};

constexpr bool operator<(monostate, monostate) noexcept { return false; }
constexpr bool operator>(monostate, monostate) noexcept { return false; }
constexpr bool operator<=(monostate, monostate) noexcept { return true; }
constexpr bool operator>=(monostate, monostate) noexcept { return true; }
constexpr bool operator==(monostate, monostate) noexcept { return true; }
constexpr bool operator!=(monostate, monostate) noexcept { return false; }

template<typename Types, template <typename...> class Template>
struct is_specialization: std::false_type
{};
template<template <typename...> class Template, typename ... Types>
struct is_specialization<Template<Types...>, Template>: std::true_type
{};


template <size_t I>
using build_const = std::integral_constant<size_t, I>;

template<bool is_trivially_destructible, typename ... Ts>
struct storage {
    void reset(size_t ind)
    {}
};

template<typename ... Ts>
using storage_t = storage<std::conjunction_v<std::is_trivially_destructible<Ts>...>, Ts...>;

template<typename T0, typename ... Ts>
struct storage<1, T0, Ts...>
{
    union
    {
        T0 head;
        storage_t<Ts...> tail;
    };

    constexpr storage() noexcept(std::is_nothrow_constructible_v<T0>)
        : head()
    {}

    template<typename ... Args>
    constexpr storage(build_const<0>, Args&& ... args) noexcept(std::is_nothrow_constructible_v<T0, Args...>)
        : head(std::forward<Args>(args)...)
    {}

    template<size_t I, typename ... Args>
    constexpr storage(build_const<I>, Args&& ... args)
    noexcept(std::is_nothrow_constructible_v<storage_t<Ts...>, build_const<I - 1>, Args...>)
        : tail(build_const<I - 1>{}, std::forward<Args>(args)...)
    {}

    void reset(size_t ind) noexcept
    {
        if (ind == 0)
        {
            head.~T0();
        }
        else
        {
            tail.reset(ind - 1);
        }
    }

    constexpr decltype(auto) get(build_const<0>){
        return std::forward<T0>(head);
    }
    template <size_t I>
    constexpr decltype(auto) get(build_const<I>){
        return tail.get(build_const<I - 1>{});
    }
    template<typename ...Args, typename = std::enable_if_t<std::is_constructible_v<T0, Args...>>>
    constexpr decltype(auto) set(build_const<0>, Args&&... args){
        head = T0(args...);
    }
    template <size_t I, typename ...Args>
    constexpr decltype(auto) set(build_const<I>, Args&&... args){
        return tail.set(build_const<I - 1>{}, std::forward<Args>(args)...);
    }
};


template<typename T0, typename ... Ts>
struct storage<0, T0, Ts...>
{
    union
    {
        T0 head;
        storage_t<Ts...> tail;
    };

    constexpr storage() noexcept(std::is_nothrow_constructible_v<T0>)
        : head()
    {}

    template<typename ... Args>
    constexpr storage(build_const<0>, Args&& ... args) noexcept(std::is_nothrow_constructible_v<T0, Args...>)
        : head(std::forward<Args>(args)...)
    {}

    template<size_t I, typename ... Args>
    constexpr storage(build_const<I>, Args&& ... args)
    noexcept(std::is_nothrow_constructible_v<storage_t<Ts...>, build_const<I - 1>, Args...>)
        : tail(build_const<I - 1>{}, std::forward<Args>(args)...)
    {}

    void reset(size_t ind) noexcept
    {
        if (ind == 0)
        {
            head.~T0();
        }
        else
        {
            tail.reset(ind - 1);
        }
    }
    constexpr decltype(auto) get(build_const<0>){
        return std::forward<T0>(head);
    }
    template <size_t I>
    constexpr decltype(auto) get(build_const<I>){
        return tail.get(build_const<I - 1>{});
    }

    template <typename ...Args>
    constexpr decltype(auto) set(build_const<0>, Args&&... args){
        head = T0(std::forward<T0>(args)...);
    }
    template <size_t I, typename ...Args>
    constexpr decltype(auto) set(build_const<I>, Args&&... args){
        return tail.set(build_const<I - 1>{}, std::forward<Args>(args)...);
    }

    ~storage() noexcept {}
};


template<typename T>
struct single_type
{
    static T f(T);
};

template<typename T, typename ... Ts>
struct select_type : single_type<T>, select_type<Ts...>
{
    using select_type<Ts...>::f;
    using single_type<T>::f;
};

template<typename T>
struct select_type<T> : single_type<T>
{
    using single_type<T>::f;
};

template<typename U, typename T, typename ... Ts>
using select_type_t = decltype(select_type<T, Ts...>::f(std::declval<U>()));


template <typename U, typename... Ts>
struct get_type_index{};

template <typename U, typename T, typename... Ts>
struct get_type_index<U, T, Ts...>
{
    static const size_t value = get_type_index<U, Ts...>::value + 1;
};

template <typename U, typename ...Ts>
struct get_type_index<U, U,Ts...>
{
    static const size_t value = 0;
};

template <typename U>
struct get_type_index<U>
{
    static const size_t value = 0;
};


template <size_t I, typename... Ts>
struct get_type{
};

template<size_t I, typename T, typename... Ts>
struct get_type<I, T, Ts...>
{
    typedef typename get_type<I-1, Ts...>::type type;
};

template<typename T, typename... Ts>
struct get_type<0, T, Ts...>
{
    typedef T type;
};

template<size_t I>
struct get_type<I>
{
    typedef void type;
};

template <size_t I, typename... Ts>
using get_type_t = typename get_type<I, Ts...>::type;



template<typename T>
struct is_in_place_index_specialization
{
    static constexpr bool value = 0;
};

template<size_t I>
struct is_in_place_index_specialization<std::in_place_index_t<I>>
{
    static constexpr bool value = 1;
};

template<typename T0, typename... Ts>
struct variant{
private:
    static const size_t data_size = static_max<sizeof(T0), sizeof(Ts)...>::value;
    static const size_t data_align = static_max<alignof(T0), alignof(Ts)...>::value;
    
//    using data_t = typename std::aligned_storage<data_size, data_align>::type;
    
    static inline constexpr size_t invalid_type() {
        return get_type_index<void, T0, Ts...>::value;
    }

    size_t type_id;
    storage_t <T0, Ts...> data;
public:

    template<typename = std::enable_if_t<std::is_default_constructible_v<T0>>>
    constexpr variant() noexcept(std::is_nothrow_default_constructible_v<T0>)
        : type_id(get_type_index<T0, T0, Ts...>::value),
          data()
    {
        //        typename get_type<5, T0, Ts...>::type i(1);
        //        cout << i.size() << endl;
        //        cout << i({1, 2}) << endl;
    }

    variant(const variant<T0, Ts...>& other) = default;

    variant(variant<T0, Ts...>&& other) = default;

    template <typename T, typename varT = select_type_t<T, T0, Ts...>,
              typename = std::enable_if_t<
                  !std::is_same_v<std::decay_t<T>, variant> &&
                  std::is_constructible_v<varT, T>>, size_t I = get_type_index<varT, T0, Ts...>::value>
    constexpr variant(T&& t) noexcept(std::is_nothrow_constructible_v<varT, T>):
        type_id(I),
        data(build_const<I>{}, std::forward<T>(t))
    {}

    template <typename T, typename ...Args,
              size_t I = get_type_index<T, T0, Ts...>::value,
              typename = std::enable_if_t<
                  !(I >= invalid_type()) &&                      // < but parsing...
                  std::is_constructible_v<T, Args...>>>
    constexpr variant(std::in_place_type_t<T> t, Args&&... args):
        type_id(I),
        data(build_const<I>{}, std::forward<Args>(args)...)
    {}

    template <size_t I, typename ...Args, typename T = get_type_t<I, T0, Ts...>,
              typename = std::enable_if_t<
                  !(I >= invalid_type()) &&                       // < but parsing...
                  std::is_constructible_v<T, Args...>
                  >
              >
    constexpr variant(std::in_place_index_t<I>, Args&&... args):
        type_id(I),
        data(build_const<I>{}, std::forward<Args>(args)...)
    {}

    constexpr std::size_t index() const noexcept{
        return std::min(type_id, invalid_type());
    }

//    variant<T0, Ts...>& operator= (variant<T0, Ts...> old)
//    {
//        std::swap(type_id, old.type_id);
//        std::swap(data, old.data);

//        return *this;
//    }

    template<size_t I>
    constexpr decltype(auto) get(){
        if(I >= invalid_type() || I != type_id){
            throw std::bad_cast();
        }
        return data.get(build_const<I>{});
    }

    template<size_t I, typename ...Args>
    constexpr void set(Args&&... args){
        if(I >= invalid_type()){
            type_id = invalid_type();
            throw std::bad_cast();
        }
        data.reset(type_id);
        type_id = I;
        data.set(build_const<I>{}, std::forward<Args>(args)...);
    }

    template<typename T>
    constexpr decltype(auto) get(){
        constexpr size_t index = get_type_index<T, T0, Ts...>::value;
        if(index >= invalid_type() || index != type_id){
            type_id = invalid_type();
            throw std::bad_cast();
        }
        else{
            return data.get(build_const<index>{});
        }
    }

    template<typename T, typename ...Args, size_t I = get_type_index<T, T0, Ts...>::value>
    constexpr void set(Args&&... args) {
//        cout << typeid(T).name() << endl;
        constexpr size_t index = get_type_index<T, T0, Ts...>::value;
        if(index == invalid_type()){
            type_id = invalid_type();
            throw std::bad_cast();
        }
        else{
            data.reset(type_id);
            type_id = index;
            data.set(build_const<index>{}, std::forward<Args>(args)...);
        }
    }

    bool is_valid() {
        return (type_id < invalid_type());
    }
};

#endif // VARIANT_H
