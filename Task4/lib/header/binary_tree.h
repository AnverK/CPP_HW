#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <vector>
#include <stdint.h>

using namespace std;

class BinaryTree {
private:
    BinaryTree *left, *right;
    char bit;
    vector<uint8_t> s;
    uint64_t weight;
    uint16_t num;
    friend void make_code(BinaryTree *root, vector<uint8_t> &code, vector<vector<uint8_t> > &table);

    friend void enumerate_tree(BinaryTree *root, uint16_t &num);

public:
    BinaryTree(BinaryTree *l, BinaryTree *r, vector<uint8_t> &str, uint64_t w);
    BinaryTree();
    BinaryTree(BinaryTree *l, BinaryTree *r, uint16_t n);

    void merge(BinaryTree *root1, BinaryTree *root2);

    void make_tree(vector <uint8_t> const &input_block, uint8_t &num, vector <vector <int> > const &edges, size_t &read_pos);

    uint64_t get_weight();

    BinaryTree* get_left();
    BinaryTree* get_right();
    char get_bit();
    uint64_t get_num();
    uint8_t get_sym();

    void write_tree_structure(vector<uint16_t> &output_block);

    void write_tree_leaves(vector <uint8_t> &output_block);
    void clear();

};

#endif
