#pragma once

#include<type_traits>
#include <cstddef>
#include <iterator>

using std::cout;
using std::endl;

struct bad_iterator: std::exception
{};


static constexpr size_t SIZE = sizeof(void*);
static constexpr size_t ALIGN = sizeof(void*);

typedef std::aligned_storage<SIZE, ALIGN>::type storage_t;

template<typename I>
struct is_small
{
    static constexpr bool value = sizeof(I) <= SIZE && alignof(I) <= ALIGN && std::is_nothrow_constructible_v<I>;
};

template<typename I>
inline constexpr bool is_small_v = is_small<I>::value;

template <typename T>
struct base_ops
{
    template <typename I>
    static typename std::enable_if_t<is_small_v<I>> copy(storage_t const& from, storage_t &to){
        new (&reinterpret_cast<I&>(to)) I(reinterpret_cast<I const&>(from));
    }

    template <typename I>
    static typename std::enable_if_t<!is_small_v<I>> copy(storage_t const& from, storage_t &to){
        reinterpret_cast<I*&>(to) = new I(get<I>(from));
    }

    template<typename I>
    static typename std::enable_if_t<is_small_v<I>> mov(storage_t &&from, storage_t &to){
        new (&get<I>(to)) I(std::move(get<I>(from)));
    }

    template<typename I>
    static typename std::enable_if_t<!is_small_v<I>> mov(storage_t &&from, storage_t &to){
        reinterpret_cast<I*&>(to) = reinterpret_cast<I*&>(from);
//        new(&get<I>(to)) I(std::move(get<I>(from)));
        reinterpret_cast<I*&>(from) = nullptr;
    }

    template<typename I>
    static typename std::enable_if_t<is_small_v<I>> del(storage_t &st){
        reinterpret_cast <I&>(st).~I();
    }

    template<typename I>
    static typename std::enable_if_t<!is_small_v<I>> del(storage_t &st){
        delete (reinterpret_cast <I*&>(st));
    }

    template<typename I>
    static typename std::enable_if_t<is_small_v<I>, I&> get(storage_t &st){
        return reinterpret_cast<I&>(st);
    }

    template<typename I>
    static typename std::enable_if_t<is_small_v<I>, I const&> get(storage_t const &st){
        return reinterpret_cast<I const&>(st);
    }

    template<typename I>
    static typename std::enable_if_t<!is_small_v<I>, I&> get(storage_t &st){
        return *reinterpret_cast<I*&>(st);
    }

    template<typename I>
    static typename std::enable_if_t<!is_small_v<I>, I const&> get(storage_t const& st){
        return *reinterpret_cast<I* const&>(st);
    }

    template <typename I>
    static bool is_equal(storage_t const& a, storage_t const& b){
        return get<I>(a) == get<I>(b);
    }

    template <typename I>
    static bool is_less(storage_t const &a, storage_t const& b){
        return get<I>(a) < get<I>(b);
    }

    template <typename I>
    static ptrdiff_t diff(storage_t const& a, storage_t const& b){
        return get<I>(a) - get<I>(b);
    }

    template <typename I>
    static T& get_value(storage_t const& st){
        return *get<I>(st);
    }

    template <typename I>
    static void inc(storage_t &st){
        ++get<I>(st);
    }

    template <typename I>
    static void dec(storage_t &st){
        --get<I>(st);
    }

    template <typename I>
    static void add_n(storage_t &st, ptrdiff_t n){
        get<I>(st) += n;
    }

    static void copy_default(storage_t const&, storage_t&){}
    static void mov_default(storage_t &&, storage_t&){}
    static void del_default(storage_t &){}
    static bool is_equal_default(storage_t const&, storage_t const&){throw bad_iterator();}
    static bool is_less_default(storage_t const&, storage_t const&){throw bad_iterator();}
    static ptrdiff_t diff_default(storage_t const&, storage_t const&){throw bad_iterator();}
    static T& get_value_default(storage_t const&){throw bad_iterator();}
    static void inc_default(storage_t &){throw bad_iterator();}
    static void dec_default(storage_t &){throw bad_iterator();}
    static void add_n_default(storage_t &, ptrdiff_t n){throw bad_iterator();}
};


