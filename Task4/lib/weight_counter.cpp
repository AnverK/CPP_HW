#include "weight_counter.h"

WeightCounter::WeightCounter(vector <uint8_t> const &input_block)
{
    frequencies.resize(256);
    length = 0;
    for (size_t i = 0; i < input_block.size(); i++)
    {
        frequencies[input_block[i]]++;
    }
    length += input_block.size();
}

void WeightCounter::add_block(vector<uint8_t> input_block)
{
    for (size_t i = 0; i < input_block.size(); i++)
    {
        frequencies[input_block[i]]++;
    }
    length += input_block.size();
}

vector <uint64_t> WeightCounter::get_frequncies()
{
    return frequencies;
}

vector <uint64_t> WeightCounter::get_frequncies() const
{
    return frequencies;
}

uint64_t WeightCounter::get_length()
{
    return length;
}

uint64_t WeightCounter::get_length() const
{
    return length;
}
