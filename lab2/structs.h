#pragma once
#include <bitset>
#include <cstdint>
#include <vector>

// Структура для хранения ARGB-цвета (32-битный цвет)
struct Color {
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// Структура для хранения заголовка файла
#pragma pack(push, 1)
struct FileHeader {
    uint16_t width;  // Ширина изображения в пикселях
    uint16_t height; // Высота изображения в пикселях
    uint8_t bitsPerPixel; // Количество бит на пиксель
    uint16_t paletteSize; // Количество значений в палитре
};
#pragma pack(pop)

typedef uint8_t Pixel;

typedef std::vector<Color> Palette;

// Структура для хранения заголовка файла
#pragma pack(push, 1)
struct Image {
    FileHeader header;
    std::vector<Pixel> pixels;
};
#pragma pack(pop)