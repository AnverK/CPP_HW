#ifndef MY_LIST_H
#define MY_LIST_H

#include <iterator>
#include <iostream>
using std::cout;
using std::endl;

template <typename T>
class my_list
{
private:
    friend void swap(my_list<T>& a, my_list<T>& b) noexcept
    {
        std::swap(a.tail, b.tail);
    }
    struct base_node
    {
        base_node():
            prev(nullptr),
            next(nullptr)
        {}

        base_node(base_node *p, base_node *n):
            prev(p),
            next(n)
        {}

        base_node* prev;
        base_node* next;
    };

    struct node: base_node
    {
        node() = delete;
        node(const T& a):
            base_node(nullptr, nullptr),
            data(a)
        {}
        node(base_node *p, base_node *n, const T& a):
            base_node(p, n),
            data(a)
        {}
        T data;
    };

public:

    template <typename U>
    struct my_iterator
    {
        typedef std::ptrdiff_t difference_type;
        typedef std::remove_const<U> value_type;
        typedef U* pointer;
        typedef U& reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        my_iterator();
        my_iterator(base_node* x);

        template <typename X> my_iterator(my_iterator<X> const &other,
                                          typename std::enable_if<std::is_same<const X, U>::value>::type* = nullptr):
            ptr(other.ptr)
        {
        }

        my_iterator& operator++();
        my_iterator& operator--();

        my_iterator operator++(int);
        my_iterator operator--(int);

        U& operator*() const;

        base_node* ptr;
    };
    typedef my_iterator<T> iterator;
    typedef my_iterator<const T> const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    my_list();
    ~my_list();

    my_list(my_list const&);
    my_list& operator=(my_list const&);

    bool empty() const;

    void clear();
    void push_back(T const&);
    void pop_back();

    void pop_front();
    void push_front(T const&);
    T& front();
    T const& front() const;

    T& back();
    T const& back() const;

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;

    iterator insert(const_iterator, T const&);

    iterator erase(const_iterator);
    iterator erase(const_iterator, const_iterator);

    void splice(const_iterator pos, my_list &other, const_iterator first, const_iterator last);


private:
     base_node *tail;
     void remove_one(base_node *v);
};

template<typename T>
bool operator==(my_list<T>::const_iterator &t) const
{
    return (ptr == t.ptr);
}

template<typename T>
bool operator!=(my_list<T>::const_iterator &t) const
{
    return (ptr != t.ptr);
}

template<typename T>
template<typename U>
my_list<T>::my_iterator<U>::my_iterator():
    ptr(nullptr)
{
}

template<typename T>
template<typename U>
my_list<T>::my_iterator<U>::my_iterator(base_node *x):
    ptr(x)
{

}

template<typename T>
template<typename U>
typename my_list<T>::template my_iterator<U>& my_list<T>::my_iterator<U>::operator++()
{
    ptr = ptr->next;
    return (*this);
}

template<typename T>
template<typename U>
typename my_list<T>::template my_iterator<U> my_list<T>::my_iterator<U>::operator++(int)
{
    my_iterator<U> cp(*this);
    ++(*this);
    return cp;
}


template<typename T>
template<typename U>
typename my_list<T>::template my_iterator<U>& my_list<T>::my_iterator<U>::operator--()
{
    ptr = ptr->prev;
    return (*this);
}

template<typename T>
template<typename U>
typename my_list<T>::template my_iterator<U> my_list<T>::my_iterator<U>::operator--(int)
{
    my_iterator<U> cp(*this);
    --(*this);
    return cp;
}

template<typename T>
template<typename U>
U& my_list<T>::my_iterator<U>::operator*() const
{
    return static_cast<node*>(ptr)->data;
}

template<typename T>
my_list<T>::my_list()
{
    tail = new base_node();
    tail->next = tail;
    tail->prev = tail;
}

template<typename T>
my_list<T>::my_list(my_list const& other):
    my_list()
{
    for(auto x: other)
    {
        insert(end(), x);
    }
}

template<typename T>
my_list<T>& my_list<T>::operator=(my_list const& other)
{
    my_list tmp(other);
    swap(*this, tmp);
    return *this;
}

