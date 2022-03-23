//Conner Kulbaski 7841266
#include <iostream>
#include <queue> 
#include <string>
#include <cmath>

using namespace std;


class BST_node{

public: 

    uint8_t symbol; 
    uint32_t frequency;
    string codeword;
    uint32_t cw_len;
    BST_node* left = NULL;
    BST_node* right = NULL;

    BST_node(uint8_t sym, uint32_t freq){
        symbol = sym;
        frequency = freq;
        left = NULL;
        right = NULL;
        cw_len = 0;
    }

    bool has_left_child(){
        return !(left == NULL);
    }
    bool has_right_child(){
        return !(right == NULL);
    }
    void add_left_child(BST_node* n){
        left = n;
    }
    void add_right_child(BST_node* n){
        right = n;
    }
    void rmv_left_child(){
        free(left);
        left = NULL;
    }
    void rmv_right_child(){
        free(right);
        right = NULL;
    }
    void print(){
        cout << "symbol: " << (int)symbol << " frequency: " << frequency <</* " codeword: " << codeword <<*/ "\n";
    }

};


class Q_object{
    public: 
        BST_node* item;

        Q_object(BST_node* n){
            item = n;
        }

};

bool operator<(const Q_object &s1, const Q_object &s2){
 
    // this will return true when second symbol
    // has  frequency less than that of the first symbol given by s1. 
    
    return s1.item->frequency >  s2.item->frequency;
}

int validate_args(int argc, char **argv){
    if(argc < 4){
        cout << "usage: ./A1Q1_Kulbaski_C [c/d] [in_file].huff [out_file].puff\n";
        return EXIT_FAILURE;
    }

    if(argv[1][0] != 'c' && argv[1][0] != 'd'){
        cout << "error: first argument should be c for compress or d for decompress.\n";
        return EXIT_FAILURE;
    }

    if(argv[1][0] == 'c'){
        string out_filename = argv[3];
        if(out_filename.find(".huff") == string::npos){
            cout << "error: out file should end in .huff\n";
            return EXIT_FAILURE;
        }

    }
    else if(argv[1][0] == 'd'){
        string filename = argv[2];
        if(filename.find(".huff") == string::npos){
            cout << "error: in file should end in .huff\n";
            return EXIT_FAILURE;
        }
        filename = argv[3];
        if(filename.find(".puff") == string::npos){
            cout << "error: in file should end in .huff\n";
            return EXIT_FAILURE;
        }

    }
    
    return EXIT_SUCCESS;
}

void print_tree(BST_node* n){

    if(n->has_left_child()){
        //cout << "left\n";
        //interior with left child
        print_tree(n->left);
    }
    if(n->has_right_child()){
        //cout << "right\n";
        //interior with right child
        print_tree(n->right);
    }
    if(!n->has_left_child() && !n->has_right_child()){
        //cout << "leaf\n";
        n->print();
    }

        
}

//builds codewords recursively and assigns to nodes
double cw_len = 0;
void build_codewords(BST_node* n, string codeword, string dest[256]){

    if(n->has_left_child()){
        //interior with left child
        build_codewords(n->left, codeword + "0", dest);
    }
    if(n->has_right_child()){
        //interior with right child
        build_codewords(n->right, codeword + "1", dest);
    }
    if(!n->has_left_child() && !n->has_right_child()){
        n->codeword = codeword;
        n->cw_len = codeword.size();
        dest[n->symbol] = n->codeword;
        cw_len += n->cw_len;
    }
}

void print_codewords(string codewords[256]){

    for(int i = 0; i < 256; i++){
        if(codewords[i].size() > 0 ){
            cout << "Symbol: " << i << " -> " << codewords[i] << "\n";
        }
    }
    cout << "\n";

}

//fills the frequency table 
//returns number of symbols 
uint32_t compress_fill_freq_table(uint32_t* freq_table, FILE* in){

    uint8_t sym;
    uint32_t num_syms=0;
    while(fread(&sym, 1, 1, in)){
        num_syms++;
        //cout << sym;
        freq_table[sym]++;
    }
    return num_syms;
}

