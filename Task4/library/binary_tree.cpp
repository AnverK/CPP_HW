#include "binary_tree.h"
#include <iostream>

binary_tree::binary_tree()
{
    left = nullptr;
    right = nullptr;
}

binary_tree::binary_tree(binary_tree *l, binary_tree *r, vector<uint8_t> &str, uint64_t w) {
    left = l;
    right = r;
    s = str;
    weight = w;
}

binary_tree::binary_tree(binary_tree *l, binary_tree *r, uint16_t n) {
    left = l;
    right = r;
    num = n;
}

void binary_tree::merge(binary_tree *root1, binary_tree *root2) {
    root1->bit = 0;
    root2->bit = 1;
    vector<uint8_t> buf = root1->s;
    buf.insert(buf.end(), root2->s.begin(), root2->s.end());
    left = root1;
    right = root2;
    weight = root1->weight + root2->weight;
    s = buf;
//    binary_tree *root = new binary_tree(root1, root2, buf, root1->weight + root2->weight);
//    return root;
}

void enumerate_tree(binary_tree *root, uint16_t &num) {
    if (root != nullptr) {
        root->num = num;
        num++;
        enumerate_tree(root->left, num);
        enumerate_tree(root->right, num);
    }
}

void make_code(binary_tree *root, vector<char> &code, vector<vector<char>> &table) {
    if (root->left != nullptr) {
        code.push_back(0);
        make_code(root->left, code, table);
        code.pop_back();
        code.push_back(1);
        make_code(root->right, code, table);
        code.pop_back();
    } else {
        table[root->s[0]] = code;
        return;
    }
}

uint64_t binary_tree::get_weight() {
    return weight;
}

binary_tree* binary_tree::get_left()
{
    return left;
}

binary_tree* binary_tree::get_right()
{
    return right;
}

char binary_tree::get_bit()
{
    return bit;
}

uint64_t binary_tree::get_num()
{
    return num;
}

uint8_t binary_tree::get_sym()
{
    return s[0];
}

void binary_tree::clear()
{
    if(left != nullptr)
    {
        left->clear();
        right->clear();
    }
    delete this;
}
