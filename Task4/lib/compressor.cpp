#include "compressor.h"

Compressor::Compressor(WeightCounter const &wc, uint64_t &length, uint16_t &unique,
                       vector <uint16_t> &tree_output, vector <uint8_t> &leaves_output)
{
    CodeTable ct(wc);
    table = ct.get_table();
    root = ct.get_root();
    length = ct.get_length();
    unique = ct.get_unique();
    if(root != nullptr)
    {
        write_tree_structure(root, tree_output);
        write_tree_leaves(root, leaves_output);
    }
}



void Compressor::write_tree_structure(BinaryTree* root, vector<uint16_t> &output_block)
{
    if(root->get_left() != nullptr)
    {
        output_block.push_back(root->get_num());
        output_block.push_back(root->get_left()->get_num());
        output_block.push_back(root->get_right()->get_num());
        write_tree_structure(root->get_left(), output_block);
        write_tree_structure(root->get_right(), output_block);
    }
}

void Compressor::write_tree_leaves(BinaryTree* root, vector<uint8_t> &output_block)
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

void Compressor::compress_block(vector <uint8_t> const &input_block, vector <uint8_t> &output_block, uint8_t &end)
{
    if(input_block.size() == 0)
    {
        return;
    }
    size_t used = end;
    uint8_t cur = (output_block.size() > 0 ? output_block[output_block.size()-1] : 0), c = 0;         // c -- символ из inout_block, cur -- текущий байт на запись
    output_block.clear();
    for (size_t i = 0; i < input_block.size(); i++)
    {
        c = input_block[i];
        size_t size = table[c].size();
        int k = 0;
        if (size >= 8 - used)    //добиваем остаточек, если возможно
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

Compressor::~Compressor()
{
    if(root)
    {
        root->clear();
    }
}