//returns total number of symbols
//fills num_alphabet_syms with number of symbols in the alphabet
void decompress_fill_freq_table(uint32_t* freq_table, uint32_t num_alphabet_syms, FILE* in){

    uint8_t sym;
    for(uint32_t i = 0; i < num_alphabet_syms; i++){
        fread(&sym, 1, 1, in);
        fread(&freq_table[sym], 4, 1, in);
        //cout << "read " << (int)sym << " with freq " << freq_table[sym] <<"\n";
    }
}

uint32_t count_alphabet_syms(uint32_t* freq_table){
    uint32_t result = 0;
    for(int i = 0; i < 256; i++){ 
        if(freq_table[i] > 0){
            result ++;
        }
    }
    return result;
}


//returns average codeword length
double get_avg_cw_len(uint32_t* freq_table, uint32_t num_syms, string* codewords){
    double avg = 0;
    for(int i = 0; i < 256; i++){ 
        if(freq_table[i] > 0){
            //sum codeword lengths
            avg += ((double)freq_table[i] / (double)num_syms)*(double)codewords[i].size();
            
        }
    }
    return avg;
}

//writes the number of symbols and the symbol frequencies out to file
void write_freq_table(uint32_t* freq_table, FILE* out){
    for(int i = 0; i < 256; i++){
        if(freq_table[i] > 0){
            //write symbol and frequency to out file
            uint8_t sym = (uint8_t) i;
            fwrite(&sym, 1, 1, out);
            fwrite(&freq_table[sym], 4, 1, out);
        }
    }
}

//reads the in file and writes the codewords to out file
void compress_to_file(string* codewords, FILE* in, FILE* out){
    uint8_t sym;
    uint8_t buf = 0x00;
    int buf_index = 0; // number of written bits in buffer
    int num_written = 0;
    while(fread(&sym, 1, 1, in)){
        string codeword = codewords[sym];
        //cout << "Symbol: " << sym << " Codeword: " << codeword << "\n"; 
        while(codeword.size() > 0){
            // if 8 bits are in the buffer write to file
            if(buf_index == 8){
                //cout << "buf = "<< hex << (int)buf << ", writing to file\n";
                fwrite(&buf, 1, 1, out);
                // reset buffer 
                buf_index = 0;
                buf = 0x00;
            }
            // shift buffer left for new symbol 
            buf = buf << 1; 
            if(codeword[0] == '1'){ 
                // only write 1 if the symbol is 1
                buf = buf | 0x01; 
                //cout << "write 1\n";
            }
            //else{ cout << "write 0\n"; }
            buf_index ++;
            // remove first symbol of codeword
            codeword.erase(0,1); 
        }
        num_written++;

    }
    //cout << "buf = "<< hex << (int)buf << ", writing to file\n";
    buf = buf << (8-buf_index);
    fwrite(&buf, 1, 1, out);
    //cout << "num written: "<< num_written << "\n";
}

int get_sym_from_cw(string* codewords, string cw){
    for(int i = 0; i < 256; i++){
       if(codewords[i] == cw){
           return (uint8_t) i;
       }
    }
    return -1;
}

void decompress_to_file(string* codewords, uint32_t num_syms, FILE* in, FILE* out){
    uint8_t sym;
    uint32_t num_read = 0;

    string curr_codeword = "";
    while(num_read < num_syms){
        fread(&sym, 1, 1, in);
        for(uint8_t x = 0x80; x >= 0x01 && num_read < num_syms; x = x >> 1){
            char c = '0';
            if((sym & x) > 0){
                c = '1';
            }
            curr_codeword += c;
            int sym_from_cw = get_sym_from_cw(codewords, curr_codeword);
            if(sym_from_cw >= 0){
                uint8_t sym_8bits = (uint8_t)sym_from_cw;
                fwrite(&sym_8bits, 1,1,out);
                num_read++;
                curr_codeword = "";
                //write symbol to file
                //reset current codeword

            }

        }
        
    }
}

