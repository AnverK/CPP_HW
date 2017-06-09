#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H
#include <vector>
#include <stdint.h>
#include <queue>
#include "exceptions.h"
using namespace  std;

class Decompressor
{
private:
    const static int SZ = 512;
    const static int ALP = 2;
    int edges[SZ][ALP];
    vector <uint8_t> sym_by_num;
    uint64_t length;
    uint16_t unique;
    uint64_t read_pos;
    uint64_t cur_len;
    uint16_t last_visited;
    void check_tree();
    void fill_syms(vector<uint8_t> const &input_block);
    void write_symbols(uint16_t cur, const vector<uint8_t> &input_block, vector<uint8_t> &output_block);

public:
    Decompressor(uint64_t length, uint16_t unique, vector <uint16_t> const& tree_input, vector <uint8_t> const& leaves_input);
    uint64_t static count_tree_input_size_by_unique(uint16_t u);
    uint16_t static count_leaves_input_size_by_unique(uint16_t u);
    void decompress_block(const vector<uint8_t> &input_block, vector<uint8_t> &output_block);
};

#endif // DECOMPRESSOR_H
