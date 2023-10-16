#pragma once

#include "io.h"
#include "structs.h"
#include <algorithm>
#include <filesystem>
#include <iostream>

Image replaceColor(Image image, Pixel from, Pixel to) {
    std::replace(image.pixels.begin(), image.pixels.end(), from, to);
    return image;
}

void replaceColor(const Image &image, Pixel from, Pixel to,
                  std::ofstream &outputFile) {
    writeImage(outputFile, replaceColor(image, from, to));
}

void replaceColorWithPrompt() {
    std::cout << "Cwd: " << std::filesystem::current_path() << std::endl;
    std::cout << "Enter image path to replace color... ";
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

    int from, to;
    do {
        std::cout << "Enter replaced and replacer color id... ";
        std::cin >> from >> to;
    } while (from > 16 || to > 16);

    std::cout << "Enter output path... ";
    std::string outputPath;
    std::cin >> outputPath;
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cout << "Error opening\n";
        return;
    }

    replaceColor(image, from, to, outputFile);
    std::cout << "Success!\n";
}