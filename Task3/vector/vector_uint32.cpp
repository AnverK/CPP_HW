#include "vector_uint32.h"
#include <iostream>



uint32_t* copy_data(size_t capacity, size_t size, const uint32_t *src)
{
    uint32_t* dst = static_cast<uint32_t*>(operator new(capacity * sizeof(uint32_t)));
    memcpy(dst, src, size * sizeof(uint32_t));
    memset(dst + size, 0, (capacity - size) * sizeof(uint32_t));
    return dst;
}

vector_uint32::vector_uint32():
    size_(0), capacity_(SMALL_SIZE), my_data(data_.small_obj)
{
    memset(my_data, 0, SMALL_SIZE * sizeof(uint32_t));
}

vector_uint32::vector_uint32(size_t size):
    vector_uint32()
{
    reserve(size);
    size_ = size;
}

vector_uint32::vector_uint32(const vector_uint32 &other):
    size_(other.size())
{
    if(other.is_big())
    {
        new(&data_.big_obj) big_object(other.data_.big_obj);
        my_data = other.data_.big_obj.ptr.get();
    }
    else
    {
        for (size_t i = 0; i < SMALL_SIZE; i++)
        {
            data_.small_obj[i] = other.data_.small_obj[i];
        }
        my_data = data_.small_obj;
    }
}

vector_uint32::~vector_uint32()
{
    if(is_big())
    {
        data_.big_obj.~big_object();
    }
}

void vector_uint32::resize(size_t new_size)
{
    reserve(new_size);
    size_ = new_size;
}

void vector_uint32::reserve(size_t new_capacity)
{
    if(new_capacity <= get_capacity())
    {
        return;
    }
    set_capacity(new_capacity);
}

void vector_uint32::set_capacity(size_t new_capacity)
{
    if(new_capacity <= SMALL_SIZE && !is_big())
    {
        return;
    }
    if(is_big())
    {
        data_.big_obj.ptr.reset(copy_data(new_capacity, size_, my_data), my_deleter());
        data_.big_obj.capacity = new_capacity;
        my_data = data_.big_obj.ptr.get();
    }
    else
    {
        switch_to_big(new_capacity);
    }
}

void vector_uint32::switch_to_big(size_t new_capacity)
{
    new(&data_.big_obj) big_object(new_capacity, copy_data(new_capacity, size_, my_data));
    my_data = data_.big_obj.ptr.get();
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
//    change_ptr();
    return my_data[ind];
}

const uint32_t& vector_uint32::operator [](size_t ind) const
{
    return my_data[ind];
}

void vector_uint32::swap_big_and_small(data &big, data &small)
{
    uint32_t tmp[SMALL_SIZE] = {};
    for (size_t i = 0; i < SMALL_SIZE; i++)
    {
        tmp[i] = small.small_obj[i];
    }
    new(&small.big_obj) big_object(big.big_obj);
    big.big_obj.~big_object();
    for (size_t i = 0; i < SMALL_SIZE; i++)
    {
        big.small_obj[i] = tmp[i];
    }
}

void vector_uint32::swap(vector_uint32 &other)
{
    if (!is_big() && !other.is_big())
    {
        for (size_t i = 0; i < SMALL_SIZE; i++)
        {
            std::swap(data_.small_obj[i], other.data_.small_obj[i]);
        }
    }
    else if (is_big() && other.is_big())
    {
        std::swap(data_.big_obj, other.data_.big_obj);
        my_data = data_.big_obj.ptr.get();
        other.my_data = other.data_.big_obj.ptr.get();
    }
    else if (is_big())
    {
        swap_big_and_small(data_, other.data_);
        my_data = data_.small_obj;
        other.my_data = other.data_.big_obj.ptr.get();
    }
    else
    {
        swap_big_and_small(other.data_, data_);
        other.my_data = other.data_.small_obj;
        my_data = data_.big_obj.ptr.get();
    }
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

vector_uint32& vector_uint32::operator=(vector_uint32 const &other) {
    vector_uint32 temp(other);
    swap(temp);
    return *this;
}

void vector_uint32::pop_back()
{
//    change_ptr();
    size_--;
}

void vector_uint32::push_back(const uint32_t a)
{
    if (get_capacity() <= size_)
    {
        reserve(increase_capacity(size_));
    }
    my_data[size_] = a;
    size_++;
}

uint32_t vector_uint32::back()
{
    return my_data[size_ - 1];
}

uint32_t vector_uint32::back()  const
{
    return my_data[size_ - 1];
}

uint32_t vector_uint32::front()
{
    return my_data[0];
}


uint32_t vector_uint32::front()  const
{
    return my_data[0];
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
    return my_data;
}

uint32_t const* vector_uint32::begin() const
{
    return my_data;
}

uint32_t* vector_uint32::end()
{
    return my_data + size_;
}

uint32_t const* vector_uint32::end() const
{
    return my_data + size_;
}

void vector_uint32::clear()
{
    vector_uint32().swap(*this);
}

uint64_t vector_uint32::get_capacity()
{
    if(is_big())
    {
        return data_.big_obj.capacity;
    }
    else
    {
        return SMALL_SIZE;
    }
}

uint64_t vector_uint32::get_capacity() const
{
    if(is_big())
    {
        return data_.big_obj.capacity;
    }
    else
    {
        return SMALL_SIZE;
    }
}

vector_uint32::big_object::big_object(size_t new_capacity, uint32_t *ptr):
    capacity(new_capacity), ptr(ptr)
{

}

void vector_uint32::big_object::swap(big_object &other)
{
    std::swap(capacity, other.capacity);
    std::swap(ptr, other.ptr);
}

vector_uint32::data::data()
{

}

vector_uint32::data::~data()
{

}

bool vector_uint32::is_big()
{
    return my_data != data_.small_obj;
}

bool vector_uint32::is_big() const
{
    return my_data != data_.small_obj;
}

//void vector_uint32::change_ptr()
//{
//    if (is_big() && !data_.big_obj.ptr.unique())
//    {
//        data_.big_obj.ptr.reset(copy_data(size(), size(), my_data), my_deleter());
//        my_data = data_.big_obj.ptr.get();
//    }
//}
