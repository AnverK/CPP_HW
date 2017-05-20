#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <fstream>
#include <vector>
#include <list>
#include <queue>
#include <iostream>
#include <string.h>

using namespace std;

class Huffman
{
    struct binary_tree
    {
        binary_tree *left, *right;
        char bit;
        vector <uint8_t> s;
        uint64_t weight;
        uint16_t num;

        binary_tree *merge(binary_tree *root1, binary_tree *root2)
        {
            binary_tree *root = new binary_tree;
            root1->bit = 0;
            root2->bit = 1;
            root->left = root1;
            root->right = root2;
            root->s = root1->s;
            root->s.insert(root->s.end(), root2->s.begin(), root2->s.end());
            root->weight = root1->weight + root2->weight;
            return root;
        }
    };

public:
    Huffman();

    void compress(const string &input_file_name, const string &output_file_name);

    void decompress(const string &input_file_name, const string &output_file_name);

private:
    void kill();
    void init();
    void delete_tree(binary_tree *root);


    void print_table();
    void print_tree(binary_tree *root);


    void count_frequencies();
    void make_list();
    void go_through_list();
    void make_code(binary_tree *root, vector<char> &code);
    void enumerate_tree(binary_tree *root, uint16_t &num);
    void write_compressed();
    void write_tree_structure(binary_tree *root);
    void write_tree_leaves(binary_tree *root);

    void read_edges();
    void make_tree(binary_tree *root, uint8_t &num);
    void make_table(binary_tree *root, vector<bool> &code);
    void write_decompressed(binary_tree *root);

    uint8_t find_symbol(binary_tree *root, int &used, uint8_t &byte, int &in_pos, char *buffer);

    ifstream input;
    ofstream output;

    vector <uint64_t> frequencies;

    list <binary_tree*> trees;

    vector<vector<char>> table;

    vector < vector <int> > edges;

    uint64_t length;
    uint16_t unique;

};

#endif // HUFFMAN_H
