// g++ -o run.exe main.c

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <filesystem>

using namespace std;

void readFiles(string directory)
{
    FILE *img = fopen("img0.png", "rb");
    if (img == nullptr)
    {
        cout << "Error opening file" << endl;
    }
    string filecontents = "";
    int c = 0;
    while ((c = fgetc(img)) != EOF)
    {
        filecontents += static_cast<char>(c);
    }
    fclose(img);
    printf("%s", filecontents.c_str());
}

void displayFile(string fileName)
{
    FILE *image = fopen(fileName, "rb");

    if (image == nullptr)
    {
        cout << "Error opening file" << endl;
    }
}

int main()
{
    displayFile("testImages/img0.png") return 0;
}
