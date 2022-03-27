#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>

#include"BitFile.hpp"

typedef u_int8_t uint8_t;
typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;

int BitFile::openBitFile(const char filename[], const char mode[]) {
//assume mode is either "rb" or "wb"
    
    fp = fopen(filename,mode);

    assert(fp != NULL);
    
    if (!(fp)) {
        return 1; //failed
    }
    buf = 0;
    pos = 0;

    if (strncmp(mode,"wb",2) == 0)
        bWriting = true;
    else 
        bWriting = false;

    return 0; //0 means okay
}

int BitFile::closeBitFile() {

    if (pos > 0 && bWriting) {
        writeFlush();
    }

    fclose(fp);
    return 0;
}

//bits are written from lsb to msb
int BitFile::writeBit(uint8_t bitval) {

    if (bitval == 1){
        bitval = bitval << pos;
        buf = buf | bitval;        
    }    

    if (pos == BIT_END) {
        fwrite(&buf, 1,1, fp);
        pos = 0;
        buf = 0;    
    }
    else {
        pos = pos + 1;
    }
    return 0;
}



//bit 0 is written first as the lsb in the output byte
//bit count-1 is written last and will be the high order bit of those written by this routine
//eg. bits = 0x06, count = 4
//Then the bits will be 0110

int BitFile::writeBits( uint32_t bits, int count) {

    int i;

    for (i = 0; i < count;i++) {
        if (bits & 0x01) {
            writeBit(1);
        }
        else {
            writeBit(0);
        }
        bits = bits >> 1;
    }
    return 0;
}



/*used to force write out contents when encoding is done*/
/*This must be called before closing the output file during compression*/

int BitFile::writeFlush() {

    if (pos > 0 && bWriting) {
        fwrite(&buf, 1,1,fp);

    }    
    pos = 0;
    buf = 0;
    fflush(fp);
    return 0;
}




//You need to call writeFlush() before calling the following two functions if you
//call writeBits() or writeBit() previously. 
//If you want to write a byte (or 32-bits ) immediately following writing a bunch of bits, 
//you should use writeBits() instead of writeByte(), write32Bits().

int BitFile::writeByte(uint8_t byteVal) {
    int cnt;

    cnt = fwrite(&byteVal,sizeof(uint8_t),1,fp);
    if (cnt == 1)
        return 0; //good
    else
        return 1; //bad
}

//write a 32-bit value to file
int BitFile::write32Bits(uint32_t wordVal) {

    int cnt;

    cnt = fwrite(&wordVal,sizeof(uint32_t),1,fp);
    if (cnt == sizeof(uint32_t))
        return 0; //good
    else
        return 1; //bad

}

/*read next bit and store as 0 or 1 in return value*/
uint8_t BitFile::readBit() { //returns either 0x00 or 0x01 , unless something goes wrong which returns -1.
    uint8_t bitval=0;
    int cnt;

    if (pos == 0) {
        cnt = fread(&buf,1,1,fp);
        if (cnt !=1) {
            return -1;  //to denote something went wrong
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
//read an byte to file
int BitFile::readByte(uint8_t *byte) {

    int cnt;
    cnt = fread(byte, 1, 1, fp);

    if (cnt == 1)
        return 0; //good
    else
        return 1; //bad

}

//read a 32 bit value from file
int BitFile::read32Bits(uint32_t *wordVal) {

    int cnt;

    cnt = fread(wordVal,sizeof(uint32_t),1,fp);
    if (cnt == sizeof(uint32_t))
        return 0; //good
    else
        return 1; //bad

}