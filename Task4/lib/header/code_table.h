#ifndef CODE_TABLE_H
#define CODE_TABLE_H
#include "binary_tree.h"
#include <list>
#include "weight_counter.h"

class CodeTable
{
private:

    uint16_t unique;
    uint64_t length;
    list<BinaryTree *> trees;
    vector<vector<uint8_t>> table;

    void go_through_list();
    void make_table();

public:
    CodeTable(WeightCounter const &w);

    uint16_t get_unique();
    uint64_t get_length();
    BinaryTree* get_root();
    vector<vector<uint8_t>> get_table();

    uint16_t get_unique() const;
    uint64_t get_length() const;
    BinaryTree* get_root() const;
    vector<vector<uint8_t> > get_table() const;
};

#endif // CODE_TABLE_H
