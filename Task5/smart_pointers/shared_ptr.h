#ifndef SHARED_PTR_H
#define SHARED_PTR_H

#include <cstddef>
#include <utility>
template <typename T>
class shared_ptr{
public:
    shared_ptr() = delete;

    shared_ptr(const shared_ptr<T> &other) noexcept;

    shared_ptr(T*);

    shared_ptr& operator=(shared_ptr const& other) noexcept;

    T* get() const noexcept;

    ~shared_ptr() noexcept;

    void swap(shared_ptr const& other) noexcept;

    friend void swap(shared_ptr const& a, shared_ptr const& b) noexcept{
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
shared_ptr<T>::shared_ptr(T* ptr):
    el_ptr(ptr)
{
    if(ptr != nullptr){
        cnt_ptr = new size_t(1);
    }
}

template <typename T>
void shared_ptr<T>::swap(const shared_ptr &other) noexcept{
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

