#include "binary_tree.h"

BinaryTree::BinaryTree()
{
    left = nullptr;
    right = nullptr;
}

BinaryTree::BinaryTree(BinaryTree *l, BinaryTree *r, vector<uint8_t> &str, uint64_t w) {
    left = l;
    right = r;
    s = str;
    weight = w;
}

BinaryTree::BinaryTree(BinaryTree *l, BinaryTree *r, uint16_t n) {
    left = l;
    right = r;
    num = n;
}

void BinaryTree::merge(BinaryTree *root1, BinaryTree *root2) {
    root1->bit = 0;
    root2->bit = 1;
    vector<uint8_t> buf = root1->s;
    buf.insert(buf.end(), root2->s.begin(), root2->s.end());
    left = root1;
    right = root2;
    weight = root1->weight + root2->weight;
    s = buf;
}

void enumerate_tree(BinaryTree *root, uint16_t &num) {
    if (root != nullptr)
    {
        root->num = num;
        num++;
        enumerate_tree(root->left, num);
        enumerate_tree(root->right, num);
    }
}

void make_code(BinaryTree *root, vector<uint8_t> &code, vector<vector<uint8_t>> &table) {
    if (root->left != nullptr)
    {
        code.push_back(0);
        make_code(root->left, code, table);
        code.pop_back();
        code.push_back(1);
        make_code(root->right, code, table);
        code.pop_back();
    }
    else
    {
        table[root->s[0]] = code;
        return;
    }
}

uint64_t BinaryTree::get_weight() {
    return weight;
}

BinaryTree* BinaryTree::get_left()
{
    return left;
}

BinaryTree* BinaryTree::get_right()
{
    return right;
}

char BinaryTree::get_bit()
{
    return bit;
}

uint16_t BinaryTree::get_num()
{
    return num;
}

uint8_t BinaryTree::get_sym()
{
    return s[0];
}

void BinaryTree::clear()
{
    if(left != nullptr)
    {
        left->clear();
        right->clear();
    }
    delete this;
}
