#include <iostream>
#include <fstream>
#include <ctime>
#include <string.h>

#include "exceptions.h"
#include "compressor.h"
#include "decompressor.h"


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
    ifstream input(inputFileName, ios::binary);

    uint64_t length;
    input.seekg(0, ios::end);
    length = input.tellg();

    input.seekg(0, ios::beg);

    size_t was_read = 0;
    vector<uint8_t> in(min(SIZE_BLOCK, length-was_read));

    input.read((char *) in.data(), in.size());
    WeightCounter wc = WeightCounter(in);
    was_read += in.size();

    while(was_read < length)
    {
        in.resize(min(SIZE_BLOCK, length-was_read));
        input.read((char *) in.data(), in.size());
        wc.add_block(in);
        was_read += SIZE_BLOCK;
    }

    CodeTable ct = CodeTable(wc);

    uint16_t unique;

    vector <uint16_t> out16;
    vector <uint8_t> out8;
    Compressor comp = Compressor(ct, length, unique, out16, out8);

    ofstream output(outputFileName, ios::binary);

    output.write((char*)&length, sizeof(length));
    output.write((char*)&unique, sizeof(unique));
    output.write((char*)out16.data(), sizeof(uint16_t)*out16.size());
    output.write((char*)out8.data(), out8.size());

    uint8_t end_bit = 0;
    out8.clear();
    input.seekg(0, ios::beg);

    was_read = 0;
    while(was_read < length)
    {
        in.resize(min(SIZE_BLOCK, length-was_read));
        input.read((char *) in.data(), in.size());
        comp.compress_block(in, out8, end_bit); //в out будет ещё лишний байтик, который может недописан, а может и дописан
        output.write((char*)out8.data(), out8.size() - 1);
        was_read += SIZE_BLOCK;
    }
    if(end_bit % 8)
    {
        output << out8[out8.size()-1];
    }
}

void decompress(string inputFileName, string outputFileName)
{

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

    vector <uint16_t> in16(Decompressor::count_tree_input_size_by_unique(unique));
    input.read((char*) in16.data(), 2*in16.size());

    vector <uint8_t> in8(Decompressor::count_leaves_input_size_by_unique(unique));

    input.read((char*) in8.data(), in8.size());

    Decompressor decomp = Decompressor(length, unique, in16, in8);

    uint64_t was_read = input.tellg();
    vector <uint8_t> out;

    while(was_read < input_file_length)
    {
        in8.resize(min(SIZE_BLOCK, input_file_length-was_read));
        input.read((char *) in8.data(), in8.size());

        decomp.decompress_block(in8, out);
        output.write((char*)out.data(), out.size());
        out.clear();
        was_read += SIZE_BLOCK;
    }
}

int main(int argc, char* argv[])
{
//    if(argc != 4)
//    {
//        if(argc == 2 && strcmp(argv[1], "--help") == 0)
//        {
//            show_help();
//            return 0;
//        }
//        else
//        {
//            std::cerr << "ERROR:\tIncorrect number of arguments. Run this application with only one argument: '--help'\n";
//            return 1;
//        }
//    }
//    int mode;
//    if(strcmp(argv[1], "-c") == 0)
//    {
//        mode = 0;
//    }
//    else if(strcmp(argv[1], "-d") == 0)
//    {
//        mode = 1;
//    }
//    else
//    {
//        std::cerr << "Incorrect first argument. Run this application with only one argument: '--help'\n";
//        return 1;
//    }

//    char *name_input = argv[2], *name_output = argv[3];
//    if(mode == 0)
//    {
//        try {
//            compress(name_input, name_output);
//        }
//        catch(...){
//            std::cerr << "Failed to compress file" << endl;
//        }
//    }
//    else if(mode == 1)
//    {
//        try {
//            decompress(name_input, name_output);
//        }
//        catch(...){
//            std::cerr << "Failed to decompress file" << endl;
//        }
//    }

//    compress("huge.bin", "huge.hfm");
//    cout << (double)clock()/1000<< endl;
//    decompress("huge.hfm", "huge_out.bin");
//    cout << (double)clock()/1000<< endl;


//    compress("input.txt", "output.hfm");
//    compress("input2.txt", "output2.hfm");
//    decompress("output.hfm", "output.txt");
//    compress("medium.exe", "medium.hfm");
    compress("huge.exe", "huge.hfm");
    cout << (double)clock()/1000<< endl;
    decompress("huge.hfm", "huge_out.exe");
//    decompress("medium.hfm", "medium_out.exe");
    cout << (double)clock()/1000<< endl;



//    compress("empty.txt", "output_empty.hfm");
//    decompress("output_empty.hfm", "empty_out.txt");
//    decompress("medium.hfm", "out_medium.exe");
//    cout << (double)clock()/1000 << endl;
    return 0;
}
