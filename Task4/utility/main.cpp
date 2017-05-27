#include <iostream>
#include <fstream>
#include "compression.h"
#include "decompression.h"
#include <ctime>

using namespace std;
const size_t SIZE_BLOCK = 4096;

void compress(string inputFileName, string outputFileName)
{
    Compression comp = Compression();
    ifstream input(inputFileName, ios::binary);

    input.seekg(0, ios::end);
    size_t length = input.tellg();
    uint16_t unique;
    input.seekg(0, ios::beg);

    vector<uint8_t> in(SIZE_BLOCK);
    size_t was_read = 0;

    while(was_read < length)
    {
        in.resize(min(SIZE_BLOCK, length-was_read));
        input.read((char *) in.data(), min(SIZE_BLOCK, length-was_read));
        comp.count_frequencies(in);
        was_read += SIZE_BLOCK;
    }
    comp.make_list();
    unique = comp.get_unique();
    comp.go_through_list();
    comp.make_table();


    input.seekg(0, ios::beg);
    was_read = 0;
    vector <uint8_t> out;


    ofstream output(outputFileName, ios::binary);

    output.write((char*)&length, sizeof(length));
    output.write((char*)&unique, sizeof(unique));
    uint8_t end_bit = 0;

    vector <uint16_t> out16;
    comp.write_tree_structure(out16);
    output.write((char*)out16.data(), sizeof(uint16_t)*out16.size());

    out16.clear();
    comp.write_tree_leaves(out);
    output.write((char*)out.data(), out.size());
    out.clear();

    while(was_read < length)
    {
        in.resize(min(SIZE_BLOCK, length-was_read));
        input.read((char *) in.data(), min(SIZE_BLOCK, length - was_read));
        comp.compress(in, out, end_bit); //в out будет ещё лишний байтик, который может недописан, а может и дописан
        output.write((char*)out.data(), out.size() - 1);

        was_read += SIZE_BLOCK;
    }
    if(end_bit % 8)
    {
        output << out[out.size()-1];
    }
    output.close();
}

void decompress(string inputFileName, string outputFileName)
{
    Decompression decomp = Decompression();
    ifstream input(inputFileName, ios::binary);
    ofstream output(outputFileName, ios::binary);

    uint64_t input_file_length;
    input.seekg(0, ios::end);
    input_file_length = input.tellg();
    input.seekg(0, ios::beg);

    uint64_t length;
    uint16_t unique;
    input.read((char*)&length, sizeof(length));
    input.read((char*)&unique, sizeof(unique));
    decomp.set_length(length);
    decomp.set_unique(unique);

    vector <uint16_t> in16(4096);
	cout << 3*(unique-1)*sizeof(uint16_t) << endl;
    input.read((char*) in16.data(), 3*(unique-1)*sizeof(uint16_t));


    decomp.read_edges(in16);

    vector <uint8_t> in(4096);

    input.read((char*) in.data(), unique*sizeof(uint8_t));
    decomp.make_tree(in);

    uint64_t was_read = input.tellg();
    vector <uint8_t> out;

    while(was_read < input_file_length)
    {
        in.resize(min(SIZE_BLOCK, input_file_length-was_read));
        input.read((char *) in.data(), min(SIZE_BLOCK, input_file_length- was_read));

        decomp.decompress(in, out);
        output.write((char*)out.data(), out.size());
        out.clear();
        was_read += SIZE_BLOCK;
    }

    input.close();
    output.close();
}

int main()
{
//    compress("input.txt", "output.hfm");
//    decompress("output.hfm", "output.txt");
//    compress("input2.txt", "output2.hfm");
//    decompress("output2.hfm", "output2.txt");
    compress("huge.exe", "huge.hfm");
    cout << (double)clock()/1000<< endl;
    decompress("huge.hfm", "output_huge.exe");
    cout << (double)clock()/1000 << endl;
    return 0;
}
