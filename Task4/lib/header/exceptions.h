#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

struct Decoder_error : public std::exception
{
    char const *what() const noexcept override
    {
        return "Failed to decode huffman tree";
    }
};

#endif // EXCEPTIONS_H