template <typename T, typename Tag>
struct func_ops;

template <typename T>
struct func_ops<T, std::forward_iterator_tag>
{
    using copy_t = void (*)(storage_t const&, storage_t&);
    using move_t = void (*)(storage_t&&, storage_t&);
    using del_t = void (*)(storage_t &);
    using is_equal_t = bool (*)(storage_t const&, storage_t const&);
    //    using is_less_t = bool (*)(storage_t const&, storage_t const&);
    //    using diff_t = ptrdiff_t (*)(storage_t const&, storage_t const&);
    using get_value_t = T& (*)(storage_t const&);
    using inc_t = void (*)(storage_t &);
    //    using dec_t = void (*)(storage_t &);
    //    using add_n_t = void (*)(storage_t &);

    copy_t copier;
    move_t mover;
    del_t deleter;
    is_equal_t eq_checker;
    get_value_t value_getter;
    inc_t incrementer;

    func_ops(copy_t copier, move_t mover, del_t deleter, is_equal_t eq_checker, get_value_t value_getter, inc_t incrementer):
        copier(copier), mover(mover), deleter(deleter), eq_checker(eq_checker), value_getter(value_getter), incrementer(incrementer)
    {}

    template <typename I>
    static const func_ops* get_func_ops(){
        static const func_ops instance = {
            base_ops<T>::template copy<I>,
            base_ops<T>::template mov<I>,
            base_ops<T>::template del<I>,
            base_ops<T>::template is_equal<I>,
            base_ops<T>::template get_value<I>,
            base_ops<T>::template inc<I>
        };
        return &instance;
    }

    static const func_ops* get_default_func_ops(){
        static const func_ops instance = {
            base_ops<T>::copy_default,
            base_ops<T>::mov_default,
            base_ops<T>::del_default,
            base_ops<T>::is_equal_default,
            base_ops<T>::get_value_default,
            base_ops<T>::inc_default
        };
        return &instance;
    }
};

template <typename T>
struct func_ops<T, std::bidirectional_iterator_tag>
{
    using copy_t = void (*)(storage_t const&, storage_t&);
    using move_t = void (*)(storage_t&&, storage_t&);
    using del_t = void (*)(storage_t &);
    using is_equal_t = bool (*)(storage_t const&, storage_t const&);
    //    using is_less_t = bool (*)(storage_t const&, storage_t const&);
    //    using diff_t = ptrdiff_t (*)(storage_t const&, storage_t const&);
    using get_value_t = T& (*)(storage_t const&);
    using inc_t = void (*)(storage_t &);
    using dec_t = void (*)(storage_t &);
    //    using add_n_t = void (*)(storage_t &);
    copy_t copier;
    move_t mover;
    del_t deleter;
    is_equal_t eq_checker;
    get_value_t value_getter;
    inc_t incrementer;
    dec_t decrementer;

    func_ops(copy_t copier, move_t mover, del_t deleter, is_equal_t eq_checker, get_value_t value_getter, inc_t incrementer,
             dec_t decrementer):
        copier(copier), mover(mover), deleter(deleter), eq_checker(eq_checker), value_getter(value_getter), incrementer(incrementer),
        decrementer(decrementer)
    {}

    template <typename I>
    static const func_ops* get_func_ops(){
        static const func_ops instance = {
            base_ops<T>::template copy<I>,
            base_ops<T>::template mov<I>,
            base_ops<T>::template del<I>,
            base_ops<T>::template is_equal<I>,
            base_ops<T>::template get_value<I>,
            base_ops<T>::template inc<I>,
            base_ops<T>::template dec<I>
        };
        return &instance;
    }

    static const func_ops* get_default_func_ops(){
        static const func_ops instance = {
            base_ops<T>::copy_default,
            base_ops<T>::mov_default,
            base_ops<T>::del_default,
            base_ops<T>::is_equal_default,
            base_ops<T>::get_value_default,
            base_ops<T>::inc_default,
            base_ops<T>::dec_default
        };
        return &instance;
    }
};

