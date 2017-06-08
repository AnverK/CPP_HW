#ifndef DECOMPRESSION_H
#define DECOMPRESSION_H
#include <vector>
#include "binary_tree.h"
using namespace std;

class Decompression {
private:

    vector < vector <int> > edges;
    BinaryTree *root;
    BinaryTree *last_visited;
    uint64_t length;
    uint16_t unique;
    uint64_t read_pos;
    int checked_bits;           //число битов в текущем байте, которые уже просмотрены
    uint64_t first_bit;       //позиция начала последней рассматриваемой последовательности битов во входном блоке
    uint64_t cur_len;
    void print_tree(BinaryTree *root);
    void make_tree(vector <uint8_t> const &input_block, BinaryTree *root, uint8_t &num);
    uint16_t find_symbol(BinaryTree *root, int &used, uint8_t &byte, const vector<uint8_t> &input_block);


public:
    Decompression();
    ~Decompression();

    void set_length(uint64_t l);
    void set_unique(uint16_t u);

    void read_edges(vector <uint16_t> const &input_block);
    void make_tree(vector <uint8_t> const &input_block);
    void decompress(vector <uint8_t> const &input_block, vector <uint8_t> &output_block);

    void check_tree();

    void print_tree();
    void print_edges();

};

#endif
