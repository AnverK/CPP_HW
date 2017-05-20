#include <iostream>
#include "huffman.h"
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
    Huffman h;
    char *name_input = argv[2], *name_output = argv[3];
    if(mode == 0)
    {
        h.compress(name_input, name_output);
    }
    else if(mode == 1)
    {
        h.decompress(name_input, name_output);
    }

//    h.compress("figure.png", "output.hfm");
//    h.decompress("output.hfm", "output.png");
//    h.compress("input.docx", "output.hfm");
//    h.decompress("output.hfm", "output.docx");
//    h.compress("input.txt", "output.hfm");
//    h.decompress("output.hfm", "output.txt");
//    h.compress("bigtest.exe", "output.hfm");
//    h.compress("medium.exe", "output.hfm");
//    cout << clock() << endl;
//    h.decompress("output.hfm", "output.exe");
//    cout << clock() << endl;
    return 0;
}

