//Conner Kulbaski 7841266
#include <iostream>
#include <string>
#include <cmath>
#include "BitFile.hpp"

#define DICT_SIZE 512
#define MAX_SIZE 4096

using namespace std;
 
class Dictionary{

private: 
    int size; 
    int next_open;
    string* dict;

public: 
    Dictionary(){
        size = DICT_SIZE;
        next_open = 0;
        init_dictionary();
    }

    void init_dictionary(){
        dict = new string[size];
        for(int i = 0; i < size; i++){
            if(i < 256){
                dict[i] = (char)i;
            }
            else dict[i] = "";
        }
    }

    string get(string p){
        return p;
    }

    bool put(string s){
        /*
        if(next_open == size){
            if(size == MAX_SIZE){
                return false; //full
            }
            expand_dictionary(dict);
        }
        dict[next_open++] = s;
        */
        s = s + " ";
        expand_dictionary(dict);
        return true;
    }

    void print(){
        for(int i = 0; i < size; i++){
            cout << "[" << i << "]: " << dict[i] << "\n";
        }
    }

private: 
    
    void expand_dictionary(string* src){
        cout << "Hello\n";
        size = 2*size;
        string* new_arr = new string[size];
        if(new_arr){
            cout << "New array "<< size << endl;
        }
        for(int i = 0; i < size; i++){
            if( i < size/2){
                new_arr[i] = src[i];
            }
            else{
                new_arr[i] = "";
            }
        }
        cout << "Copied array\n";
        src = new_arr;
    }

};

int validate_args(int argc, char **argv){
    if(argc < 4){
        cout << "usage: ./A1Q1_Kulbaski_C [c/d] [in_file] [out_file].lzw\n";
        return EXIT_FAILURE;
    }

    if(argv[1][0] != 'c' && argv[1][0] != 'd'){
        cout << "error: first argument should be c for compress or d for decompress.\n";
        return EXIT_FAILURE;
    }

    if(argv[1][0] == 'c'){
        string out_filename = argv[3];
        if(out_filename.find(".lzw") == string::npos){
            cout << "error: out file should end in .lzw\n";
            return EXIT_FAILURE;
        }

    }
    else if(argv[1][0] == 'd'){
        string filename = argv[2];
        if(filename.find(".lzw") == string::npos){
            cout << "error: in file should end in .lzw\n";
            return EXIT_FAILURE;
        }
        filename = argv[3];
        if(filename.find(".dlzw") == string::npos){
            cout << "error: out file should end in .dlzw\n";
            return EXIT_FAILURE;
        }

    }
    
    return EXIT_SUCCESS;
}

void compress(char* in_filename, char* out_filename){
    FILE *in = fopen(in_filename, "rb");
    FILE *out = fopen(out_filename, "wb");

    Dictionary dict = Dictionary();
    dict.print();
    dict.put("z");
    dict.print();
/*
    string p;
    char c;
    fread(&c, 1, 1, in);
    p = c; 
    while(fread(&c, 1, 1, in)){
        if( dict.get(p+c) ){
            p = p+c;
        }
        else{
            // output index in dictionary for P
            // put p+ch in dictionary
            // p = ch
        }
    }
*/
    // output index for p
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

