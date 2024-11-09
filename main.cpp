// gcc -o run.exe main.c

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

int main()
{
    FILE *img = fopen("img0.png", "rb");

    if (img == nullptr)
    {
        cout << "Error opening file" << endl;
        return 1;
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
