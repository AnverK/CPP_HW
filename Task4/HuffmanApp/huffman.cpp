#include "huffman.h"
#include <iostream>
#include <stdio.h>
#include <ctime>

using namespace std;

Huffman::Huffman()
{

}

void Huffman::init()
{
    length = 0;
    unique = 0;
}

void Huffman::kill()
{
    input.close();
    output.close();

    trees.clear();
    frequencies.clear();
    table.clear();
    edges.clear();
}

void Huffman::compress(const string &input_file_name, const string &output_file_name)
{
    init();
    input.open(input_file_name, ios::binary);
    count_frequencies();
    make_list();
    unique = trees.size();
    go_through_list();
    table.resize(256);
    vector<char> tmp;
    make_code(trees.front(), tmp);

    uint16_t num = 0;
    enumerate_tree(trees.front(), num);

    output.open(output_file_name, ios::binary|ios::trunc);
    write_compressed();

    delete_tree(trees.front());
    kill();
}

void Huffman::decompress(const string &input_file_name, const string &output_file_name)
{
    init();
    length = 0;
    unique = 0;
    input.open(input_file_name, ios_base::binary);
    input.read((char*) &length, sizeof(length));
    input.read((char*) &unique, sizeof(unique));
    binary_tree *root = new binary_tree;
    read_edges();

    uint8_t num = 0;
    root->num = 0;
    make_tree(root, num);
    output.open(output_file_name, ios::binary);
    write_decompressed(root);

    delete_tree(root);
    kill();
}

void Huffman::count_frequencies()
{
    frequencies.resize(256);
    char *buffer_in = (char*)malloc(4096);
    input.seekg(0, ios::end);
    length = input.tellg();
    input.seekg(0, ios::beg);
    uint64_t was_read = 0;
    uint8_t c;
    while(was_read < length)
    {
        uint64_t block_size = min((uint64_t)4096, length-was_read)*sizeof(char);
        was_read += block_size;
        input.read(buffer_in, block_size);
        for(size_t i = 0; i < block_size; i++)
        {
            c = buffer_in[i];
            frequencies[c]++;
        }
    }
    free(buffer_in);
}

void Huffman::make_list()
{
    while(1)
    {
        uint8_t c;
        uint64_t max = 0;
        for(size_t i = 0; i < frequencies.size(); i++)
        {
            if(max < frequencies[i])
            {
                c = i;
                max = frequencies[i];
            }
        }
        if(max == 0)
        {
            break;
        }
        frequencies[c] = 0;
        binary_tree *root = new binary_tree;
        root->left = nullptr;
        root->right = nullptr;
        root->weight = max;
        root->s.push_back(c);
        trees.push_back(root);
    }
}

void Huffman::go_through_list()
{
    while(trees.size() > 1)
    {
        binary_tree *r1 = trees.back();
        trees.pop_back();
        binary_tree *root;
        root = root->merge(trees.back(), r1);
        trees.pop_back();
        list <binary_tree*>::iterator it = trees.end();

        for(it--; it != trees.end() && (*it)->weight < root->weight; it--)
        {
        }
        it++;
        trees.insert(it, root);
    }
}

void Huffman::print_tree(binary_tree *root)
{
    if(root != nullptr)
    {
        for(size_t i = 0; i < root->s.size(); i++)
        {
            cout << root->s[i] << " ";
        }
        cout << (int) root->num << endl;
        print_tree(root->left);
        print_tree(root->right);
    }
}

void Huffman::print_table()
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

void Huffman::make_code(binary_tree* root, vector <char> &code)
{
    if(root->left != nullptr)
    {
        code.push_back(0);
        make_code(root->left, code);
        code.pop_back();
        code.push_back(1);
        make_code(root->right, code);
        code.pop_back();
    }
    else
    {
        table[root->s[0]] = code;
        return;
    }
}

void Huffman::enumerate_tree(binary_tree *root, uint16_t &num)
{
    if(root != nullptr)
    {
        root->num = num;
        num++;
        enumerate_tree(root->left, num);
        enumerate_tree(root->right, num);
    }
}

