#ifndef LINKED_PTR_H
#define LINKED_PTR_H

#include <utility>
template <typename T>
struct linked_ptr
{
    linked_ptr() = delete;

    linked_ptr(linked_ptr const&) noexcept;

    linked_ptr(T*) noexcept;

    linked_ptr(linked_ptr&&);

    linked_ptr& operator=(linked_ptr const&) noexcept;

    linked_ptr& operator=(linked_ptr&&) noexcept;

    T* get() const noexcept;
    ~linked_ptr() noexcept;

    void swap(linked_ptr&) noexcept;

    void reset(T*) noexcept;


    friend void swap(linked_ptr& a, linked_ptr& b) noexcept
    {
        a.swap(b);
    }

private:
    T* el_ptr;
    linked_ptr* prev;
    mutable linked_ptr* next;

    void fix(linked_ptr &) noexcept;
};

template <typename T>
linked_ptr<T>::linked_ptr(linked_ptr const& other) noexcept :
    el_ptr(other.el_ptr), prev(&const_cast<linked_ptr&>(other)), next(other.next)
{
    other.next = this;
    fix(*this);
}

template <typename T>
linked_ptr<T>::linked_ptr(T* p) noexcept : el_ptr(p), prev(nullptr), next(nullptr)
{}

template <typename T>
linked_ptr<T>::linked_ptr(linked_ptr&& other) : el_ptr(other.el_ptr), prev(other.prev), next(other.next)
{
    other.el_ptr = nullptr;
    fix(*this);
}

template <typename T>
linked_ptr<T>& linked_ptr<T>::operator=(linked_ptr const& other) noexcept
{
    if(other.el_ptr == el_ptr){
        return *this;
    }
    linked_ptr tmp(other);
    swap(tmp);
    return *this;
}

template <typename T>
linked_ptr<T>& linked_ptr<T>::operator=(linked_ptr&& other) noexcept
{
    swap(other);
    return *this;
}

template <typename T>
T* linked_ptr<T>::get() const noexcept
{
    return el_ptr;
}

template <typename T>
linked_ptr<T>::~linked_ptr() noexcept
{
    if (el_ptr)
    {
        if (prev == next)
        {
            delete el_ptr;
        }
        else
        {
            if (prev)
            {
                prev->next = next;
            }
            if (next)
            {
                next->prev = prev;
            }
        }
    }
}

template <typename T>
void linked_ptr<T>::swap(linked_ptr& other) noexcept
{
    if(other.el_ptr == el_ptr){
        return;
    }
    using std::swap;
    swap(el_ptr, other.el_ptr);
    swap(prev, other.prev);
    swap(next, other.next);
    fix(*this);
    fix(other);
}

template <typename T>
void linked_ptr<T>::reset(T* el_ptr) noexcept
{
    if(this->el_ptr == el_ptr){
        return;
    }
    if (el_ptr)
    {
        if (prev == next)
        {
            delete el_ptr;
        }
        else
        {
            if (prev)
            {
                prev->next = next;
            }
            if (next)
            {
                next->prev = prev;
            }
        }
    }
    this->el_ptr = el_ptr;
    prev = next = nullptr;
}

template <typename T>
void linked_ptr<T>::fix(linked_ptr &p) noexcept
{
    if(!p.get()){
        return;
    }
    if (p.next)
    {
        p.next->prev = &p;
    }
    if (p.prev)
    {
        p.prev->next = &p;
    }
}

#endif // LINKED_PTR_H
