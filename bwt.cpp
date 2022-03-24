//Conner Kulbaski 7841266
#include <iostream>
#include <string>
#include <cmath>
#include "BitFile.hpp"

#define DICT_SIZE 512

using namespace std;
typedef __uint32_t int32; 
class Dictionary{

private: 
    size_t size; 
    int next_open;
    int32* dict;

public: 
    Dictionary(){
        size = DICT_SIZE;
        next_open = 0;
        init_dictionary();
    }

    void init_dictionary(){
        dict = (int32*)malloc(size * sizeof(int32));
        for(int i = 0; i < 256; i++){
            dict[i] = i;
        }
    }

};

int validate_args(int argc, char **argv){
    if(argc < 4){
        cout << "usage: ./A1Q1_Kulbaski_C [c/d] [in_file] [out_file].bwt\n";
        return EXIT_FAILURE;
    }

    if(argv[1][0] != 'c' && argv[1][0] != 'd'){
        cout << "error: first argument should be c for compress or d for decompress.\n";
        return EXIT_FAILURE;
    }

    if(argv[1][0] == 'c'){
        string out_filename = argv[3];
        if(out_filename.find(".bwt") == string::npos){
            cout << "error: out file should end in .bwt\n";
            return EXIT_FAILURE;
        }

    }
    else if(argv[1][0] == 'd'){
        string filename = argv[2];
        if(filename.find(".bwt") == string::npos){
            cout << "error: in file should end in .bwt\n";
            return EXIT_FAILURE;
        }
        filename = argv[3];
        if(filename.find(".dbwt") == string::npos){
            cout << "error: out file should end in .dbwt\n";
            return EXIT_FAILURE;
        }

    }
    
    return EXIT_SUCCESS;
}

void compress(char* in_filename, char* out_filename){
    FILE *in = fopen(in_filename, "rb");
    FILE *out = fopen(out_filename, "wb");


    fclose(in);
    fclose(out);

}

void decompress(char* in_filename, char* out_filename){
    FILE *in = fopen(in_filename, "rb");
    FILE *out = fopen(out_filename, "wb");


    fclose(in);
    fclose(out);
    
}

int main(int argc, char **argv){

    if(validate_args(argc, argv) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    //initialize file i/o
    char* in_filename = argv[2];
    char* out_filename = argv[3];
    if(argv[1][0] == 'c'){
        compress(in_filename, out_filename);
    }
    else if(argv[1][0] == 'd'){
        decompress(in_filename, out_filename);
    }

    return EXIT_SUCCESS;
    
}

