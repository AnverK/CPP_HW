#pragma once
#include "any_iterator.h"
#include <cassert>
#include <any>
#include <vector>
#include <list>
#include <deque>
#include <iostream>

//using std::cout;
//using std::endl;

static constexpr size_t CONT_SIZE = 8*sizeof(void*);
static constexpr size_t CONT_ALIGN = sizeof(void*);

typedef std::aligned_storage<CONT_SIZE, CONT_ALIGN>::type container_t;

template<typename C>
struct is_small_container
{
    static constexpr bool value = sizeof(C) <= CONT_SIZE && alignof(C) <= CONT_ALIGN && std::is_nothrow_constructible_v<C>;
};

struct bad_container: std::exception
{};

template<typename C>
inline constexpr bool is_small_container_v = is_small_container<C>::value;

template <typename T, typename Tag>
struct base_container_ops
{
    typedef any_iterator<T, Tag> iter;
    typedef any_iterator<const T, Tag> const_iter;

    template <typename I>
    static typename std::enable_if_t<is_small_container_v<I>> copy(container_t const& from, container_t &to){
        new (&reinterpret_cast<I&>(to)) I(reinterpret_cast<I const&>(from));
    }

    template <typename I>
    static typename std::enable_if_t<!is_small_container_v<I>> copy(container_t const& from, container_t &to){
        reinterpret_cast<I*&>(to) = new I(get<I>(from));
    }

    template<typename I>
    static typename std::enable_if_t<is_small_container_v<I>> mov(container_t &&from, container_t &to){
        new (&get<I>(to)) I(std::move(get<I>(from)));
    }

    template<typename I>
    static typename std::enable_if_t<!is_small_container_v<I>> mov(container_t &&from, container_t &to){
        reinterpret_cast<I*&>(to) = reinterpret_cast<I*&>(from);
        reinterpret_cast<I*&>(from) = nullptr;
    }

    template<typename I>
    static typename std::enable_if_t<is_small_container_v<I>> del(container_t &st){
        reinterpret_cast <I&>(st).~I();
    }

    template<typename I>
    static typename std::enable_if_t<!is_small_container_v<I>> del(container_t &st){
        delete (reinterpret_cast <I*&>(st));
    }

    template<typename I>
    static typename std::enable_if_t<is_small_container_v<I>, I&> get(container_t &st){
        return reinterpret_cast<I&>(st);
    }

    template<typename I>
    static typename std::enable_if_t<is_small_container_v<I>, I const&> get(container_t const &st){
        return reinterpret_cast<I const&>(st);
    }

    template<typename I>
    static typename std::enable_if_t<!is_small_container_v<I>, I&> get(container_t &st){
        return *reinterpret_cast<I*&>(st);
    }

    template<typename I>
    static typename std::enable_if_t<!is_small_container_v<I>, I const&> get(container_t const& st){
        return *reinterpret_cast<I* const&>(st);
    }

    template<typename I>
    static iter get_beg(container_t &st){
        return get<I>(st).begin();
    }

    template<typename I>
    static iter get_end(container_t &st){
        return get<I>(st).end();
    }

    //    template<typename I>
    //    static const_iter get_cbeg(const container_t &st){
    //        //        iter it = get<I>(st).begin();
    //        return get<I>(st).begin();
    //    }

    //    template<typename I>
    //    static const_iter get_cend(const container_t &st){
    //        return get<I>(st).end();
    //    }

    template<typename I>
    static iter ins(const_iter it, const T& el, container_t &st){
        typedef typename std::remove_reference_t<decltype(get<I>(st))>::const_iterator to;
        if constexpr(is_small_v<to>){
            return get<I>(st).insert(reinterpret_cast<to&>(it.get_data()), el);
        }
        else{
            to pos = *reinterpret_cast<to*&>(it.get_data());
            return get<I>(st).insert(pos, el);
        }
    }

