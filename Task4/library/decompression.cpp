#include "decompression.h"
#include <iostream>


Decompression::Decompression()
{

}

Decompression::~Decompression()
{
    root->clear();
}

void Decompression::set_length(uint64_t l)
{
    length = l;
}

void Decompression::set_unique(uint16_t u)
{
    unique = u;
}


void Decompression::read_edges(const vector<uint16_t> &input_block) // размер этого блока должен быть посчитан как 3*2*(unique-1)

{
    read_pos = 0;
    edges.resize(512);

    for(int i = 1; i < unique; i++)
    {
        uint16_t v, l, r;

        v = input_block[read_pos];
        l = input_block[read_pos+1];
        r = input_block[read_pos+2];
        read_pos+=3;
		if(v >= 512 || l >= 512 || r >= 512)
		{
			// break;
			cout << v << " " << l << " " << r << " " << i << " " << unique << " " << read_pos << " " << input_block.size() << endl;
		}
        edges[v].push_back(l);
        edges[v].push_back(r);
    }
}

void Decompression::print_edges()
{
    for(size_t i = 0; i < edges.size(); i++)
    {
        for(size_t j = 0; j < edges[i].size(); j++)
        {
            cout << edges[i][j] << " ";
        }
        if(edges[i].size() != 0)
            cout << ": " << i << endl;
    }
}

void Decompression::make_tree(const vector<uint8_t> &input_block)
{
    uint8_t num = 0;
    read_pos = 0;
    root = new binary_tree(nullptr, nullptr, 0);
    make_tree(input_block, root, num);
    last_visited = root;
    cur_len = 0;
}

void Decompression::make_tree(const vector<uint8_t> &input_block, binary_tree *root, uint8_t &num)
{
    root->left = nullptr;
    root->right = nullptr;

    if(edges[root->num].size() != 0)
    {
        root->left = new binary_tree(nullptr, nullptr, edges[root->num][0]);
        root->right = new binary_tree(nullptr, nullptr, edges[root->num][1]);

        num++;
        make_tree(input_block, root->left, num);
        make_tree(input_block, root->right, num);
    }
    else
    {
        uint8_t c;
        c = input_block[read_pos];
        read_pos++;
        root->s.push_back(c);
    }
}

void Decompression::print_tree()
{
    print_tree(root);
}

void Decompression::print_tree(binary_tree *root)
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

void Decompression::decompress(const vector<uint8_t> &input_block, vector<uint8_t> &output_block)
{
    uint8_t byte = input_block[0];
    read_pos = 1;
    int used = 0;
    uint16_t cur_sym;
    while (cur_len < length)
    {
        cur_sym = find_symbol(last_visited, used, byte, input_block);
        if(cur_sym == 256)
        {
            return;
        }
        output_block.push_back((uint8_t)cur_sym);
        cur_len++;
        last_visited = root;
    }

}

uint16_t Decompression::find_symbol(binary_tree *root, int &used, uint8_t &byte, const vector<uint8_t> &input_block)
{
    if(root->get_left() == nullptr)
    {
        return root->get_sym();
    }
    if(used == 8)
    {
        used = 0;
        if(read_pos == input_block.size())
        {
            last_visited = root;
            read_pos = 0;
            return 256;
        }
        byte = input_block[read_pos];
        read_pos++;
    }

//    used++;
//    return ( ((byte >> (8-used)) & 0x01) == 1 ? find_symbol(root->get_right(), used, byte, input_block) : find_symbol(root->get_left(), used, byte, input_block));
    if( (byte >> (7 - used)) & 0x01)
    {
        used++;
                 return find_symbol(root->get_right(), used, byte, input_block);

    }
    else
    {
        used++;
        return find_symbol(root->get_left(), used, byte, input_block);
    }
}