BST_node* build_coding_tree(uint32_t* freq_table){
    priority_queue<Q_object> Q;
    for(int i = 0; i < 256; i++){
        if(freq_table[i] > 0){
            //create coding tree nodes and add to queue
            BST_node* n = new BST_node(i, freq_table[i]);
            Q_object* item = new Q_object(n);
            Q.push(*item);
        }
    }

    while(Q.size() > 1){
        BST_node* left = Q.top().item;
        Q.pop();
        BST_node* right = Q.top().item;
        Q.pop();

        BST_node* new_root = new BST_node(-1, left->frequency + right->frequency);
        new_root->add_left_child(left);
        new_root->add_right_child(right); 

        Q_object* item = new Q_object(new_root);
        Q.push(*item);
    }
    BST_node* final_tree = Q.top().item;
    Q.pop();
    return final_tree;
}

void compress(char* in_filename, char* out_filename){
    cout << "Compressing " << in_filename << ".\n";

    //open in file for reading
    FILE *in = fopen(in_filename, "rb");
    if(!in){
        cout << "Error: " << in_filename << " not found.\n";
        return;
    }
    
    //fill freq table
    uint32_t freq_table[1<<8] = {0}; //2^8
    uint32_t num_syms = compress_fill_freq_table(freq_table, in);
    
    //create and populate priority queue

    //build and print coding tree
    BST_node* final_tree = build_coding_tree(freq_table);
    cout << "Symbol frequencies (symbol:frequency): \n";
    print_tree(final_tree);
    cout << "\n";

    //build and print codewords
    cout << "Codewords: \n";
    string codewords[256];
    build_codewords(final_tree, "", codewords);
    print_codewords(codewords);

    cout << "Average codeword length: "<< get_avg_cw_len(freq_table, num_syms, codewords) <<"\n";
    uint32_t num_alphabet_syms = count_alphabet_syms(freq_table);
    cout << "\n";

    //open and write everything to out file
    FILE *out = fopen(out_filename, "wb");
    //write number of symbols and number of symbols in alphabet
    fwrite(&num_syms, 4, 1, out);
    fwrite(&num_alphabet_syms, 4, 1, out);
    write_freq_table(freq_table, out);
    rewind(in);
    compress_to_file(codewords, in, out);

    //close files
    fclose(in);
    fclose(out);

    cout << "Compressed to " << out_filename << "\n";
}

void decompress(char* in_filename, char* out_filename){
    cout << "Decompressing " << in_filename << ".\n";

    //open in file for reading
    FILE *in = fopen(in_filename, "rb");
    
    //fill freq table
    uint32_t freq_table[1<<8] = {0}; //2^8
    uint32_t num_syms, num_alphabet_syms;
    fread(&num_syms, 4, 1, in);
    fread(&num_alphabet_syms, 4, 1, in);
    decompress_fill_freq_table(freq_table, num_alphabet_syms, in);
    

    //build and print coding tree
    BST_node* final_tree = build_coding_tree(freq_table);
    cout << "Symbol frequencies (symbol:frequency): \n";
    print_tree(final_tree);
    cout << "\n";
    
    //build and print codewords
    cout << "Codewords: \n";
    string codewords[256];
    build_codewords(final_tree, "", codewords);
    print_codewords(codewords);
    
    //create and populate priority queue
    double avg_cw_len = get_avg_cw_len(freq_table, num_syms, codewords);
    cout << "Average codeword length: "<< avg_cw_len <<", num_syms = "<< num_syms <<"\n";

    //open and write everything to out file
    FILE *out = fopen(out_filename, "wb");
    decompress_to_file(codewords, num_syms, in, out);

    //close files
    fclose(in);
    fclose(out);

    cout << "Decompressed to " << out_filename << "\n";

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

