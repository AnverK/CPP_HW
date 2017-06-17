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

    void change_ptr();

private:
    const static int SMALL_SIZE = 4;

    size_t size_;
    size_t capacity_;

    struct big_object
    {
        big_object(size_t new_capacity, uint32_t* ptr);
        size_t capacity;
        std::shared_ptr <uint32_t> ptr;
        void swap(big_object &other);
    };

    union data
    {
        uint32_t small_obj[SMALL_SIZE];
        big_object big_obj;
        data();
        ~data();
    } data_;

    struct my_deleter
    {
        void operator()(uint32_t* ptr)
        {
            operator delete(ptr);
        }
    };

    friend uint32_t* copy_data(size_t capacity, size_t size, const uint32_t *src);

    uint32_t* my_data;  //если объект маленький, то на small_obj, если большой, то ptr.get()

    void swap(vector_uint32 &other);
    void swap_big_and_small(data &big, data &small);
    void set_capacity(size_t new_capacity);
    size_t increase_capacity(size_t new_size) const;
    uint64_t get_capacity();
    uint64_t get_capacity() const;
    void switch_to_big(size_t new_capacity);
    bool is_big();
    bool is_big() const;
};

#endif // VECTOR_UINT32_H
