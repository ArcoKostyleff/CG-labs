#include <SFML/Graphics.hpp>
#include <filesystem>
#include <fstream>
#include <future>
#include <vector>

#include "io.h"
#include "structs.h"

constexpr int virtualPixelSize = 30;

void displayImage(const Image &image) {
    sf::RenderWindow window(
        sf::VideoMode(image.header.width * virtualPixelSize,
                      image.header.height * virtualPixelSize),
        "SFML Image Display");

    // Создаем изображение и устанавливаем его пиксели
    sf::Image sfmlImage;
    sfmlImage.create(image.header.width, image.header.height);

    std::vector<sf::Color> sfmlColors;
    for (const Color &color : image.palette) {
        sfmlColors.emplace_back(color.red, color.green, color.blue,
                                color.alpha);
    }

    for (int y = 0; y < image.header.height; y++) {
        for (int x = 0; x < image.header.width; x++) {
            int pixelIndex = y * image.header.width + x;
            Pixel pixelValue = image.pixels[pixelIndex];
            sfmlImage.setPixel(x, y, sfmlColors[pixelValue]);
        }
    }

    // Создаем текстуру и спрайт
    sf::Texture texture;
    texture.loadFromImage(sfmlImage);
    sf::Sprite sprite(texture);

    // Устанавливаем масштаб спрайта
    sprite.setScale(static_cast<float>(virtualPixelSize),
                    static_cast<float>(virtualPixelSize));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}

void createImage() {
    Image img;
    img.header.bitsPerPixel = 4;
    img.header.width = 7;
    img.header.height = 6;
    img.header.paletteSize = 16;
    img.pixels = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                  0,   0,   0,   0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC,
                  0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 4,   4,   4,   4,   4,
                  4,   4,   4,   4,   4,   4,   4,   4,   4};

    std::ifstream paletteFile("../palette2.bin");

    img.palette = _readPalette(paletteFile);

    std::ofstream outputFile("../flagBig.bin", std::ios::binary);
    writeImage(outputFile, img);
}

int main() {
    // createImage();

    std::cout << "Cwd: " << std::filesystem::current_path() << std::endl;
    std::cout << "Enter image path to view... ";
    std::string imagePath;
    std::cin >> imagePath;    

    std::ifstream inputFile("../" + imagePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cout << "Error opening\n";
        return 1;
    }
    Image image = readImage(inputFile);
    std::cout << image.header.width << "x" << image.header.height << std::endl;

    Color startColor;
    std::cout << "Enter first shade color: ";
    std::cin >> startColor.red >> startColor.green >> startColor.blue;
    std::cout << "Enter diff: ";
    uint16_t diff;
    std::cin >> diff;
    Color replaceColor;
    std::cout << "Enter color to replace: ";
    std::cin >> replaceColor.red >> replaceColor.green >> replaceColor.blue;

    replaceImageHueToColor(image, startColor, diff, replaceColor);
    displayImage(image);

    return 0;
}
