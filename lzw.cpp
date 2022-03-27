//Conner Kulbaski 7841266
#include <iostream>
#include <string>
#include <cmath>
#include <map>

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
        int i;
        for(i = 0; i < 256; i++){
            string s = "";
            s += char(i);
            dict[i] = s;
        }
        next_open = 256;
    }

    // returns the index of the string in the table or -1 for failure
    uint32_t find(string p){
        for( int i = 0; i < next_open; i++){
            if(dict[i] == p){
                //cout << "Symbol [" << i << "] is " << p << endl;
                return (uint32_t)i;
            }
        }
        return -1;
    }

    string get(uint32_t index){
        /*
        if(dict[index] == ""){
            cout << "npos" << endl;
        }
        else cout << " " << dict[index];
        */
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
            else cout << dict[i];
            cout << endl;
        }
    }

    int get_size(){
        return size;
    }

private: 
    
    void expand_dictionary(){
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


uint8_t buf = 0;
int buf_index = 0; 
void write_bits( uint32_t bits, int count, FILE* out) {

    //cout << "Writing " << count << " bits of " << bits << endl;

    uint32_t i = 0x0001;
    for(i = i << (count-1); i >= 0x0001; i = i >> 1){
        
        if(buf_index == 8){
            fwrite(&buf, 1, 1, out);
            buf_index = 0;
        }

        buf = buf << 1;
        
        if(bits & i){
            buf = buf | 1;
            //cout << "1";
        }
        //else { cout << "0"; }

        buf_index ++ ;

    }

    //cout << endl;
}

void flush_out(FILE* out) {

    buf = buf << (8 - buf_index);
    fwrite(&buf, 1,1,out);

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
    if(!in){
        cout << "Could not find "<< in_filename << endl;
        return;
    }
    if(!out){
        cout << "Could not find "<< out_filename << endl;
        return;
    }

    Dictionary dict = Dictionary();

    string p = "";
    char c;
    fread(&c, 1, 1, in);
    p += c;
    while(fread(&c, 1, 1, in)){
        int index = dict.find(p+c);
        if( index >= 0 ){
            p = p+c;
        }
        else{
            uint32_t index = dict.find(p);
            int len = (int)log2(dict.get_size());
            write_bits(index, len, out);
            dict.put(p+c);
            p = c;
        }
    }

    uint32_t index = dict.find(p);
    int len = (int)log2(dict.get_size());
    //cout << "Writing " << len << " bits of " << index << endl;
    write_bits(index, len, out);

    flush_out(out);

    dict.print();

    cout << "Compressed file to " << out_filename << endl;
    fclose(in);
    fclose(out);

}


// this function was taken from Ben Li's BitFile interface 
// to work with my program. 
uint8_t read_buf = 0;
int read_buf_index = 8;
uint8_t read_bit(FILE* in) { //returns either 0x00 or 0x01 , unless something goes wrong which returns -1.

    int cnt; 
    uint8_t bitval=0;

    if(read_buf_index == 8){
        cnt = fread(&buf,1,1,in);
        read_buf_index = 0;
        if (cnt !=1) {
            return 2;  //to denote something went wrong
        }
    }

    uint8_t bitmask = 0x80 >> (read_buf_index);
    if(buf & bitmask) {
        bitval = 1;
    }
    else {
        bitval = 0;
    }
    read_buf_index++;
    return bitval;


}

uint32_t get_next_index(FILE* in, int len){
    uint32_t index = 0; 

    cout << "Next "<<len<<"-bit index: ";
    for(int i = 0; i < len; i++){
        uint8_t bit = read_bit(in);
        if(bit == 2){ // error
            cout << "EOF" << endl;
            return 0x8888;
        } 
        index = index << 1;
        if(bit == 1){
            index = index | 0x01;
            cout << "1";
        }
        else { cout << "0"; }
    }
    cout << endl;
    return index;
}

void write_string(string s, FILE* out){
    
    long unsigned int i;
    for(i = 0; i < s.size(); i++){
        fwrite(&s[i], 1, 1, out);
    }

}

void decompress(char* in_filename, char* out_filename){
    FILE *in = fopen(in_filename, "rb");
    FILE *out = fopen(out_filename, "wb");

    if(!in){
        cout << "Could not find "<< in_filename << endl;
        return;
    }
    if(!out){
        cout << "Could not find "<< out_filename << endl;
        return;
    }
    
    Dictionary dict = Dictionary();


   
    //get first index
    int index_length = (int)log2(dict.get_size());
    uint32_t old_index = get_next_index(in, index_length);
    string old_str = dict.get(old_index);

    write_string(old_str, out);

    while(old_index < 0x8888){
        uint32_t new_index  = get_next_index(in, index_length); 
        if(new_index == 0x8888){
            cout << "EOF" << endl;
            break;
        }
        string str = dict.get(new_index);
        if(str == ""){
            cout << "dict[" << new_index << "] is empty" << endl;
            str = dict.get(old_index);
            str = str + str.front();
        }
        write_string(str, out);
        char ch = str.front();
        dict.put(old_str + ch);
        old_index = new_index;
        old_str = dict.get(old_index);

        index_length = (int)log2(dict.get_size());
    }

    cout << "Decompressed file to " << out_filename << endl;
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

