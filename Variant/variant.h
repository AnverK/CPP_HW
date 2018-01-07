#ifndef VARIANT_H
#define VARIANT_H
#include <iostream>
#include <utility>
#include <typeinfo>
#include <type_traits>
#include <functional>
#include "storage.h"
#include "variant_alternative.h"
using std::cout;
using std::endl;

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
struct variant_size;

template <typename... Types>
struct variant_size<variant<Types...>>: std::integral_constant<std::size_t, sizeof...(Types)>
{};

template <typename T>
struct variant_size<const T> : variant_size<T>
{};

template <typename T>
struct variant_size<volatile T> : variant_size<T>
{};
template <typename T>
struct variant_size<const volatile T> : variant_size<T>
{};

template <typename T>
inline constexpr size_t variant_size_v = variant_size<T>::value;

template<typename U, typename ...Ts>
struct count_type
{};

template<typename U, typename V, typename... Ts>
struct count_type<U, V, Ts...>
{
    static constexpr size_t value = std::is_same<U, V>::value + count_type<U, Ts...>::value;
};

template<typename U, typename V>
struct count_type<U, V>
{
    static constexpr size_t value = std::is_same_v<U, V>;
};

template<typename U>
struct count_type<U>
{
    static constexpr size_t value = 0;
};

template<typename U, typename ... Ts>
inline constexpr bool is_unique_v = count_type<U, Ts...>::value <= 1;


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

class bad_variant_access : public std::exception
{};


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

template <typename T, typename ... Ts, size_t I = get_type_index<T, Ts...>::value, typename = std::enable_if_t<is_unique_v<T, Ts...> >>
constexpr decltype(auto) get(variant<Ts...>& v)
{
    return get<I>(v);
}

template <typename T, typename ... Ts, size_t I = get_type_index<T, Ts...>::value, typename = std::enable_if_t<is_unique_v<T, Ts...> >>
constexpr decltype(auto) get(variant<Ts...>&& v)
{
    return get<I>(std::move(v));
}

template <typename T, typename ... Ts, size_t I = get_type_index<T, Ts...>::value, typename = std::enable_if_t<is_unique_v<T, Ts...> >>
constexpr decltype(auto) get(variant<Ts...> const& v)
{
    return get<I>(v);
}

template <typename T, typename ... Ts, size_t I = get_type_index<T, Ts...>::value, typename = std::enable_if_t<is_unique_v<T, Ts...> >>
constexpr decltype(auto) get(variant<Ts...> const&& v)
{
    return get<I>(std::move(v));
}


template<typename T0, typename... Ts>
struct variant: copy_assignable_storage_t<T0, Ts...>
{
private:

    using data = copy_assignable_storage_t<T0, Ts...>;

    using data::valueless_by_exception_impl;
    using data::set_index;
    using data::ind;
    using data::reset;
    using data::move_constructor;
    using data::copy_constructor;

    template <size_t I>
    using get_type_t = variant_alternative_t<I, variant>;

