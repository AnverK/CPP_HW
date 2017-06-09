#include "vector_uint32.h"
#include <iostream>
vector_uint32::vector_uint32():
    size_(0), capacity_(SMALL_SIZE)
{
    data_ = static_cast<uint32_t*>(operator new(SMALL_SIZE * sizeof(uint32_t)));
    memset(data_, 0, SMALL_SIZE * sizeof(uint32_t));
}

vector_uint32::vector_uint32(size_t size):
    vector_uint32()
{
    resize(size);
}

vector_uint32::vector_uint32(const vector_uint32 &other):
    size_(other.size()), capacity_(std::max(other.size(), (size_t)SMALL_SIZE))
{
    data_ = static_cast<uint32_t*>(operator new(capacity_ * sizeof(uint32_t)));
    memcpy(data_, other.data_, capacity_ * sizeof(uint32_t));
}

vector_uint32::~vector_uint32()
{
//    if(data_ != nullptr)
//    clear();
    operator delete(data_);
}

void vector_uint32::resize(size_t new_size)
{
    if(new_size > capacity_)
    {
        set_capacity(new_size);
        uint32_t* new_data = static_cast<uint32_t*>(operator new(capacity_ * sizeof(uint32_t)));
        for(size_t i = 0; i < size_; ++i)
        {
            new (new_data + i) uint32_t(data_[i]);
        }
        for(size_t i = size_; i < new_size; i++)
        {
            new (new_data + i) uint32_t(0);
        }
        operator delete(data_);
        data_ = new_data;
    }
    else if(new_size > size_)
    {
        for(size_t i = size_; i < new_size; i++)
        {
            data_[i] = 0;
        }
    }
    size_ = new_size;
}

void vector_uint32::reserve(size_t new_capacity)
{
    if(new_capacity <= capacity_)
    {
        return;
    }
    set_capacity(new_capacity);
    uint32_t* new_data = static_cast<uint32_t*>(operator new(capacity_ * sizeof(uint32_t)));
    for(size_t i = 0; i < size_; ++i)
    {
        new (new_data + i) uint32_t(data_[i]);
    }
    operator delete(data_);
    data_ = new_data;
}

void vector_uint32::set_capacity(size_t new_capacity)
{
    if(new_capacity <= SMALL_SIZE)
    {
        return;
    }
    capacity_ = new_capacity;
}

size_t vector_uint32::size()
{
    return size_;
}

size_t vector_uint32::size() const
{
    return size_;
}

uint32_t& vector_uint32::operator[](size_t ind)
{
    return data_[ind];
}

const uint32_t& vector_uint32::operator [](size_t ind) const
{
    return data_[ind];
}

void vector_uint32::swap(vector_uint32 &other)
{
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(data_, other.data_);
}

vector_uint32& vector_uint32::operator=(vector_uint32 const &other) {
    vector_uint32 temp(other);
    swap(temp);
    return *this;
}

void vector_uint32::pop_back()
{
    size_--;
}

void vector_uint32::push_back(const uint32_t a)
{
    if (capacity_ <= size_)
    {
        reserve(increase_capacity(std::max(capacity_, size_)));
    }
    data_[size_] = a;
    size_++;
}

uint32_t vector_uint32::back()
{
    return data_[size_ - 1];
}

uint32_t vector_uint32::back()  const
{
    return data_[size_ - 1];
}

uint32_t vector_uint32::front()
{
    return data_[0];
}


uint32_t vector_uint32::front()  const
{
    return data_[0];
}


size_t vector_uint32::increase_capacity(size_t new_size) const
{
    if(new_size == 0)
    {
        return SMALL_SIZE;
    }
    return new_size * 3 / 2;
}

uint32_t* vector_uint32::begin()
{
    return data_;
}

uint32_t const* vector_uint32::begin() const
{
    return data_;
}

uint32_t* vector_uint32::end()
{
    return data_ + size_;
}

uint32_t const* vector_uint32::end() const
{
    return data_ + size_;
}

void vector_uint32::clear()
{
    vector_uint32().swap(*this);
}


