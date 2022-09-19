// Headers
//Nathan Dobbs
//Some sources for BMPS
//http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
//https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file/38440684
//^useful stackexchange thread that got me started on reading the bmp and fixing some weird errors//
//
#include "SDES.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <math.h>
#include <stdio.h>

using namespace std;

bitset<8> InitialPerm(bitset<8> byte);
bitset<8> Reverse(bitset<8> byte);
char getChar(bitset<8> byte);
bitset<4> functionF(bitset<8> byte, bitset<8> key);
bitset<8> Expansion(bitset<4> byte);
bitset<8> XOR(bitset<8> first, bitset<8> second);
bitset<4> SBoxes(bitset<8> byte);
int conversionToInt(int one, int two);
bitset<2> conversionToBin(int num);
bitset<4> P4(bitset<4> halfByte);
bitset<8> LRSwaped(bitset<8> byte, bitset<4> right);
bitset<4> XOR4(bitset<4> first, bitset<4> second);
bitset<8> FinalPerm(bitset<8> byte);
int binaryToInt(bitset<8> binary);
void printByte(bitset<8> byte);

// Main function
int main(int argc, char *argv[])
{
    bool numInput = false;
    bool decrypt = false;
    string fileName;
    string outputName;
    string dOrE = "";
    string tempKey = "";

    if(argc == 2)
    {
        fileName = argv[1];
        outputName = "cypher.bmp";
    }
    else if(argc == 3)
    {
        fileName = argv[1];
        outputName = argv[2];
    }
    else if(argc == 4)
    {
        fileName = argv[1];
        outputName = argv[2];
        dOrE = argv[3];
        if(dOrE == "d")
        {
            decrypt = true;
        }
        else if(dOrE == "e")
        {
            decrypt == false;
        }
        else
        {
            cout << endl << "Unknown third argument, program will now exit" << endl;
            return(404);
        }
    }
    else if(argc == 5)
    {
        fileName = argv[1];
        outputName = argv[2];
        dOrE = argv[3];
        if(dOrE == "d")
        {
            decrypt = true;
        }
        else if(dOrE == "e")
        {
            decrypt == false;
        }
        else
        {
            cout << "Unknown third argument, program will now exit" << endl;
            return(404);
        }
        tempKey = argv[4];
    }

    else
    {
        fileName = "normal.bmp";
        outputName = "cypher.bmp";
    }

    if(decrypt == true)
    {
        cout << endl << "Decrypting " << fileName << endl;
    }
    else
        cout << endl << "Encrypting " << fileName << endl;

	// File I/O
    FILE* picture;
    FILE* cypher;

    cypher = fopen(outputName.c_str(), "wb");

    if(!(picture = fopen(fileName.c_str(), "rb")))
    {
        cout << fileName << " is missing program will now exit" << endl;
        return(1);

    }

    unsigned char header[54];

    //Reading the header
    fread(header, sizeof(unsigned char), 54, picture);

    //getting picture stats
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    //Need this to support multiple BMP types
    short int bitPerPixel = *(short int*) &header[28];
    int multiplier = bitPerPixel/8;

    int headerSize = * (int *) &header[14];

    //Dealing with Padding Rules
    if(width % 4 != 0)
    {
        width = width + (4 - (width % 4));
    }

    int size = multiplier * width * height;
    unsigned char* pixelData = new unsigned char[size];

    //Reading Pixel Data
    fread(pixelData, sizeof(unsigned char), size, picture);
    fclose(picture);
//          .
//          .
//          .
//          .

//  (you use your own S-DES routine to encrypt and decrypt the images.)