template <typename T>
struct func_ops<T, std::random_access_iterator_tag>
{
    using copy_t = void (*)(storage_t const&, storage_t&);
    using move_t = void (*)(storage_t&&, storage_t&);
    using del_t = void (*)(storage_t &);
    using is_equal_t = bool (*)(storage_t const&, storage_t const&);
    using is_less_t = bool (*)(storage_t const&, storage_t const&);
    using diff_t = ptrdiff_t (*)(storage_t const&, storage_t const&);
    using get_value_t = T& (*)(storage_t const&);
    using inc_t = void (*)(storage_t &);
    using dec_t = void (*)(storage_t &);
    using add_n_t = void (*)(storage_t &, ptrdiff_t n);
    copy_t copier;
    move_t mover;
    del_t deleter;
    is_equal_t eq_checker;
    is_less_t less_checker;
    diff_t differ;
    get_value_t value_getter;
    inc_t incrementer;
    dec_t decrementer;
    add_n_t adder;

    func_ops(copy_t copier, move_t mover, del_t deleter, is_equal_t eq_checker, is_less_t less_checker, diff_t differ, get_value_t value_getter,
             inc_t incrementer, dec_t decrementer, add_n_t adder):
        copier(copier), mover(mover), deleter(deleter), eq_checker(eq_checker), less_checker(less_checker), differ(differ),
        value_getter(value_getter), incrementer(incrementer),decrementer(decrementer), adder(adder)
    {}

    template <typename I>
    static const func_ops* get_func_ops(){
        static const func_ops instance = {
            base_ops<T>::template copy<I>,
            base_ops<T>::template mov<I>,
            base_ops<T>::template del<I>,
            base_ops<T>::template is_equal<I>,
            base_ops<T>::template is_less<I>,
            base_ops<T>::template diff<I>,
            base_ops<T>::template get_value<I>,
            base_ops<T>::template inc<I>,
            base_ops<T>::template dec<I>,
            base_ops<T>::template add_n<I>
        };
        return &instance;
    }

    static const func_ops* get_default_func_ops(){
        static const func_ops instance{
            base_ops<T>::copy_default,
                    base_ops<T>::mov_default,
                    base_ops<T>::del_default,
                    base_ops<T>::is_equal_default,
                    base_ops<T>::is_less_default,
                    base_ops<T>::diff_default,
                    base_ops<T>::get_value_default,
                    base_ops<T>::inc_default,
                    base_ops<T>::dec_default,
                    base_ops<T>::add_n_default
        };
        return &instance;
    }
};

template<typename T, typename Tag>
struct any_iterator;

template<typename T, typename Tag>
struct any_iterator_base
{};

template<typename T>
struct any_iterator_base<T, std::random_access_iterator_tag>
{
    using derived = any_iterator<T, std::random_access_iterator_tag>;

    T& operator[](ptrdiff_t n)
    {
        return *(static_cast<derived&>(*this) + n);
    }
};

template <typename T, typename Tag>
struct any_iterator: any_iterator_base<T, Tag>{
private:
    typedef func_ops<T, Tag> func_ops_t;
    const func_ops_t* ops;
    storage_t data;

    template<typename I>
    typename std::enable_if_t<is_small<I>::value> ctor(I it) {
        new (&data) I(std::move(it));
    }

    template<typename I>
    std::enable_if_t<!is_small_v<I>> ctor(I it) {
//        cout << *it << endl;
        reinterpret_cast<I*&>(data) = new I(std::move(it));
    }

public:

    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
    typedef Tag iterator_category;

    any_iterator() noexcept:
        ops(func_ops_t::get_default_func_ops())
    {}

    any_iterator(const any_iterator &other):
        ops(other.ops)
    {
//        std::cout << other.data.__data << std::endl;
        ops->copier(other.data, data);
    }
    any_iterator(any_iterator &&other):
        ops(other.ops)
    {
        ops->mover(std::move(other.data), data);
    }