    template<typename I>
    static iter ers(const_iter it, container_t &st){
        typedef typename std::remove_reference_t<decltype(get<I>(st))>::const_iterator to;
//        if(it.is_small_object() ){
        if constexpr(is_small_v<to>){
                return get<I>(st).erase(reinterpret_cast<to&>(it.get_data()));
        }
        else{
            return get<I>(st).erase(*reinterpret_cast<to*&>(it.get_data()));
        }
    }

    template<typename I>
    static size_t get_size(const container_t &st){
        return get<I>(st).size();
    }

    static void copy_default(container_t const&, container_t&){}
    static void mov_default(container_t &&, container_t&){}
    static void del_default(container_t &){}
    static iter get_beg_default(container_t &){throw bad_container();}
    static iter get_end_default(container_t &){throw bad_container();}
    //    static const_iter get_cbeg_default(const container_t &){throw bad_container();}
    //    static const_iter get_cend_default(const container_t &){throw bad_container();}
    static iter ins_default(const_iter, const T&, container_t&){throw bad_container();}
    static iter ers_default(const_iter, container_t&){throw bad_container();}
    static size_t get_size_default(const container_t &){return 0;}
};

template <typename T, typename Tag>
struct func_container_ops {
    typedef any_iterator<T, Tag> iter;
    typedef any_iterator<const T, Tag> const_iter;

    using copy_t = void (*)(container_t const&, container_t&);
    using move_t = void (*)(container_t&&, container_t&);
    using del_t = void (*)(container_t &);
    using beg_t = iter (*)(container_t&);
    using end_t = iter (*)(container_t&);
    //    using cbeg_t = const_iter (*)(const container_t&);
    //    using cend_t = const_iter (*)(const container_t&);
    using ins_t = iter (*)(const_iter, const T&, container_t&);
    using ers_t = iter (*)(const_iter, container_t&);
    using sz_t = size_t (*)(const container_t &);

    copy_t copier;
    move_t mover;
    del_t deleter;
    beg_t beg_getter;
    end_t end_getter;
    //    cbeg_t cbeg_getter;
    //    cend_t cend_getter;
    ins_t inserter;
    ers_t eraser;
    sz_t size_getter;

    func_container_ops(copy_t copier, move_t mover, del_t deleter, beg_t beg_getter, end_t end_getter,
                       //                       cbeg_t cbeg_getter,
                       //                       cend_t cend_getter,
                       ins_t inserter, ers_t eraser, sz_t size_getter):
        copier(copier), mover(mover), deleter(deleter), beg_getter(beg_getter), end_getter(end_getter),
        //        cbeg_getter(cbeg_getter),
        //        cend_getter(cend_getter),
        inserter(inserter), eraser(eraser), size_getter(size_getter)
    {}
    template <typename I>
    static const func_container_ops* get_func_ops(){
        static const func_container_ops instance = {
            base_container_ops<T, Tag>::template copy<I>,
            base_container_ops<T, Tag>::template mov<I>,
            base_container_ops<T, Tag>::template del<I>,
            base_container_ops<T, Tag>::template get_beg<I>,
            base_container_ops<T, Tag>::template get_end<I>,
            //            base_container_ops<T, Tag>::template get_cbeg<I>,
            //            base_container_ops<T, Tag>::template get_cend<I>,
            base_container_ops<T, Tag>::template ins<I>,
            base_container_ops<T, Tag>::template ers<I>,
            base_container_ops<T, Tag>::template get_size<I>
        };
        return &instance;
    }

    static const func_container_ops* get_default_func_ops(){
        static const func_container_ops instance = {
            base_container_ops<T, Tag>::copy_default,
            base_container_ops<T, Tag>::mov_default,
            base_container_ops<T, Tag>::del_default,
            base_container_ops<T, Tag>::get_beg_default,
            base_container_ops<T, Tag>::get_end_default,
            //            base_container_ops<T, Tag>::get_cbeg_default,
            //            base_container_ops<T, Tag>::get_cend_default,
            base_container_ops<T, Tag>::ins_default,
            base_container_ops<T, Tag>::ers_default,
            base_container_ops<T, Tag>::get_size_default
        };
        return &instance;
    }
};

