#include "compression.h"

#include <iostream>
#include <algorithm>
const int SIZE = 4096;

void Compression::count_frequencies(vector<uint8_t> const &input_block)
{
    for (size_t i = 0; i < input_block.size(); i++)
    {
        frequencies[input_block[i]]++;
    }
}

Compression::Compression()
{
    frequencies.resize(256);
    table.resize(256);
}
Compression::~Compression()
{
    if(trees.front())
    {
        trees.front()->clear();
    }
}

void Compression::make_list()
{
    while (1)
    {
        uint8_t c = 0;
        uint64_t max = 0;
        for (size_t i = 0; i < frequencies.size(); i++) //frequencies.size() <= 256. So c could be only from 0 to 255
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
        binary_tree *root = new binary_tree(nullptr, nullptr, s, max);
        trees.push_back(root);
    }
    unique = trees.size();
}

void Compression::go_through_list()
{
    while (trees.size() > 1)
    {
        binary_tree *r1 = trees.back();
        trees.pop_back();
        binary_tree *root = new binary_tree();
        root->merge(trees.back(), r1);
        trees.pop_back();
        list<binary_tree *>::iterator it = trees.end();
		
		if(trees.size() == 0)
		{
			trees.insert(trees.begin(), root);
			return;
		}
		
		
        for (it--; it != trees.begin() && (*it)->get_weight() < root->get_weight(); it--) {
        
		}
		
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
        // it++;
        // trees.insert(it, root);
    }
}

void Compression::make_table()
{
    vector<char> tmp;
    if(trees.front())
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

void Compression::print_table()
{
    for(int i = 0; i < 256; i++)
    {
        for(size_t j = 0; j < table[i].size(); j++)
        {
            cout << (int)table[i][j];
        }
        if(table[i].size() > 0)
        {
            cout << ": " << i << endl;
        }
    }
}

void Compression::print_tree(binary_tree *root)
{
    if(root != nullptr)
    {
        for(size_t i = 0; i < root->s.size(); i++)
        {
            cout << root->s[i] << " ";
        }
        cout << (int) root->get_num() << endl;
        print_tree(root->get_left());
        print_tree(root->get_right());
    }
}

uint16_t Compression::get_unique()
{
    return unique;
}

void Compression::write_tree_structure(vector<uint16_t> &output_block)
{
    if(trees.front())
    {
        write_tree_structure(trees.front(), output_block);
    }
}

void Compression::write_tree_leaves(vector<uint8_t> &output_block)
{
    if(trees.front())
    {
        write_tree_leaves(trees.front(), output_block);
    }
}

void Compression::write_tree_structure(binary_tree *root, vector<uint16_t> &output_block)
{
    if(root->get_left() != nullptr)   //у дерева Хаффмана всегда 0 или 2 ребёнка
    {
//        output.write((char*) &root->num, sizeof(root->num));
//        output.write((char*) &root->left->num, sizeof(root->left->num));
//        output.write((char*) &root->right->num, sizeof(root->right->num));
        output_block.push_back(root->get_num());
        output_block.push_back(root->get_left()->get_num());
        output_block.push_back(root->get_right()->get_num());
        write_tree_structure(root->get_left(), output_block);
        write_tree_structure(root->get_right(), output_block);
    }
}

void Compression::write_tree_leaves(binary_tree *root, vector <uint8_t> &output_block)
{
    if(root->get_left() != nullptr)
    {
        write_tree_leaves(root->get_left(), output_block);
        write_tree_leaves(root->get_right(), output_block);
    }
    else
    {
        output_block.push_back(root->get_sym());
    }
}

void Compression::compress(vector<uint8_t> const &input_block, vector<uint8_t> &output_block, uint8_t &end)
{
    if(input_block.size() == 0)
    {
        return;
    }
    int used = end;
    uint8_t cur = (output_block.size() > 0 ? output_block[output_block.size()-1] : 0), c = 0;         // c -- символ из inout_block, cur -- текущий байт на запись
    output_block.clear();
    for (size_t i = 0; i < input_block.size(); i++)
    {
        c = input_block[i];
        size_t size = table[c].size();
        int k = 0;
        if ((int) size >= 8 - used)    //добиваем остаточек, если возможно
        {
            uint8_t out_byte = cur;
            for (int i = used; i < 8; i++) {
                out_byte |= (table[c][i - used] << (7 - i));
            }
            cur = 0;
            k = 8 - used;
            size -= k;
            used = 0;
            output_block.push_back(out_byte);
        }

        while (size >= 8)    //кладём целые байты, сколько возможно
        {
            uint8_t out_byte = 0;
            for (size_t i = 0; i < 8; i++) {
                out_byte |= (table[c][i + k] << (7 - i));
            }
            k += 8;
            size -= 8;
            output_block.push_back(out_byte);
        }

        for (size_t i = 0; i < size; i++)    //кладём хвост
        {
            cur |= (table[c][i + k] << (7 - i - used));
        }
        used += size;
    }
    end = used;
    output_block.push_back(cur);
}
