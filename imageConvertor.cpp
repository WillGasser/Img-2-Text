// g++ -o run.exe main.cpp -lpng
#include <iostream>
#include <vector>
#include <string>
#include <png.h>
#include <iomanip>
#include <cstdio>

void convertGrayScale(FILE *image, png_structp png_read, png_infop info, std::vector<png_bytep> &row_pointers)
{
    png_init_io(png_read, image);
    png_read_info(png_read, info);

    int width = png_get_image_width(png_read, info);
    int height = png_get_image_height(png_read, info);
    png_byte color_type = png_get_color_type(png_read, info);
    png_byte bit_depth = png_get_bit_depth(png_read, info);

    row_pointers.resize(height);
    for (int y = 0; y < height; y++)
    {
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png_read, info));
        if (!row_pointers[y])
        {
            std::cerr << "Memory allocation failed for row " << y << std::endl;
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

            pixel[0] = static_cast<png_byte>(colorWeight);
            pixel[1] = static_cast<png_byte>(colorWeight);
            pixel[2] = static_cast<png_byte>(colorWeight);
        }
    }
}

void outputGrayscale(const std::string &fileName)
{
    FILE *image = fopen(fileName.c_str(), "rb");
    if (image == nullptr)
    {
        std::cerr << "Error opening file" << std::endl;
        return;
    }

    png_structp png_read = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_read)
    {
        std::cerr << "Error creating PNG read struct" << std::endl;
        fclose(image);
        return;
    }

    png_infop info = png_create_info_struct(png_read);
    if (!info)
    {
        std::cerr << "Error creating PNG info struct" << std::endl;
        png_destroy_read_struct(&png_read, nullptr, nullptr);
        fclose(image);
        return;
    }

    if (setjmp(png_jmpbuf(png_read)))
    {
        std::cerr << "Error during PNG read" << std::endl;
        png_destroy_read_struct(&png_read, &info, nullptr);
        fclose(image);
        return;
    }

    // Allocate row pointers and convert to grayscale
    std::vector<png_bytep> row_pointers;
    convertGrayScale(image, png_read, info, row_pointers);

    // Prepare to write to output file
    FILE *outputImage = fopen("output.png", "wb");
    if (!outputImage)
    {
        std::cerr << "Error opening output file for writing" << std::endl;
        png_destroy_read_struct(&png_read, &info, nullptr);
        for (auto row : row_pointers)
            free(row);
        fclose(image);
        return;
    }

    png_structp png_write = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_write)
    {
        std::cerr << "Error creating PNG write struct" << std::endl;
        fclose(outputImage);
        png_destroy_read_struct(&png_read, &info, nullptr);
        for (auto row : row_pointers)
            free(row);
        fclose(image);
        return;
    }

    png_infop info_write = png_create_info_struct(png_write);
    if (!info_write)
    {
        std::cerr << "Error creating PNG info struct for writing" << std::endl;
        png_destroy_write_struct(&png_write, nullptr);
        fclose(outputImage);
        png_destroy_read_struct(&png_read, &info, nullptr);
        for (auto row : row_pointers)
            free(row);
        fclose(image);
        return;
    }

    if (setjmp(png_jmpbuf(png_write)))
    {
        std::cerr << "Error during PNG write" << std::endl;
        png_destroy_write_struct(&png_write, &info_write);
        fclose(outputImage);
        png_destroy_read_struct(&png_read, &info, nullptr);
        for (auto row : row_pointers)
            free(row);
        fclose(image);
        return;
    }

    png_init_io(png_write, outputImage);

    int width = png_get_image_width(png_read, info);
    int height = png_get_image_height(png_read, info);
    int bit_depth = png_get_bit_depth(png_read, info);
    int color_type = png_get_color_type(png_read, info);

    png_set_IHDR(png_write, info_write, width, height, bit_depth, color_type,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_write, info_write);
    png_write_image(png_write, row_pointers.data());
    png_write_end(png_write, nullptr);

    // Cleanup
    png_destroy_read_struct(&png_read, &info, nullptr);
    png_destroy_write_struct(&png_write, &info_write);
    fclose(image);
    fclose(outputImage);
    for (auto row : row_pointers)
        free(row);
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

    outputGrayscale("testImages/" + imageFile);
    return 0;
}
