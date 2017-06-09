#include "code_table.h"

CodeTable::CodeTable(const WeightCounter &w)
{
    vector <uint64_t> frequencies = w.get_frequncies();
    while (1)
    {
        uint8_t c = 0;
        uint64_t max = 0;
        for (size_t i = 0; i < frequencies.size(); i++) //frequencies.size() <= 256. Поэтому c в отрезке от 0 до 255
        {
            if (max < frequencies[i])
            {
                c = (uint8_t) i;
                max = frequencies[i];
            }
        }
        if (max == 0)
        {
            break;
        }
        frequencies[c] = 0;
        vector<uint8_t> s;
        s.push_back(c);
        BinaryTree *root = new BinaryTree(nullptr, nullptr, s, max);
        trees.push_back(root);
    }
    unique = trees.size();
    length = w.get_length();
    go_through_list();
    table.resize(256);
    make_table();
}

void CodeTable::go_through_list()
{
    while (trees.size() > 1)
    {
        BinaryTree *r1 = trees.back();
        trees.pop_back();
        BinaryTree *root = new BinaryTree();
        root->merge(trees.back(), r1);
        trees.pop_back();
        list<BinaryTree *>::iterator it = trees.end();

        if(trees.size() == 0)
        {
            trees.insert(trees.begin(), root);
            return;
        }

        for (it--; it != trees.begin() && (*it)->get_weight() < root->get_weight(); it--) {}

        if(it == trees.begin())
        {
            if((*it)->get_weight() < root->get_weight())
            {
                trees.insert(trees.begin(), root);
            }
            else
            {
                it++;
                trees.insert(it, root);
            }
        }
        else
        {
            trees.insert(it, root);
        }
    }
}
#include <iostream>
void CodeTable::make_table()
{
    vector<uint8_t> tmp;
    if(!trees.empty() && trees.front())
    {
        make_code(trees.front(), tmp, table);
        uint16_t num = 0;
        enumerate_tree(trees.front(), num);
        if(num == 1)
        {
            table[trees.front()->get_sym()].push_back(0);
        }
    }
}


uint64_t CodeTable::get_length()
{
    return length;
}

uint64_t CodeTable::get_length() const
{
    return length;
}


uint16_t CodeTable::get_unique()
{
    return unique;
}


uint16_t CodeTable::get_unique() const
{
    return unique;
}

BinaryTree* CodeTable::get_root()
{
    if(trees.empty())
    {
        return nullptr;
    }
    return trees.front();
}

BinaryTree* CodeTable::get_root() const
{
    if(trees.empty())
    {
        return nullptr;
    }
    return trees.front();
}

vector <vector <uint8_t> > CodeTable::get_table()
{
    return table;
}

vector <vector <uint8_t> >  CodeTable::get_table() const
{
    return table;
}
