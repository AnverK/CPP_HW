#ifndef SHARED_PTR_H
#define SHARED_PTR_H

#include <cstddef>
#include <utility>
template <typename T>
class shared_ptr{
public:
    shared_ptr() = delete;

    shared_ptr(shared_ptr<T> const&) noexcept;
    shared_ptr(shared_ptr<T> &&) noexcept;

    explicit shared_ptr(T*);

    shared_ptr& operator=(shared_ptr const&) noexcept;

    shared_ptr& operator=(shared_ptr&&) noexcept;

    T* get() const noexcept;

    ~shared_ptr() noexcept;

    void swap(shared_ptr&) noexcept;

    void reset(T*) noexcept;

    friend void swap(shared_ptr& a, shared_ptr& b) noexcept{
        a.swap(b);
    }

private:
    size_t* cnt_ptr;
    T* el_ptr;
};

template <typename T>
shared_ptr<T>::shared_ptr(shared_ptr<T> const& other) noexcept:
    cnt_ptr(other.cnt_ptr), el_ptr(other.el_ptr)
{
    if(el_ptr){
        (*cnt_ptr)++;
    }
}

template <typename T>
shared_ptr<T>::shared_ptr(shared_ptr<T> && other) noexcept:
    cnt_ptr(other.cnt_ptr), el_ptr(other.el_ptr)
{
    other.el_ptr = nullptr;
}

template <typename T>
shared_ptr<T>::shared_ptr(T* ptr):
    cnt_ptr(nullptr), el_ptr(ptr)
{
    if(ptr != nullptr){
        cnt_ptr = new size_t(1);
    }
}

template <typename T>
void shared_ptr<T>::swap(shared_ptr &other) noexcept{
    std::swap(cnt_ptr, other.cnt_ptr);
    std::swap(el_ptr, other.el_ptr);
}

template <typename T>
shared_ptr<T>& shared_ptr<T>::operator=(shared_ptr const& other) noexcept{
    shared_ptr tmp(other);
    swap(tmp);
    return *this;
}

template <typename T>
shared_ptr<T>& shared_ptr<T>::operator=(shared_ptr&& other) noexcept{
    swap(other);
    return *this;
}

template <typename T>
T* shared_ptr<T>::get() const noexcept{
    return el_ptr;
}

template <typename T>
void shared_ptr<T>::reset(T* ptr) noexcept{
    if(el_ptr == ptr){
        return;
    }
    if(el_ptr){
        if(*cnt_ptr == 1){
            delete cnt_ptr;
            delete el_ptr;
        }
        else{
            (*cnt_ptr)--;
        }
    }
    el_ptr = ptr;
    cnt_ptr = new size_t(1);
}

template <typename T>
shared_ptr<T>::~shared_ptr() noexcept{
    if(el_ptr){
        if(*cnt_ptr == 1){
            delete cnt_ptr;
            delete el_ptr;
        }
        else{
            (*cnt_ptr)--;
        }
    }
}


#endif // SHARED_PTR_H

