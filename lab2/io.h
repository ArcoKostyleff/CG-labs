#pragma once
#include <bitset>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "structs.h"

// Функция для чтения заголовка из файла
FileHeader readHeader(std::ifstream &file) {
    FileHeader header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header));
    return header;
}

// Функция для записи заголовка в файл
void writeHeader(std::ofstream &file, const FileHeader &header) {
    file.write(reinterpret_cast<const char *>(&header), sizeof(header));
}

// Функция для чтения палитры из файла
std::vector<Color> readPalette(std::ifstream &file, uint16_t paletteSize = 16) {
    std::vector<Color> palette(paletteSize);
    for (int i = 0; i < paletteSize; ++i) {
        Color color;
        file.read(reinterpret_cast<char *>(&color), sizeof(Color));
        palette[i] = color;
    }
    return palette;
}

// Функция для записи палитры в файл
void writePalette(std::ofstream &file, const std::vector<Color> &palette) {
    for (const Color &color : palette) {
        file.write(reinterpret_cast<const char *>(&color), sizeof(Color));
    }
}

// Функция для чтения пикселей изображения
std::vector<Pixel> readPixels(std::ifstream &file, uint16_t width,
                              uint16_t height) {
    std::vector<Pixel> pixels(width * height);
    for (int i = 0; i < pixels.size(); i += 2) {
        uint8_t twoPixels;
        file.read(reinterpret_cast<char *>(&twoPixels), sizeof(twoPixels));
        pixels[i] = twoPixels >> 4;
        pixels[i + 1] = twoPixels & 0x0F;
    }
    return pixels;
}

// Функция для записи пикселей в файл
void writePixels(std::ofstream &file, const std::vector<Pixel> &pixels) {

    for (int i = 0; i < pixels.size() - 1; i += 2) {
        uint8_t twoPixels = (pixels[i] << 4) | pixels[i + 1];
        file.write(reinterpret_cast<const char *>(&twoPixels),
                   sizeof(twoPixels));
    }
    if (pixels.size() % 2 != 0) {
        uint8_t pixel = pixels.back();
        file.write(reinterpret_cast<const char *>(&pixel), sizeof(pixel));
    }
}

Image readImage(std::ifstream &file) {
    FileHeader header = readHeader(file);
    return {header, readPixels(file, header.width, header.height)};
}

void writeImage(std::ofstream &file, const Image &image) {
    writeHeader(file, image.header);
    writePixels(file, image.pixels);
}

// int main() {

//     // Чтение палитры из файла "palette.bin"
//     std::ifstream readPaletteFile("palette.bin", std::ios::binary);
//     if (!readPaletteFile) {
//         std::cerr << "Failed to open palette file for reading." << std::endl;
//         return 1;
//     }

//     std::vector<Color> palette = readPalette(readPaletteFile);
//     std::cout << "Palette:\n";
//     for (auto c : palette) {
//         std::cout << (int)c.alpha << " " << (int)c.red << " " << (int)c.green
//                   << " " << (int)c.blue << std::endl;
//     }

//     FileHeader header;
//     header.width = 12;
//     header.height = 10;
//     header.bitsPerPixel = 4;
//     header.paletteSize = 16;

//     std::vector<Pixel> pixels(header.width * header.height);

//     {
//         std::ofstream outputFile("image1.bin", std::ios::binary);
//         writeHeader(outputFile, header);

//         int i = 1;
//         for (auto &p : pixels) {
//             p = i++ % 16;
//         }
//         writePixels(outputFile, pixels);
//     }

//     std::ifstream inputFile("image1.bin", std::ios::binary);

//     std::ofstream outputFile2("image1_echo.bin", std::ios::binary);
//     Image imageFromFile = readImage(inputFile);
//     writeImage(outputFile2, imageFromFile);
//     assert(header.width == imageFromFile.header.width);
//     assert(header.height == imageFromFile.header.height);
//     assert(header.bitsPerPixel == imageFromFile.header.bitsPerPixel);
//     assert(header.paletteSize == imageFromFile.header.paletteSize);
//     assert(pixels == imageFromFile.pixels);

//     return 0;
// }
