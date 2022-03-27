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
        uint8_t i;
        for(i = 0; i < 255; i++){
            dict[i] = (char)i;
        }
        i = 255;
        dict[255] = (char)i;
        next_open = 256;
    }

    // returns the index of the string in the table or -1 for failure
    uint32_t find(string p){
        for( int i = 0; i < next_open; i++){
            if(dict[i] == p){
                cout << "Symbol [" << i << "] is " << p << endl;
                return (uint32_t)i;
            }
        }
        return -1;
    }

    string get(uint32_t index){
        return dict[index];
    }

    bool put(string s){
        
        if(next_open == size){
            if(size == MAX_SIZE){
                return false; //full
            }
            expand_dictionary();
        }
        dict[next_open++] = s;
        return true;
    }

    void print(){
        for(int i = 0; i < size; i++){

            cout << "[" << i << "]: ";
            if(dict[i] == ""){
                cout << "npos";
            } 
            else cout << dict[i] << "\n";
        }
    }

    int get_size(){
        return size;
    }

private: 
    
    void expand_dictionary(){
        cout << "Hello\n";
        size = 2*size;
        string* new_arr = new string[size];
        if(new_arr){
            cout << "New array "<< size << endl;
        }
        for(int i = 0; i < size; i++){
            if( i < size/2){
                new_arr[i] = dict[i];
            }
            else{
                new_arr[i] = "";
            }
        }
        cout << "Copied array\n";
        dict = new_arr;
    }

};

//the following 2 writeBit(s) functions were adapted from Ben Li's "BitFile.cpp" 
//interface to work with my program
uint8_t buf = 0;
int pos = 0; //indicates while bit is to be filled on next bit write. Value is between 0 and 7.
void writeBit(uint8_t bitval, FILE* out) {

    if (bitval == 1){
        bitval = bitval << pos;
        buf = buf | bitval;        
    }    

    if (pos == BIT_END) {
        fwrite(&buf, 1,1, out);
        pos = 0;
        buf = 0;    
    }
    else {
        pos++;
    }
}

void writeBits( uint32_t bits, int count, FILE* out) {

    int i;
    cout << "writing ";
    for (i = 0; i < count;i++) {
        if (bits & 0x80) {
            cout << "1";
            writeBit(1, out);
        }
        else {
            cout << "0";
            writeBit(0, out);
        }
        bits = bits << 1;
    }
    cout << endl;
}

void writeFlush(FILE* out) {

    if (pos > 0) {
        fwrite(&buf, 1,1,out);

    }    
    pos = 0;
    buf = 0;
    fflush(out);
}

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


    string p;
    char c;
    fread(&c, 1, 1, in);
    p = c; 
    while(fread(&c, 1, 1, in)){

        int index = dict.find(p+c);
        if( index >= 0 ){
            p = p+c;
            //output index
        }
        else{
            uint32_t index = dict.find(p);
            int len = (int)log2(dict.get_size());
            cout << "Writing " << len << " bits of " << index << endl;
            writeBits(index, len, out);
            string p_c = p+c;
            dict.put(p_c);
            p = c;
        }
    }

    uint32_t index = dict.find(p);
    int len = (int)log2(dict.get_size());
    cout << "Writing " << len << " bits of " << index << endl;
    writeBits(index, len, out);

    writeFlush(out);

    cout << "Compressed file to " << out_filename << endl;
    fclose(in);
    fclose(out);

}


// this function was also taken from Ben Li's BitFile interface 
// to work with my program. 
uint8_t readBit(FILE* in) { //returns either 0x00 or 0x01 , unless something goes wrong which returns -1.
    uint8_t bitval=0;
    int cnt;

    if (pos == 0) {
        cnt = fread(&buf,1,1,in);
        if (cnt !=1) {
            return 2;  //to denote something went wrong
        }
    }
    bitval = buf & 0x01; //get bit value

    buf = buf >> 1; //move to next bit
    //update position
    if (pos == BIT_END) {
        pos = 0;
    } 
    else {
        pos = pos + 1;
    }

    return bitval;
}

uint32_t get_next_index(FILE* in, int len){
    uint32_t index = 0; 

    for(int i = 0; i < len; i++){
        uint8_t bit = readBit(in);
        if(bit == 2){
            return 0x8000;
        } 
        index = index << 1;
        if(bit == 1){
            index = index | 0x01;
        }
    }
    return index;
}

void decompress(char* in_filename, char* out_filename){
    FILE *in = fopen(in_filename, "rb");
    FILE *out = fopen(out_filename, "wb");
    
    Dictionary dict = Dictionary();


   
    //get first index
    int index_length = (int)log2(dict.get_size());
    uint32_t old_index = get_next_index(in, index_length);
    cout << "First index is " << old_index << " and has " << index_length << " bits" << endl; 
    string old_str = dict.get(old_index);
    cout << "Found string" << old_str << endl;
    fwrite(&old_str, 1, old_str.size(), out);

    while(old_index > 0){
        uint32_t new_index  = get_next_index(in, index_length); 
        if(new_index == 0x8000){
            break;
        }
        string str = dict.get(new_index);
        if(str == ""){
            str = dict.get(old_index);
            str = str + str.front();
        }
        fwrite(&str, 1, str.size(), out);
        char ch = str.front();
        dict.put(old_str += ch);
        old_index = new_index;
        old_str = dict.get(old_index);

        index_length = (int)log2(dict.get_size());
    }

    cout << "Deompressed file to " << out_filename << endl;
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

