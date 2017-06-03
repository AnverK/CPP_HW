#include <iostream>
#include <fstream>
#include "compression.h"
#include "decompression.h"
#include "exceptions.h"
#include <ctime>
#include <string.h>

using namespace std;

void show_help()
{
    std::cerr << "You can use 1 or 3 arguments:\n";
    std::cerr << "If you use only 1 argument it should be only '--help'\n";
    std::cerr << "Otherwise:\n";
    std::cerr << "1 argument: '-c' for compressing or '-d' for decomressing file\n";
    std::cerr << "2 argument: path to input file\n";
    std::cerr << "3 argument: path to output file\n";
}

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
    input.close();
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



int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        if(argc == 2 && strcmp(argv[1], "--help") == 0)
        {
            show_help();
            return 0;
        }
        else
        {
            std::cerr << "ERROR:\tIncorrect number of arguments. Run this application with only one argument: '--help'\n";
            return 1;
        }
    }
    int mode;
    if(strcmp(argv[1], "-c") == 0)
    {
        mode = 0;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        mode = 1;
    }
    else
    {
        std::cerr << "Incorrect first argument. Run this application with only one argument: '--help'\n";
        return 1;
    }

    char *name_input = argv[2], *name_output = argv[3];
    if(mode == 0)
    {
        try {
            compress(name_input, name_output);
        }
        catch(...){
            std::cerr << "Failed to compress file" << endl;
        }
    }
    else if(mode == 1)
    {
        try {
            decompress(name_input, name_output);
        }
        catch(...){
            std::cerr << "Failed to decompress file" << endl;
        }
    }

//    compress("input.txt", "output.hfm");
//    decompress("output.hfm", "output.txt");
//    compress("input2.txt", "output2.hfm");
//    decompress("output2.hfm", "output2.txt");
//    compress("medium.exe", "medium.hfm");


//    compress("empty.txt", "output_empty.hfm");
//    decompress("output_empty.hfm", "empty_out.txt");
//    cout << (double)clock()/1000<< endl;
//    decompress("medium.hfm", "out_medium.exe");
//    cout << (double)clock()/1000 << endl;
    return 0;
}
