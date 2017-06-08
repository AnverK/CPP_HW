#include "decompressor.h"
Decompressor::Decompressor(uint64_t l, uint16_t u, vector <uint16_t> const& tree_input, vector <uint8_t> const& leaves_input)
{
    if(tree_input.size() != count_tree_input_size_by_unique(u) ||
            leaves_input.size() != count_leaves_input_size_by_unique(u))
    {
        throw Decoder_error();
    }

    length = l;
    unique = u;
    edges.resize(512);

    for(read_pos = 0; read_pos < tree_input.size(); read_pos += 3)
    {
        uint16_t v, l, r;

        v = tree_input[read_pos];
        l = tree_input[read_pos+1];
        r = tree_input[read_pos+2];
        if(v >= 512 || l >= 512 || r >= 512)
        {
            throw Decoder_error();
        }
        edges[v].push_back(l);
        edges[v].push_back(r);
        if(edges[v].size() != 2)
        {
            throw Decoder_error();
        }
    }
    if(unique > 0)
    {
        check_tree();
    }

    make_tree(leaves_input);
    read_pos = 0;
    cur_len = 0;
    last_visited = root;

}


void Decompressor::check_tree()
{
    if(unique <= 0)
    {
        return;
    }
    vector <char> visited(unique*2 - 1);//столько вершин всего
    queue <int> q;
    q.push(0);
    while(!q.empty())
    {
        int cur = q.front();
        q.pop();
        if(visited[cur] == 1)
        {
            throw Decoder_error();
        }
        visited[cur] = 1;
        if(edges[cur].size() == 2)
        {
            q.push(edges[cur][0]);
            q.push(edges[cur][1]);
        }
    }
}

void Decompressor::make_tree(const vector<uint8_t> &input_block)
{
    uint8_t num = 0;
    read_pos = 0;
    root = new BinaryTree(nullptr, nullptr, 0);
    if(input_block.size() == 0)
    {
        return;
    }
    root->make_tree(input_block, num, edges, read_pos);
}


uint16_t Decompressor::count_leaves_input_size_by_unique(uint16_t u)
{
    return u;
}

uint64_t Decompressor::count_tree_input_size_by_unique(uint16_t u)
{
    if(u == 0)
    {
        return 0;
    }
    return (uint64_t)(3*(u-1));
}

void Decompressor::decompress_block(const vector<uint8_t> &input_block, vector<uint8_t> &output_block)
{
    if(input_block.size() == 0)
    {
        return;
    }
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

uint16_t Decompressor::find_symbol(BinaryTree *root, int &used, uint8_t &byte, const vector<uint8_t> &input_block)
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
