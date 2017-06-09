#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>

#include "gtest/gtest.h"
#include "exceptions.h"
#include "compressor.h"
#include "decompressor.h"

vector <vector<uint8_t>> CHECK_DECOMPRESS(size_t const length, uint16_t const unique, vector <uint8_t> const  &leaves,
                                 vector <uint16_t> const &tree, vector < vector <uint8_t> > const & data)
{
    Decompressor decomp = Decompressor(length, unique, tree, leaves);
    vector <vector <uint8_t> > res;
    for(auto v: data)
    {
        res.resize(res.size()+1);
        decomp.decompress_block(v, res.back());
    }
    return res;
}

void CHECK(vector < vector<uint8_t> > const &input)
{
    if(input.size() == 0)
    {
        return;
    }

    WeightCounter wc = WeightCounter(input[0]);
    for(size_t i = 1; i < input.size(); i++)
    {
        wc.add_block(input[i]);
    }
    CodeTable ct = CodeTable(wc);

    uint64_t length;
    uint16_t unique;
    vector <uint16_t> out16;
    vector <uint8_t> out8;
    Compressor comp = Compressor(ct, length, unique, out16, out8);

    vector < vector <uint8_t> > data;
    vector <uint8_t> buf;
    uint8_t end_bit = 0;
    for(auto v: input)
    {
        if(v.size() == 0)
        {
            continue;
        }
        comp.compress_block(v, buf, end_bit);
        uint8_t c = buf.back();
        buf.pop_back();
        data.push_back(buf);
        buf.push_back(c);
    }
    if(end_bit % 8)
    {
        data.back().push_back(buf.back());
    }

    vector < vector <uint8_t> > res = CHECK_DECOMPRESS(length, unique, out8, out16, data);

    size_t index_in = 0, index_out = 0, i = 0, j = 0, cur = 0;
    while(index_in < input.size() && index_out < res.size())
    {
        while(index_in < input.size() && i >= input[index_in].size())
        {
            i = 0;
            index_in++;
        }
        while(index_out < res.size() && j >= res[index_out].size())
        {
            j = 0;
            index_out++;
        }
        if(index_in < input.size() && index_out < res.size() && j < res[index_out].size() && i < input[index_in].size())
        {
            ASSERT_TRUE(res[index_out][j] == input[index_in][i]);
            cur++;
        }
        i++;
        j++;
    }
    ASSERT_TRUE(cur == length);
}

void generate_blocks(vector <vector <uint8_t> > &T, int max_size, int num_of_blocks)
{
    int size = max_size;
    T.resize(num_of_blocks);
    for(int i = 0; i < num_of_blocks; i++)
    {
        T[i].resize(size);
        for (int j = 0; j < size; j++)
        {
            T[i][j] = rand() % 256;
        }
        size = rand() % (max_size+1);
    }
}

TEST(correctness, one_block)
{
    int max_size = 1000;
    int num_of_blocks = 1;
    vector <vector <uint8_t> > T;
    generate_blocks(T, max_size, num_of_blocks);
    CHECK(T);
}

TEST(correctness, empty)
{
    int max_size = 0;
    int num_of_blocks = 1000;
    vector <vector <uint8_t> > T;
    generate_blocks(T, max_size, num_of_blocks);
    CHECK(T);
}

TEST(correctness, one_byte)
{
    int max_size = 1;
    int num_of_blocks = 1000;
    vector <vector <uint8_t> > T;
    generate_blocks(T, max_size, num_of_blocks);
    CHECK(T);
}

TEST(correctness, random_tests)
{
    int max_size = 1000;
    int num_of_blocks = 100;
    vector <vector <uint8_t> > T;
    generate_blocks(T, max_size, num_of_blocks);
    CHECK(T);
}


TEST(correctness, small_blocks)
{
    int max_size = 6;
    int num_of_blocks = 100000;
    vector <vector <uint8_t> > T;
    generate_blocks(T, max_size, num_of_blocks);
    CHECK(T);
}

TEST(correctness, huge_blocks)
{
    int max_size = 10000;
    int num_of_blocks = 100;
    vector <vector <uint8_t> > T;
    generate_blocks(T, max_size, num_of_blocks);
    CHECK(T);
}