    template <typename I>
    any_iterator(I it) noexcept(is_small_v<I>):
        ops(func_ops_t::template get_func_ops<I>())
    {
//        cout << *it << endl;
        ctor(std::move(it));
    }

    any_iterator& operator =(const any_iterator& other){
        any_iterator tmp(other);
        swap(tmp);
        return *this;
    }

    void swap(any_iterator &other) noexcept{
        any_iterator tmp(std::move(other));
        other = std::move(*this);
        *this = std::move(tmp);
    }

    any_iterator& operator =(any_iterator&& other) noexcept{
        if(this == &other){
            return *this;
        }
        ops->deleter(data);
        ops = other.ops;
        ops->mover(std::move(other.data), data);
        return *this;
    }

    template<typename I>
    any_iterator& operator =(I it) noexcept(is_small_v<I>){
        *this = any_iterator(std::move(it));
        return *this;
    }

    ~any_iterator(){
        ops->deleter(data);
    }

    explicit operator bool() const noexcept{
        return ops != func_ops_t::get_default_func_ops();
    }

    T& operator*() const{
        return ops->value_getter(data);
    }

    T* operator->() const{
        return &ops->value_getter(data);
    }

    friend bool operator ==(const any_iterator &a, const any_iterator &b){
        return (a.ops == b.ops) && a.ops->eq_checker(a.data, b.data);
    }

    friend bool operator!=(const any_iterator &a, const any_iterator &b){
        return !(a == b);
    }

    any_iterator& operator++(){
        ops->incrementer(data);
        return *this;
    }

    any_iterator operator++(int){
        any_iterator tmp(*this);
        ops->incrementer(data);
        return tmp;
    }

    //    any_iterator& operator--(){
    //        ops->decrementer(data);
    //        return *this;
    //    }

    //    any_iterator operator--(int){
    //        any_iterator tmp(*this);
    //        ops->decrementer(data);
    //        return tmp;
    //    }

    //    any_iterator& operator+=(ptrdiff_t n){
    //        ops->adder(data, n);
    //        return *this;
    //    }

    //    any_iterator& operator-=(ptrdiff_t n){
    //        ops->adder(data, -n);
    //        return *this;
    //    }

    //    ptrdiff_t operator-(any_iterator &other){
    //        if(ops != other.ops){
    //            throw bad_iterator();
    //        }
    //        return ops->differ(data, other.data);
    //    }


//    template<typename T2, typename Tag2>
//    friend bool operator==(any_iterator<T2, Tag2> &a, any_iterator<T2, Tag2> &b);

//    template<typename T2, typename Tag2>
//    friend bool operator!=(any_iterator<T2, Tag2> &a, any_iterator<T2, Tag2> &b);

    template<typename T2, typename Tag2, typename Cond>
    friend ptrdiff_t operator-(const any_iterator<T2, Tag2> &a, const any_iterator<T2, Tag2> &b);

    template<typename T2, typename Tag2, typename Cond>
    friend any_iterator<T2, Tag2>& operator--(any_iterator<T2, Tag2> &a);

    template<typename T2, typename Tag2, typename Cond>
    friend any_iterator<T2, Tag2> operator--(any_iterator<T2, Tag2> &a, int);

    template<typename T2, typename Tag2, typename Cond>
    friend any_iterator<T2, Tag2> operator+=(any_iterator<T2, Tag2> it, ptrdiff_t n);

    template<typename T2, typename Tag2, typename Cond>
    friend any_iterator<T2, Tag2> operator-=(any_iterator<T2, Tag2> it, ptrdiff_t n);

    template<typename T2, typename Tag2, typename Cond>
    friend any_iterator<T2, Tag2> operator+(any_iterator<T2, Tag2> it, ptrdiff_t n);

    template<typename T2, typename Tag2, typename Cond>
    friend any_iterator<T2, Tag2> operator+(ptrdiff_t n, any_iterator<T2, Tag2> it);

    template<typename T2, typename Tag2, typename Cond>
    friend any_iterator<T2, Tag2> operator-(any_iterator<T2, Tag2> it, ptrdiff_t n);

