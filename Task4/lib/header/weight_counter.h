#ifndef WEIGHT_COUNTER_H
#define WEIGHT_COUNTER_H

#include <stdint.h>
#include <vector>

using namespace std;

class WeightCounter
{
private:
    uint64_t length;
    vector <uint64_t> frequencies;
public:
    WeightCounter(vector <uint8_t> const &input_block); // нам дали блок, мы создали класс, в котором есть инфа о частотах.
                                            // а если много блоков хотят дать?
    void add_block(vector <uint8_t> input_block);      // вот для множества блоков. Не захотят юзать -- не очень-то и хотелось

    vector <uint64_t> get_frequncies();

    vector <uint64_t> get_frequncies() const;

    uint64_t get_length();

    uint64_t get_length() const;
};

#endif // WEIGHT_COUNTER_H