TEST(correctness, only_unique_symbols)
{
    vector <vector <uint8_t> > T(1);
    T[0].resize(256);
    for(int i = 0; i < 256; i++)
    {
        T[0][i] = i;
    }
    random_shuffle(T[0].begin(), T[0].end());
    CHECK(T);
}

TEST(correctness, only_one_symbol)
{
    int max_size = 10000;
    int size = max_size;
    int num_of_blocks = 100;
    vector <vector <uint8_t> > T(num_of_blocks);
    for(int i = 0; i < num_of_blocks; i++)
    {
        T[i].resize(size);
        for(int j = 0; j < size; j++)
        {
            T[i][j] = 12;
        }
        size = rand() % max_size;
    }
    CHECK(T);
}

TEST(correctness, 2k_distribution)
{
    int size = 1;
    vector <vector <uint8_t> > T(1);
    T[0].resize(1<<20);
    int prev = 0;
    for(int i = 0; i <= 20; i++)
    {
        for(int j = prev; j < size; j++)
        {
            T[0][j] = i;
        }
        prev = size;
        size *= 2;
    }
    random_shuffle(T.begin(), T.end());
    CHECK(T);
}

TEST(correctness, too_many_childrens_in_tree)
{
    size_t len = 0;
    vector <uint8_t> leaves(0);
    vector <vector <uint8_t> > data(0);      //неважно, чему эти параметры равны, ждём эксепшна. А если нет, то и пофиг

    uint16_t unique = 10+rand()%10;
    vector <uint16_t> tree;

    uint16_t v = rand() % unique;    //одна из вершин дерева, необязательно лист
    for(int i = 0; i < 2; i++)
    {
        tree.push_back(v);
        tree.push_back(rand()%unique);
        tree.push_back(rand()%unique);
    }

    try {
        CHECK_DECOMPRESS(len, unique, leaves, tree, data);
        ASSERT_EQ(1, 2);
    }
    catch(Decoder_error e){
//        cout << e.what() << endl;
    }
}

TEST(correctness, not_existed_vertex_in_tree)
{
    size_t len = 0;
    vector <uint8_t> leaves(0);
    vector <vector <uint8_t> > data(0);      //неважно, чему эти параметры равны, ждём эксепшна. А если нет, то и пофиг

    uint16_t unique = 50+rand()%100;

    vector <uint16_t> tree;

    for(int i = 0; i < 100; i++)
    {
        tree.push_back(rand()%1000+512);
        tree.push_back(rand()%1000+512);
        tree.push_back(rand()%1000+512);
    }

    try {
        CHECK_DECOMPRESS(len, unique, leaves, tree, data);
        ASSERT_EQ(1, 2);
    }
    catch(Decoder_error e){
    }
}

TEST(correctness, cyclic_tree)
{
    size_t len = 0;
    vector <uint8_t> leaves(0);
    vector <vector <uint8_t> > data(0);      //неважно, чему эти параметры равны, ждём эксепшна. А если нет, то и пофиг

    uint16_t unique = 3;

    vector <uint16_t> tree{0, 1, 2, 1, 0, 2};

    try {
        CHECK_DECOMPRESS(len, unique, leaves, tree, data);
        ASSERT_EQ(1, 2);
    }
    catch(Decoder_error e){
    }
}

TEST(correctness, disconnected_tree)
{
    size_t len = 0;
    vector <vector <uint8_t> > data(0);      //неважно, чему эти параметры равны, ждём эксепшна. А если нет, то и пофиг

    uint16_t unique = 3;

    vector <uint8_t> leaves{0, 1, 2};       // размер совпадает с unique

    vector <uint16_t> tree{0, 1, 2, 3, 4, 5};   //размер совпадает с допустимым: если 3 уникальных, то есть 5 вершин.
    // Но тут они образуют несвязные два дерева

    try {
        CHECK_DECOMPRESS(len, unique, leaves, tree, data);
        ASSERT_EQ(1, 2);
    }
    catch(Decoder_error e){
    }
}
