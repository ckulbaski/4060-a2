#define BIT_END 7

#ifndef BIT_FILE_HPP

#define BIT_FILE_HPP

class BitFile {

private:
    FILE *fp;
    int pos; //indicates while bit is to be filled on next bit write. Value is between 0 and 7.
    uint8_t buf;
    bool bWriting;
    
public:
    int openBitFile(const char * path, const char * mode); //mode should be either "rb" or "wb"
    int closeBitFile();
    int writeBit(uint8_t bitval);
    uint8_t readBit();    
    int writeBits(uint32_t bits, int count);
    int writeFlush();
    int writeByte(uint8_t byteval);
    int write32Bits(uint32_t wordVal);
    int readByte(uint8_t *byte);
    int read32Bits(u_int32_t *w);

};

#endif