template<typename T>
my_list<T>::~my_list()
{
    clear();
    delete tail;
}

template<typename T>
void my_list<T>::clear()
{
    erase(begin(), end());
}

template<typename T>
typename my_list<T>::template my_iterator<T> my_list<T>::insert(const_iterator pos, T const& el)
{
    if(empty())
    {
        base_node *ins = new node(el);
        ins->next = tail;
        ins->prev = tail;
        tail->next = ins;
        tail->prev = ins;
        return begin();
    }
    base_node *p = pos.ptr->prev;
    base_node *n = p->next;
    base_node *ins = new node(el);
    ins->next = n;
    ins->prev = p;
    p->next = ins;
    n->prev = ins;
    return iterator(ins);
}

template<typename T>
typename my_list<T>::template my_iterator<T> my_list<T>::erase(const_iterator first, const_iterator last)
{
    while(first != last)
    {
        first = erase(first);
    }
    return iterator(first.ptr);
}

template<typename T>
typename my_list<T>::template my_iterator<T> my_list<T>::erase(const_iterator pos)
{
    base_node *prev = pos.ptr->prev;
    remove_one(pos.ptr);
    return iterator(prev->next);
}

template<typename T>
void my_list<T>::remove_one(base_node *v)
{
//    cout << static_cast<node*>(v)->data << endl;
    if(v->next == tail && v->prev == tail)
    {
//        cout << "HERE" << endl;
        tail->next = tail;
        tail->prev = tail;
        delete (static_cast<node*>(v));
    }
    else
    {
        v->prev->next = v->next;
        v->next->prev = v->prev;
        delete (static_cast<node*>(v));
    }
}

template <typename T>
T& my_list<T>::back()
{
    return static_cast<node*>(tail->prev)->data;
}

template <typename T>
T const& my_list<T>::back() const
{
    return static_cast<node*>(tail->prev)->data;
}

template <typename T>
void my_list<T>::pop_back()
{
    erase(--end());
}

template <typename T>
void my_list<T>::push_back(T const& el)
{
    insert(end(), el);
}

template <typename T>
T& my_list<T>::front()
{
    return static_cast<node*>(tail->next)->data;
}

template <typename T>
T const& my_list<T>::front() const
{
    return static_cast<node*>(tail->next)->data;
}

template <typename T>
void my_list<T>::pop_front()
{
    erase(begin());
}

template <typename T>
void my_list<T>::push_front(T const& el)
{
    insert(begin(), el);
}

template <typename T>
void my_list<T>::splice(const_iterator pos, my_list &other, const_iterator first, const_iterator last)
{
    base_node *oth_p = first.ptr->prev;
    base_node *oth_n = last.ptr;
    base_node *p = pos.ptr->prev;
    base_node *n = pos.ptr;

    p->next = first.ptr;
    first.ptr->prev = p;
    last.ptr->prev->next = n;
    n->prev = last.ptr->prev;

    oth_p->next = oth_n;
    oth_n->prev = oth_p;

}

template<typename T>
typename my_list<T>::iterator my_list<T>::begin()
{
    return iterator(tail->next);
}

template<typename T>
typename my_list<T>::const_iterator my_list<T>::begin() const
{
    return const_iterator(tail->next);
}

template<typename T>
typename my_list<T>::iterator my_list<T>::end()
{
    return iterator(tail);
}

template<typename T>
typename my_list<T>::const_iterator my_list<T>::end() const
{
    return const_iterator(tail);
}

template<typename T>
bool my_list<T>::empty() const
{
    return (begin() == end());
}

template <typename T>
typename my_list<T>::const_reverse_iterator my_list<T>::rbegin() const {
    return const_reverse_iterator(end());
}

template <typename T>
typename my_list<T>::const_reverse_iterator my_list<T>::rend() const {
    return const_reverse_iterator(begin());
}

template <typename T>
typename my_list<T>::reverse_iterator my_list<T>::rbegin() {
    return reverse_iterator(end());
}

template <typename T>
typename my_list<T>::reverse_iterator my_list<T>::rend() {
    return reverse_iterator(begin());
}

template struct my_list<int>;

#endif // MY_LIST_H
