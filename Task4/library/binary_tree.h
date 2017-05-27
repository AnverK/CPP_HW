#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <vector>
#include <stdint.h>

using namespace std;

class binary_tree {
public:
    binary_tree *left, *right;
    char bit;
    vector<uint8_t> s;
    uint64_t weight;
    uint16_t num;
    friend void make_code(binary_tree *root, vector<char> &code, vector<vector<char> > &table);

    friend void enumerate_tree(binary_tree *root, uint16_t &num);

public:
    binary_tree(binary_tree *l, binary_tree *r, vector<uint8_t> &str, uint64_t w);
    binary_tree();
    binary_tree(binary_tree *l, binary_tree *r, uint16_t n);

    void merge(binary_tree *root1, binary_tree *root2);

    uint64_t get_weight();

    binary_tree* get_left();
    binary_tree* get_right();
    char get_bit();
    uint64_t get_num();
    uint8_t get_sym();

    void write_tree_structure(vector<uint16_t> &output_block);

    void write_tree_leaves(vector <uint8_t> &output_block);
    void clear();

};

#endif
