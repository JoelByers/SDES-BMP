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

    //take in pixels as unsigned chars translated into boolean array
    //decrypt the pixels and xor with the iv array
    //Write to a bmp file given

    if(decryptMode == true)
    {
        cout << endl << "Decrypting " << fileName << endl;

        bool iv[8] = {0,0,0,0,0,0,0,0}; //initialization vector to be xored with the output of decryption
        bool bits[8] = {0,0,0,0,0,0,0,0};
        bool previousBits[8] = {0,0,0,0,0,0,0,0}; //placeholder to then be put into the iv
        bool key[10] = {0,0,0,0,0,0,0,0,0,0}; //Key hardcoded

        asciiToBinary(pixelData[0], bits); //take in unsigned character and return boolean array
        
        for(int j = 0; j < 8; j++){ //overrite previous bits to then overrite iv later
            previousBits[j] = bits[j];
        }
        decrypt(bits, key);
        exclusiveOr(bits, iv, 8);
        pixelData[0] = binaryToAscii(bits); //translate back to ascii

        for(int i = 1; i < size; i++){ //after the first iteration loop through the entirity of the bmp file
            asciiToBinary(pixelData[i], bits);
            for(int j = 0; j < 8; j++){
                iv[j] = bits[j];
            }

            decrypt(bits, key);
            exclusiveOr(bits, previousBits, 8);
            pixelData[i] = binaryToAscii(bits);

            for(int j = 0; j < 8; j++){
                previousBits[j] = iv[j];
            }
        }
    }
    else
    {
        cout << endl << "Encrypting " << fileName << endl;
            bool iv[8] = {0,0,0,0,0,0,0,0};

        for(int i = 0; i < size; i++){
            bool bits[8] = {0,0,0,0,0,0,0,0};
            bool key[10] = {0,0,0,0,0,0,0,0,0,0};
            asciiToBinary(pixelData[i], bits);
            exclusiveOr(bits, iv, 8); //exclusive or before encrypting with iv
            encrypt(bits, key);
            
            for(int j = 0; j < 8; j++){
                iv[j] = bits[j];
            }
            pixelData[i] = binaryToAscii(bits);
        }
    }



    fwrite(header, sizeof(unsigned char), 54, cypher); //use the same header as the original image used
    fwrite(pixelData, sizeof(unsigned char), size, cypher); //write the rest of the ecrypted/decrypted to the file given
    fclose(cypher);
}