void Huffman::write_compressed()
{
    output.write((char*) &length, sizeof(length));
    output.write((char*) &unique, sizeof(unique));
    write_tree_structure(trees.front());
    write_tree_leaves(trees.front());
    input.clear();
    input.seekg(0);
    uint8_t c;
    int used  = 0; //сколько из 8 бит текущего байта занято
    char cur = 0;

    uint64_t was_read = 0;

    char *buffer_in = (char*)malloc(4096);
    char *buffer_out = (char*)malloc(4096);
    int out_pos = 0;

    while(was_read < length)
    {
        uint64_t block_size = min((uint64_t)4096, length-was_read)*sizeof(char);
        was_read += block_size;
        input.read(buffer_in, block_size);
        for(size_t i = 0; i < block_size; i++)
        {
            c = buffer_in[i];
            size_t size = table[c].size();

            int k = 0;

            if((int) size >= 8 - used)    //добиваем остаточек, если возможно
            {
                char out_byte = cur;
                for(size_t i = used; i < 8; i++)
                {
                    out_byte |= (table[c][i-used] << (7-i));
                }
                cur = 0;
                size -= (8-used);
                k = 8 - used;
                used = 0;
                buffer_out[out_pos] = out_byte;
                out_pos++;
                if(out_pos == 4096)
                {
                    output.write(buffer_out, 4096);
                    out_pos = 0;
                }
            }

            while(size >= 8)    //кладём целые байты, сколько возможно
            {
                char out_byte = 0;
                for(size_t i = 0; i < 8; i++)
                {
                    out_byte |= (table[c][i + k] << (7-i));
                }
                k += 8;
                size -= 8;
                buffer_out[out_pos] = out_byte;
                out_pos++;
                if(out_pos == 4096)
                {
                    output.write(buffer_out, 4096);
                    out_pos = 0;
                }
            }

            for(size_t i = 0; i < size; i++)    //кладём хвост
            {
                cur |= (table[c][i + k] << (7 - i - used));
            }
            used += size;
        }
    }
    output.write(buffer_out, out_pos*sizeof(char));
    output.write((char*) &cur, sizeof(cur));
    free(buffer_in);
    free(buffer_out);
}

void Huffman::write_tree_structure(binary_tree *root)
{
    if(root->left != nullptr)   //у дерева Хаффмана всегда 0 или 2 ребёнка
    {
        output.write((char*) &root->num, sizeof(root->num));
        output.write((char*) &root->left->num, sizeof(root->left->num));
        output.write((char*) &root->right->num, sizeof(root->right->num));
        write_tree_structure(root->left);
        write_tree_structure(root->right);
    }
}

void Huffman::write_tree_leaves(binary_tree *root)
{
    if(root->left != nullptr)
    {
        write_tree_leaves(root->left);
        write_tree_leaves(root->right);
    }
    else
    {
        output.write((char*) &root->s[0], sizeof(root->s[0]));
    }
}

void Huffman::delete_tree(binary_tree *root)
{
    if(root->left != nullptr)
    {
        delete_tree(root->left);
        delete_tree(root->right);
    }
    delete root;
}

void Huffman::read_edges()
{
    edges.resize(512);
    for(int i = 1; i < unique; i++)
    {
        uint16_t v, l, r;
        input.read((char *) &v, sizeof(v));
        input.read((char *) &l, sizeof(l));
        input.read((char *) &r, sizeof(r));
        edges[v].push_back(l);
        edges[v].push_back(r);
    }
}



void Huffman::make_tree(binary_tree *root, uint8_t &num)
{
    root->left = nullptr;
    root->right = nullptr;

    if(edges[root->num].size() != 0)
    {
        root->left = new binary_tree;
        root->left->num = edges[root->num][0];
        root->left->left = nullptr;
        root->left->right = nullptr;

        root->right = new binary_tree;
        root->right->num = edges[root->num][1];
        root->right->left = nullptr;
        root->right->right = nullptr;

        num++;
        make_tree(root->left, num);
        make_tree(root->right, num);
    }
    else
    {
        uint8_t c;
        input.read((char*) &c, sizeof(c));
        root->s.push_back(c);
    }
}

void Huffman::write_decompressed(binary_tree *root)
{
    uint8_t c;
    int used = 0; //сколько из 8 бит текущего байта прочитано
    uint64_t len = 0;

    char *buffer_in = (char*)malloc(4096);
    char *buffer_out = (char*)malloc(4096);
    int out_pos = 0, in_pos = 0;
    input.read(buffer_in, 4096);
    c = buffer_in[in_pos];
    in_pos++;
    while(len < length)
    {
        buffer_out[out_pos] = find_symbol(root, used, c, in_pos, buffer_in);
        out_pos++;
        len++;
        if(out_pos == 4096)
        {
            output.write(buffer_out, 4096);
            out_pos = 0;
        }
    }
    output.write(buffer_out, out_pos*sizeof(char));
    free(buffer_in);
    free(buffer_out);
}

uint8_t Huffman::find_symbol(binary_tree *root, int &used, uint8_t &byte, int &in_pos, char *buffer)
{
    if(root->left == nullptr)
    {
        return root->s[0];
    }
    if(used == 8)
    {
        used = 0;
        byte = buffer[in_pos];
        in_pos++;
        if(in_pos == 4096)
        {
            input.read(buffer, 4096);
            in_pos = 0;
        }
    }
    if( (byte >> (7 - used)) & 0x01)
    {
        used++;
        return find_symbol(root->right, used, byte, in_pos, buffer);
    }
    else
    {
        used++;
        return find_symbol(root->left, used, byte, in_pos, buffer);
    }
}
