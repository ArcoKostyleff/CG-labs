#pragma once

#include "io.h"
#include "structs.h"
#include <filesystem>
#include <iostream>

Image upscale(const Image &image, uint16_t upscaleX, uint16_t upscaleY) {
    Image scaledImage{
        FileHeader{static_cast<uint16_t>(image.header.width * upscaleX),
                   static_cast<uint16_t>(image.header.height * upscaleY),
                   image.header.bitsPerPixel, image.header.paletteSize},
        image.palette,
        std::vector<Pixel>(image.header.width * image.header.height * upscaleX *
                           upscaleY)};

    for (int y = 0; y < image.header.height; y++) {
        for (int x = 0; x < image.header.width; x++) {
            Pixel currentPixel = image.pixels[y * image.header.width + x];
            for (int y_fill = 0; y_fill < upscaleY; y_fill++) {

                for (int x_fill = 0; x_fill < upscaleX; x_fill++) {
                    scaledImage.pixels[(y * upscaleY + y_fill) *
                                           scaledImage.header.width * 1 +
                                       (x * upscaleX + x_fill)] = currentPixel;
                }
            }
        }
    }

    return scaledImage;
}

void upscale(const Image &image, uint16_t upscaleX, uint16_t upscaleY,
             std::ofstream &outputFile) {
    writeImage(outputFile, upscale(image, upscaleX, upscaleY));
}

void upscaleWithPrompt() {
    std::cout << "Cwd: " << std::filesystem::current_path() << std::endl;
    std::cout << "Enter image path to upscale... ";
    std::string imagePath;
    std::cin >> imagePath;

    std::ifstream inputFile(imagePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cout << "Error opening\n";
        return;
    }

    Image image = readImage(inputFile);
    std::cout << "Original size: " << image.header.width << "x"
              << image.header.height << std::endl;

    uint16_t upscaleX, upscaleY;
    std::cout << "Enter upscale x and y factors: ";
    std::cin >> upscaleX >> upscaleY;

    std::cout << "Enter output path... ";
    std::string outputPath;
    std::cin >> outputPath;
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cout << "Error opening\n";
        return;
    }

    upscale(image, upscaleX, upscaleY, outputFile);
    std::cout << "Success!\n";
}