    static inline constexpr size_t invalid_type() {
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
                  std::is_constructible_v<varT, T>  &&
                  is_unique_v<varT, T0, Ts...> >,
              size_t I = get_type_index<varT, T0, Ts...>::value
              >
    constexpr variant(T&& t) noexcept(std::is_nothrow_constructible_v<varT, T>):
        data(build_const<I>{}, std::forward<T>(t))
    {}
    template <typename T, typename ...Args,
              size_t I = get_type_index<T, T0, Ts...>::value,
              typename = std::enable_if_t<
                  (I != invalid_type()) &&
                  std::is_constructible_v<T, Args...> &&
                  is_unique_v<std::in_place_type<T>, T0, Ts...> >>
    constexpr variant(std::in_place_type_t<T> t, Args&&... args):
        data(build_const<I>{}, std::forward<Args>(args)...)
    {}

    template <size_t I, typename ...Args, typename T = get_type_t<I>,
              typename = std::enable_if_t<
                  (I != invalid_type()) &&
                  std::is_constructible_v<T, Args...>
                  >
              >
    constexpr variant(std::in_place_index_t<I>, Args&&... args):
        data(build_const<I>{}, std::forward<Args>(args)...)
    {}

    template<typename T, typename varT = select_type_t<T, T0, Ts...>,
             typename = std::enable_if_t<std::is_constructible_v<varT, T> && std::is_assignable_v<varT&, T>&&
                                         is_unique_v<varT, T0, Ts...> >,
             size_t I = get_type_index<varT, T0, Ts...>::value>
    variant& operator =(T&& t) noexcept{
        if(I == index()){
            get<I>(*this) = std::forward<T>(t);
            return *this;
        }
        return this->operator=(variant(std::forward<T>(t)));
    }

    constexpr std::size_t index() const noexcept{
        return std::min(ind(), invalid_type());
    }

    template <typename T, typename... Args, size_t I = get_type_index<T, T0, Ts...>::value,
              typename = std::enable_if_t<std::is_constructible_v<T, Args...> && is_unique_v<T, T0, Ts...> >>
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

    void swap(variant& rhs) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<Ts>..., std::is_nothrow_move_constructible<T0>,
                                     std::is_nothrow_swappable<Ts>..., std::is_nothrow_swappable<T0>>)
    {
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

template <typename T, typename ... Ts, typename = std::enable_if_t<is_unique_v<T, Ts...> >>
constexpr decltype(auto) get_if(variant<Ts...>* pv) noexcept
{
    constexpr size_t I = get_type_index<T, Ts...>();
    return get_if<I>(pv);
}

template <typename T, typename ... Ts, typename = std::enable_if_t<is_unique_v<T, Ts...> >>
constexpr decltype(auto) get_if(const variant<Ts...>* pv) noexcept
{
    constexpr size_t I = get_type_index<T, Ts...>();
    return get_if<I>(pv);
}


template<typename T, size_t... dimensions>
struct multi_array
{
    constexpr const T& access() const{
        return data;
    }
    T data;
};

template<typename T, size_t first, size_t... rest>
struct multi_array<T, first, rest...>
{
    template<typename... Args>
    constexpr const T& access(size_t first_index, Args... rest_indices) const {
        return arr[first_index].access(rest_indices...);
    }

    multi_array<T, rest...> arr[first];
};

template<typename array_type, typename variant_tuple, typename index_seq>
struct gen_vtable_impl;

template<size_t N, typename... Types>
struct nth_type;

template<size_t N, typename first, typename... rest>
struct nth_type<N, first, rest...>: nth_type<N - 1, rest...> {};

template<typename first, typename... rest>
struct nth_type<0, first, rest...>
{
    using type = first;
};

template<typename result_type, typename visitor, size_t... dimensions, typename... variants, size_t... indices>
struct gen_vtable_impl<
        multi_array<result_type (*)(visitor, variants...), dimensions...>,
std::tuple<variants...>, std::index_sequence<indices...>
>
{
    using next = std::remove_reference_t<typename nth_type<sizeof...(indices),variants...>::type>;
    using array_type = multi_array<result_type (*)(visitor, variants...), dimensions...>;

    static constexpr array_type apply()
    {
        array_type vtable{};
        apply_all(vtable, std::make_index_sequence<variant_size_v<next>>());
        return vtable;
    }

    template<size_t... var_indices>
    static constexpr void apply_all(array_type& vtable, std::index_sequence<var_indices...>)
    {
        (apply_single<var_indices>(vtable.arr[var_indices]), ...);
    }

    template<size_t index, typename T>
    static constexpr void apply_single(T& element)
    {
        element = gen_vtable_impl<
                std::remove_reference_t<decltype(element)>,
                std::tuple<variants...>,
                std::index_sequence<indices..., index>>::apply();
    }
};

template<typename result_type, typename visitor, typename... variants, size_t... indices>
struct gen_vtable_impl<
        multi_array<result_type (*)(visitor, variants...)>,
std::tuple<variants...>,
std::index_sequence<indices...>
>
{
    using array_type = multi_array<result_type (*)(visitor&&, variants...)>;

    decltype(auto) static constexpr visit_invoke(visitor&& vis, variants... vars)
    {
        return std::invoke(std::forward<visitor>(vis), get<indices>(std::forward<variants>(vars))...);
    }
    static constexpr auto apply()
    {
        return array_type{&visit_invoke};
    }
};

template<typename result_type, typename visitor, typename... variants>
struct gen_vtable
{
    using func_ptr = result_type (*)(visitor&&, variants...);
    using array_type = multi_array<func_ptr, variant_size_v<std::remove_reference_t<variants>>...>;

    static constexpr array_type apply()
    {
        return gen_vtable_impl<array_type, std::tuple<variants...>, std::index_sequence<>>::apply();
    }

    static constexpr auto vtable = apply();
};


template<typename Visitor, typename... Variants>
constexpr decltype(auto) visit(Visitor&& visitor, Variants&& ... variants)
{
    if ((variants.valueless_by_exception() || ...))
    {
        throw bad_variant_access();
    }

    using result_type = decltype(std::forward<Visitor>(visitor)(get<0>(std::forward<Variants>(variants))...));

    constexpr auto& v_table = gen_vtable<result_type, Visitor&&, Variants&&...>::vtable;
    auto func_ptr = v_table.access(variants.index()...);

    return (*func_ptr)(std::forward<Visitor>(visitor), std::forward<Variants>(variants)...);
}

auto comparator = [](auto&& action) constexpr {
    return ([action](auto&& a, auto&& b) constexpr -> bool  {
        if constexpr(std::is_same_v<decltype(a), decltype(b)>){
            return action(a, b);
        }
        else{
            return 0;
        }
    });
};

auto less = comparator([](auto&& x, auto&& y) constexpr -> bool  {return x < y; });
auto greater = comparator([](auto&& x, auto&& y) constexpr -> bool  {return x > y; });
auto equal = comparator([](auto&& x, auto&& y) constexpr -> bool  {return x == y; });
auto neq = comparator([](auto&& x, auto&& y) constexpr -> bool  {return x != y; });
auto leq = comparator([](auto&& x, auto&& y) constexpr -> bool  {return x <= y;});
auto geq = comparator([](auto&& x, auto&& y) constexpr -> bool  {return x >= y;});

template <typename ... Ts>
constexpr bool operator==(const variant<Ts...>& v, const variant<Ts...>& w)
{
    if (v.index() != w.index()){
        return false;
    }
    if (v.valueless_by_exception()){
        return true;
    }
    return visit(equal, v, w);
}

template <typename ... Ts>
constexpr bool operator!=(const variant<Ts...>& v, const variant<Ts...>& w)
{
    if (v.index() != w.index()){
        return true;
    }
    if (v.valueless_by_exception())
    {
        return false;
    }
    return visit(neq, v, w);
}

template <typename ... Ts>
constexpr bool operator<(const variant<Ts...>& v, const variant<Ts...>& w)
{
    if (w.valueless_by_exception()){
        return false;
    }
    if (v.valueless_by_exception()){
        return true;
    }
    if (v.index() == w.index()){
        return visit(less, v, w);
    }
    else{
        return v.index() < w.index();
    }
}

template <typename ... Ts>
constexpr bool operator>(const variant<Ts...>& v, const variant<Ts...>& w)
{
    if (v.valueless_by_exception()){
        return false;
    }
    if (w.valueless_by_exception()){
        return true;
    }
    if (v.index() == w.index()){
        return visit(greater, v, w);
    }
    else{
        return v.index() > w.index();
    }
}

template <typename ... Ts>
constexpr bool operator<=(const variant<Ts...>& v, const variant<Ts...>& w)
{
    if (v.valueless_by_exception()){
        return true;
    }
    if (w.valueless_by_exception()){
        return false;
    }
    if (v.index() == w.index()){
        return  visit(leq, v, w);
    }
    else{
        return v.index() < w.index();
    }
}

template <typename ... Ts>
constexpr bool operator>=(const variant<Ts...>& v, const variant<Ts...>& w)
{
    if (w.valueless_by_exception()){
        return true;
    }
    if (v.valueless_by_exception()){
        return false;
    }
    if (v.index() == w.index()){
        return visit(geq, v, w);
    }
    else{
        return v.index() < w.index();
    }
}

#endif // VARIANT_H
