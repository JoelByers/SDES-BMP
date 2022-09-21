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

int main(int argc, char *argv[])
{
    bool numInput = false;
    bool decryptMode = false;
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
            decryptMode = true;
        }
        else if(dOrE == "e")
        {
            decryptMode == false;
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
            decryptMode = true;
        }
        else if(dOrE == "e")
        {
            decryptMode == false;
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

    //take in pixel data byte by byte encrypt/decrypt then write to new file

    if(decryptMode == true)
    {
        cout << endl << "Decrypting " << fileName << endl;
        for(int i = 0; i < size; i++){
            bool bits[8] = {0,0,0,0,0,0,0,0};
            bool key[10] = {0,0,0,0,0,0,0,0,0,0}; //hardcode key
            asciiToBinary(pixelData[i], bits); //translate to boolean array
            decrypt(bits, key);
            pixelData[i] = binaryToAscii(bits); //translate back to ascii
        }
    }
    else{
        cout << endl << "Encrypting " << fileName << endl;

        for(int i = 0; i < size; i++){
            bool bits[8] = {0,0,0,0,0,0,0,0};
            bool key[10] = {0,0,0,0,0,0,0,0,0,0};
            asciiToBinary(pixelData[i], bits);
            encrypt(bits, key);
            pixelData[i] = binaryToAscii(bits);
        }
    }

    fwrite(header, sizeof(unsigned char), 54, cypher);
    fwrite(pixelData, sizeof(unsigned char), size, cypher);
    fclose(cypher);
}