template <typename T, typename Container, typename Tag = std::random_access_iterator_tag>
struct any_container;

template <typename InnerContainer>
struct is_any_container
{
    static constexpr bool value = false;
};

template <typename ValueType, typename Container, typename Category>
struct is_any_container<any_container<ValueType, Container, Category> >
{
    static constexpr bool value = true;
};

template <typename InnerContainer, typename InnerContainerRef>
typename std::enable_if<is_small_container_v<InnerContainer>>::type inner_container_construct(container_t& dst, InnerContainerRef&& it)
{
    static_assert(std::is_same<typename std::decay<InnerContainerRef>::type, InnerContainer>::value);
    new (&dst) InnerContainer(std::forward<InnerContainerRef>(it));
}

template <typename InnerContainer, typename InnerContainerRef>
typename std::enable_if<!is_small_container_v<InnerContainer>>::type inner_container_construct(container_t& dst, InnerContainerRef&& it)
{
    static_assert(std::is_same<typename std::decay<InnerContainerRef>::type, InnerContainer>::value);
    new (&dst) InnerContainer*(new InnerContainer(std::forward<InnerContainerRef>(it)));
}

template <typename T, typename Container, typename Tag>
struct any_container{
private:
    typedef any_iterator<T, Tag> iter;
    typedef any_iterator<const T, Tag> const_iter;
    typedef func_container_ops<T, Tag> func_container_ops_t;
    const func_container_ops_t* ops;
    container_t data;
public:

    any_container() noexcept:
        ops(func_container_ops_t::get_default_func_ops())
    {}

    any_container(const any_container &other):
        ops(other.ops)
    {
        ops->copier(other.data, data);
    }

    any_container(any_container &&other):
        ops(other.ops)
    {
        ops->mover(std::move(other.data), data);
    }

    any_container(const Container& c):
        ops(func_container_ops_t::template get_func_ops<std::decay_t<Container>>())
    {
        inner_container_construct<std::decay_t<Container>>(data, c);
//        inner_container_construct<std::decay_t<Container>>(data, std::forward<Container>(c));
    }

    any_container(Container&& c):
        ops(func_container_ops_t::template get_func_ops<std::decay_t<Container>>())
    {
        inner_container_construct<std::decay_t<Container>>(data, std::forward<Container>(c));
    }

    any_container& operator =(const any_container& other){
        any_container tmp(other);
        swap(tmp);
        return *this;
    }

    any_container& operator =(any_container&& other) noexcept{
        if(this == &other){
            return *this;
        }
        ops->deleter(data);
        ops = other.ops;
        ops->mover(std::move(other.data), data);
        return *this;
    }

    template<typename I>
    any_container& operator =(I it) noexcept(is_small_container_v<I>){
        *this = any_container(std::move(it));
        return *this;
    }

    void swap(any_container &other) noexcept{
        any_container tmp(std::move(other));
        other = std::move(*this);
        *this = std::move(tmp);
    }

    iter begin(){
        return ops->beg_getter(data);
    }

    iter end(){
        return ops->end_getter(data);
    }

    const_iter cbegin() const noexcept{
        return ops->beg_getter(const_cast<container_t&>(data));
    }

    const_iter cend() const noexcept{
        return ops->end_getter(const_cast<container_t&>(data));
    }

    iter insert(const_iter pos, T const& el){
        assert(pos.is_same_type(cend()));
        return ops->inserter(pos, el, data);
    }

    iter erase(const_iter pos){
        assert(pos.is_same_type(cend()));
        return ops->eraser(std::move(pos), data);
    }

    size_t size() const noexcept{
        return ops->size_getter(data);
    }

    bool is_empty (){
        return (begin() == end());
    }

    ~any_container(){
        ops->deleter(data);
    }
};

template<typename T>
using my_vector = any_container<T, std::vector<T>>;

template<typename T>
using my_list = any_container<T, std::list<T>, std::bidirectional_iterator_tag>;

template<typename T>
using my_deque = any_container<T, std::deque<T>>;