    template <typename T2, typename Tag2, typename Cond>
    friend bool operator<(any_iterator<T2, Tag2> &a, any_iterator<T2, Tag2>& b);

    template <typename T2, typename Tag2, typename Cond>
    friend bool operator<=(any_iterator<T2, Tag2> &a, any_iterator<T2, Tag2> &b);

    template <typename T2, typename Tag2, typename Cond>
    friend bool operator>(any_iterator<T2, Tag2> &a, any_iterator<T2, Tag2>& b);

    template <typename T2, typename Tag2, typename Cond>
    friend bool operator>=(any_iterator<T2, Tag2> &a, any_iterator<T2, Tag2>& b);
};

//template <typename T, typename Tag>
//bool operator==(const any_iterator<T, Tag> & a, const any_iterator<T, Tag> & b){
//    return (a.ops == b.ops) && a.ops->eq_checker(a.data, b.data);
//}

//template <typename T, typename Tag>
//bool operator!=(any_iterator<T, Tag> &a, any_iterator<T, Tag> &b){
//    return !(a == b);
//}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_same_v<Tag, std::random_access_iterator_tag>>>
any_iterator<T, Tag> operator+=(any_iterator<T, Tag> it, ptrdiff_t n){
    it.ops->adder(it.data, n);
    return it;
}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_same_v<Tag, std::random_access_iterator_tag>>>
any_iterator<T, Tag> operator-=(any_iterator<T, Tag> it, ptrdiff_t n){
    it.ops->adder(it.data, -n);
    return it;
}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_base_of_v<std::bidirectional_iterator_tag, Tag>>>
any_iterator<T, Tag>& operator--(any_iterator<T, Tag> &it){
    it.ops->decrementer(it.data);
    return it;
}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_base_of_v<std::bidirectional_iterator_tag, Tag>>>
any_iterator<T, Tag> operator--(any_iterator<T, Tag>& it, int){
    any_iterator<T, Tag> tmp(it);
    it.ops->decrementer(it.data);
    return tmp;
}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_base_of_v<std::bidirectional_iterator_tag, Tag>>>
ptrdiff_t operator-(const any_iterator<T, Tag>& a, const any_iterator<T, Tag> & b){
    if(a.ops != b.ops){
        throw bad_iterator();
    }
    return a.ops->differ(a.data, b.data);
}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_same_v<Tag, std::random_access_iterator_tag>>>
any_iterator<T, Tag> operator+(any_iterator<T, Tag> it, ptrdiff_t n){
    return it += n;
}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_same_v<Tag, std::random_access_iterator_tag>>>
any_iterator<T, Tag> operator-(any_iterator<T, Tag> it, ptrdiff_t n){
    return it -= n;
}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_same_v<Tag, std::random_access_iterator_tag>>>
bool operator<(any_iterator<T, Tag> &a, any_iterator<T, Tag> & b){
    if(a.ops != b.ops){
        throw bad_iterator();
    }
    return a.ops->less_checker(a.data, b.data);
}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_same_v<Tag, std::random_access_iterator_tag>>>
bool operator>=(any_iterator<T, Tag> &a, any_iterator<T, Tag> &b){
    return !(a < b);
}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_same_v<Tag, std::random_access_iterator_tag>>>
bool operator>(any_iterator<T, Tag> &a, any_iterator<T, Tag>& b){
    return b < a;
}

template <typename T, typename Tag, typename = std::enable_if_t<std::is_same_v<Tag, std::random_access_iterator_tag>>>
bool operator<=(any_iterator<T, Tag> &a, any_iterator<T, Tag>& b){
    return !(a > b);
}

template <typename ValueType>
using any_forward_iterator = any_iterator<ValueType, std::forward_iterator_tag>;

template <typename ValueType>
using any_bidirectional_iterator = any_iterator<ValueType, std::bidirectional_iterator_tag>;

template <typename ValueType>
using any_random_access_iterator = any_iterator<ValueType, std::random_access_iterator_tag>;
