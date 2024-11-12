// g++ -o run.exe main.cpp -lpng
#include <iostream>
#include <vector>
#include <string>
#include <png.h>
#include <iomanip>
#include <cstdio>

void displayFile(const std::string &fileName)
{
    FILE *image = fopen(fileName.c_str(), "rb");
    if (image == nullptr)
    {
        std::cout << "Error opening file" << std::endl;
        return;
    }

    png_structp png_read = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_read)
    {
        std::cout << "Error creating PNG read struct" << std::endl;
        fclose(image);
        return;
    }

    png_infop info = png_create_info_struct(png_read);
    if (!info)
    {
        std::cout << "Error creating PNG info struct" << std::endl;
        png_destroy_read_struct(&png_read, nullptr, nullptr);
        fclose(image);
        return;
    }

    FILE *outputImage = fopen("output.png", "wb");
    if (!outputImage)
    {
        std::cout << "Error opening output file for writing" << std::endl;
        png_destroy_read_struct(&png_read, &info, nullptr);
        fclose(image);
        return;
    }

    png_structp png_write = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_write)
    {
        std::cout << "Error creating PNG write struct" << std::endl;
        fclose(outputImage);
        png_destroy_read_struct(&png_write, &info, nullptr);
        fclose(image);
        return;
    }

    if (setjmp(png_jmpbuf(png_read)))
    {
        std::cout << "Error during PNG read" << std::endl;
        png_destroy_read_struct(&png_read, &info, nullptr);
        fclose(image);
        return;
    }

    png_init_io(png_read, image);
    png_read_info(png_read, info);

    int width = png_get_image_width(png_read, info);
    int height = png_get_image_height(png_read, info);
    png_byte color_type = png_get_color_type(png_read, info);
    png_byte bit_depth = png_get_bit_depth(png_read, info);

    std::vector<png_bytep> row_pointers(height);
    for (int y = 0; y < height; y++)
    {
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png_read, info));
        if (!row_pointers[y])
        {
            std::cout << "Memory allocation failed for row " << y << std::endl;
            for (int j = 0; j < y; j++)
            {
                free(row_pointers[j]);
            }
            png_destroy_read_struct(&png_read, &info, nullptr);
            fclose(image);
            return;
        }
    }
    png_read_image(png_read, row_pointers.data());

    for (int y = 0; y < height; y++)
    {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++)
        {
            png_bytep pixel = &(row[x * 4]);

            int colorWeight = (static_cast<int>(pixel[0]) + static_cast<int>(pixel[1]) + static_cast<int>(pixel[2])) / 3; // integer division to floor the color average

            // std::cout << std::left
            //           << "Pixel at (" << x << ", " << y << "): "
            //           << "RGB= ("
            //           << std::setw(4) << std::to_string(static_cast<int>(pixel[0])) << " "
            //           << std::setw(4) << std::to_string(static_cast<int>(pixel[1])) << " "
            //           << std::setw(4) << std::to_string(static_cast<int>(pixel[2])) << ") "
            //           << "Grayscale-shift constant: " << colorWeight << std::endl;

            pixel[0] = static_cast<png_byte>(colorWeight);
            pixel[1] = static_cast<png_byte>(colorWeight);
            pixel[2] = static_cast<png_byte>(colorWeight);
        }
    }

    png_init_io(png_write, outputImage);
    png_set_IHDR(png_write, info, width, height, bit_depth, color_type,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_write, info);
    png_write_image(png_write, row_pointers.data());
    png_write_end(png_write, nullptr);

    png_destroy_read_struct(&png_read, &info, nullptr);

    png_destroy_write_struct(&png_write, &info);
    fclose(outputImage);
    png_destroy_read_struct(&png_read, &info, nullptr);
    fclose(image);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <image_file_path>" << std::endl;
        return 1;
    }

    std::string imageFile = argv[1];

    std::cout << "Image file specified: " << imageFile << std::endl;

    displayFile("testImages/" + imageFile);
    return 0;
}
