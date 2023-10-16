#pragma once
#include "io.h"
#include "structs.h"
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