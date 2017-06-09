#include "decompressor.h"
#include <iostream>
#include <stack>

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

    sym_by_num.resize(512);
    fill_syms(leaves_input);
    last_visited = 0;
    read_pos = 0;
    cur_len = 0;
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

void Decompressor::fill_syms(const vector<uint8_t> &input_block)
{
    if(unique <= 0)
    {
        return;
    }
    stack <uint16_t> st;
    st.push(0);
    size_t i = 0;
    while(!st.empty())
    {
        uint16_t cur = st.top();
        st.pop();
        if(edges[cur].size() == 2)
        {
            st.push(edges[cur][1]);
            st.push(edges[cur][0]);
        }
        else
        {
            sym_by_num[cur] = input_block[i];
            i++;
        }
    }
    if(i != input_block.size()) //значит не все листья записаны, значит дерево не связано, это плохо
    {
        throw Decoder_error();
    }
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
    write_symbols(last_visited, input_block, output_block);
    cur_len += output_block.size();
}

void Decompressor::write_symbols(uint16_t cur, const vector<uint8_t> &input_block, vector<uint8_t> &output_block)
{
    int used = 0;
    size_t max_size = (input_block.size() << 3);
//    output_block.reserve(max_size);
    uint8_t byte = input_block[0];
    read_pos = 1;
    while(cur_len + output_block.size() < length && output_block.size() < max_size)
    {
        while(edges[cur].size() != 0)
        {
            if(used == 8)
            {
                used = 0;
                if(read_pos == input_block.size())
                {
                    last_visited = cur;
                    read_pos = 0;
                    return;
                }
                byte = input_block[read_pos];
                read_pos++;
            }
            cur = edges[cur][(byte >> (7 - used)) & 0x01];
            used++;
        }
        output_block.push_back(sym_by_num[cur]);
        cur = 0;
    }
}
