#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "binary_tree.h"
#include <list>

class Compression {
    vector<uint64_t> frequencies;

    list<BinaryTree *> trees;

    vector<vector<uint8_t>> table;

    uint16_t unique;

    void write_tree_structure(BinaryTree *root, vector <uint16_t> &output_block);

    void write_tree_leaves(BinaryTree *root, vector <uint8_t> &output_block);

public:
    Compression();
    ~Compression();


    void count_frequencies(vector<uint8_t> const &input_block);

    void make_list();

    void go_through_list();

    void make_table();

    void print_table();

    uint16_t get_unique();

    void compress(vector <uint8_t> const &input_block, vector <uint8_t> &output_block, uint8_t &end);

    void write_tree_structure(vector<uint16_t> &output_block);

    void write_tree_leaves(vector <uint8_t> &output_block);

    void print_tree(BinaryTree *root);
};

#endif
