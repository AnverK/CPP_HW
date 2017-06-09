#ifndef VECTOR_UINT32_H
#define VECTOR_UINT32_H
#include <stdint.h>
#include <string.h>
#include <memory>

using namespace std;

class vector_uint32
{
public:
    vector_uint32();
    ~vector_uint32();
    vector_uint32(size_t size);
    vector_uint32(vector_uint32 const &other);

    size_t size();
    size_t size() const;

    void reserve(size_t new_capacity);
    void resize(size_t new_size);

    uint32_t& operator[](size_t ind);
    uint32_t const& operator[](size_t ind) const;

    vector_uint32& operator=(vector_uint32 const &other);

    void pop_back();
    void push_back(const uint32_t a);

    uint32_t back();
    uint32_t front();

    uint32_t back() const;
    uint32_t front() const;

    uint32_t* begin();
    uint32_t* end();

    uint32_t const* begin() const;
    uint32_t const* end() const;

    void clear();

private:
    const static int SMALL_SIZE = 4;

    void swap(vector_uint32 &other);

    size_t size_;
    size_t capacity_;
    uint32_t* data_;

    void set_capacity(size_t new_capacity);
    size_t increase_capacity(size_t new_size) const;

};

#endif // VECTOR_UINT32_H
