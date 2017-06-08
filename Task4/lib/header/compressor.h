#ifndef COMPRESSOR_H
#define COMPRESSOR_H
#include "code_table.h"

class Compressor
{
private:
    vector <vector <uint8_t> > table;
    BinaryTree *root;

    void write_tree_structure(BinaryTree* root, vector<uint16_t> &output_block);
    void write_tree_leaves(BinaryTree* root, vector <uint8_t> &output_block);

public:
    Compressor(const CodeTable &ct, uint64_t &length, uint16_t &unique,
               vector <uint16_t> &tree_output, vector <uint8_t> &leaves_output);
    ~Compressor();
    //    подаются два любых вектора и два любых числа нужного размера.
    //    Верхняя граница на суммарный размер output: 14 кбайт. Поэтому O(1) памяти под них.
    void compress_block(vector <uint8_t> const& input_block, vector<uint8_t> &output_block, uint8_t &end);
};

#endif // COMPRESSOR_H
