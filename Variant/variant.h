#ifndef VARIANT_H
#define VARIANT_H
#include <iostream>
#include <utility>
#include <typeinfo>
#include <type_traits>
#include "storage.h"
#include "variant_alternative.h"
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

template <typename T0, typename... Ts>
constexpr bool all_copy_constructible = std::is_copy_constructible_v<T0> && std::conjunction_v<std::is_copy_constructible<Ts>...>;

template <typename T0, typename... Ts>
constexpr bool all_move_constructible = std::is_move_constructible_v<T0> && std::conjunction_v<std::is_move_constructible<Ts>...>;

class bad_variant_access : public std::exception
{};

template<typename T0, typename... Ts>
struct variant: copyable_storage_t<T0, Ts...>
{
private:
    using data = copyable_storage_t<T0, Ts...>;

    using data::valueless_by_exception_impl;
    using data::set_index;
    using data::ind;
    using data::reset;
    using data::move_constructor;
    using data::copy_constructor;


    template <size_t I>
    using get_type_t = variant_alternative_t<I, variant>;

    void make_valueless() noexcept{
        index() = invalid_type();
    }

    static inline constexpr size_t invalid_type() {
        //        return variant_npos;
        return variant_npos;
    }

public:
    template<typename = std::enable_if_t<std::is_default_constructible_v<T0>>>
    constexpr variant() noexcept(std::is_nothrow_default_constructible_v<T0>)
        :data(build_const<0>{})
    {}

    template <typename T, typename varT = select_type_t<T, T0, Ts...>,
              typename = std::enable_if_t<
                  !std::is_same_v<std::decay_t<T>, variant> &&
                  std::is_constructible_v<varT, T>>, size_t I = get_type_index<varT, T0, Ts...>::value>
    constexpr variant(T&& t) noexcept(std::is_nothrow_constructible_v<varT, T>):
        data(build_const<I>{}, std::forward<T>(t))
    {}
    template <typename T, typename ...Args,
              size_t I = get_type_index<T, T0, Ts...>::value,
              typename = std::enable_if_t<
                  !(I >= invalid_type()) &&                      // < but parsing...
                  std::is_constructible_v<T, Args...>>>
    constexpr variant(std::in_place_type_t<T> t, Args&&... args):
        data(build_const<I>{}, std::forward<Args>(args)...)
    {}

    template <size_t I, typename ...Args, typename T = get_type_t<I>,
              typename = std::enable_if_t<
                  !(I >= invalid_type()) &&                       // < but parsing...
                  std::is_constructible_v<T, Args...>
                  >
              >
    constexpr variant(std::in_place_index_t<I>, Args&&... args):
        data(build_const<I>{}, std::forward<Args>(args)...)
    {}

    constexpr std::size_t index() const noexcept{
        return std::min(ind(), invalid_type());
    }

    //    variant<T0, Ts...>& operator= (const variant<T0, Ts...> &other)     //TODO
    //    {
    //        if(valueless_by_exception()){
    //            return *this;
    //        }
    //        if(other.valueless_by_exception()){
    //            data.reset(type_id);
    //            make_valueless();
    //            return *this;
    //        }
    //        if(type_id == other.type_id){
    //            data = other.data;
    //        }
    //        type_id = other.type_id;
    //        data = other.data;
    //        return *this;
    //    }

    //        variant<T0, Ts...>& operator= (variant<T0, Ts...> &&other)      //TODO
    //        {
    ////            std::swap(data, other.data);
    //            return *this;
    //        }


    template <typename T, typename... Args, size_t I = get_type_index<T, T0, Ts...>::value,
              typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    T& emplace(Args&&... args){
        reset(index());
        copy_constructor(index(), std::forward<Args>(args)...);
        return get_storage_data(build_const<I>{}, *this);
    }
    template <size_t I, typename... Args, typename T = get_type_t<I>,
              typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    variant_alternative_t<I, variant>& emplace(Args&&... args){
        return emplace<T>(std::forward<Args>(args)...);
    }

    constexpr bool valueless_by_exception() const noexcept
    {
        return valueless_by_exception_impl();
    }

    void swap(variant& rhs) noexcept{
        if(rhs.valueless_by_exception() && valueless_by_exception()){
            return;
        }
        if(index() == rhs.index()){
            swap_data(index(), *this, rhs);
            return;
        }

        variant buf(std::move(rhs));
        if(!rhs.valueless_by_exception()){
            rhs.reset(rhs.index());
        }
        rhs.set_index(index());
        if(!valueless_by_exception()){
            rhs.move_constructor(index(), std::move(*this));
        }

        if(valueless_by_exception()){
            reset(index());
        }
        set_index(buf.index());
        if(!buf.valueless_by_exception()){
            move_constructor(index(), std::move(buf));
        }
    }

};


template <typename T, typename ... Types>
constexpr bool holds_alternative(const variant<Types...>& v) noexcept
{
    return !v.valueless_by_exception() && v.index() == get_type_index<T, Types...>::value;
}

template <size_t I, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...>& v)
{
    if (v.index() != I){
        throw bad_variant_access();
    }
    return get_storage_data(build_const<I>{}, v.get_storage());
}

template <size_t I, typename ... Ts>
constexpr variant_alternative_t<I, variant<Ts...>>&& get(variant<Ts...>&& v){
    if (v.index() != I){
        throw bad_variant_access();
    }
    return get_storage_data(build_const<I>{}, std::move(v).get_storage());
}

template <size_t I, typename ... Ts>
constexpr decltype(auto) get(const variant<Ts...> & v){
    if (v.index() != I){
        throw bad_variant_access();
    }
    return get_storage_data(build_const<I>{}, v.get_storage());
}

template <size_t I, typename ... Ts>
constexpr decltype(auto) get(const variant<Ts...>&& v){
    if (v.index() != I){
        throw bad_variant_access();
    }
    return get_storage_data(build_const<I>{}, std::move(v).get_storage());
}

template <typename T, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...>& v)
{
    constexpr size_t I = get_type_index<T, Ts...>();
    return get<I>(v);
}

template <typename T, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...>&& v)
{
    constexpr size_t I = get_type_index<T, Ts...>();
    return get<I>(std::move(v));
}

template <typename T, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...> const& v)
{
    constexpr size_t I = get_type_index<T, Ts...>();
    return get<I>(v);
}

template <typename T, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...> const&& v)
{
    constexpr size_t I = get_type_index<T, Ts...>();
    return get<I>(std::move(v));
}

template <size_t I, typename ... Ts>
constexpr decltype(auto) get_if(variant<Ts...>* pv) noexcept
{
    if(I >= sizeof...(Ts)){
        throw bad_variant_access();
    }
    if(pv != nullptr && pv->index() != I){
        return &get<I>(*pv);
    }
    return nullptr;
}

template <size_t I, typename ... Ts>
constexpr decltype(auto) get_if(const variant<Ts...>* pv) noexcept
{
    if(I >= sizeof...(Ts)){
        throw bad_variant_access();
    }
    if(pv != nullptr && pv->index() != I){
        return &get<I>(*pv);
    }
    return nullptr;
}

template <typename T, typename ... Ts>
constexpr decltype(auto) get_if(variant<Ts...>* pv) noexcept
{
    constexpr size_t I = get_type_index<T, Ts...>();
    return get_if<I>(pv);
}

template <typename T, typename ... Ts>
constexpr decltype(auto) get_if(const variant<Ts...>* pv) noexcept
{
    constexpr size_t I = get_type_index<T, Ts...>();
    return get_if<I>(pv);
}

#endif // VARIANT_H
