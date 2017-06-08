#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H
#include <vector>
#include <stdint.h>
#include <queue>
#include "binary_tree.h"
#include "exceptions.h"
using namespace  std;

class Decompressor
{
private:
    vector < vector <int> > edges;
    uint64_t length;
    uint16_t unique;
    uint64_t read_pos;
    uint64_t cur_len;
    BinaryTree *root;
    BinaryTree *last_visited;
    void check_tree();
    void make_tree(vector<uint8_t> const &input_block);
    uint16_t find_symbol(BinaryTree *root, int &used, uint8_t &byte, const vector<uint8_t> &input_block);

public:
    Decompressor(uint64_t length, uint16_t unique, vector <uint16_t> const& tree_input, vector <uint8_t> const& leaves_input);
    uint64_t static count_tree_input_size_by_unique(uint16_t u);
    uint16_t static count_leaves_input_size_by_unique(uint16_t u);
    void decompress_block(const vector<uint8_t> &input_block, vector<uint8_t> &output_block);
};

#endif // DECOMPRESSOR_H
