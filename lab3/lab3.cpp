#include <SFML/Graphics.hpp>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <thread>

#include "funcs.h"

const int SCALE = 1;
const int WINDOW_WIDTH = 168;
const int WINDOW_HEIGHT = 168;

struct Polygon {
    sf::Color color;
    std::vector<sf::Vector2f> points;
};

std::vector<Polygon> readPolygonsFromFile(const std::string& filename, int scale = 1, int shiftX = 0, int shiftY = 0)
{
    std::ifstream in(filename);
    std::vector<Polygon> polygons;
    if (!in.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return polygons;
    }
    // Количество полигонов
    int polygonsCount;
    in >> polygonsCount;
    std::cout << "Polygons count: " << polygonsCount << std::endl;
    // Считываем полигоны
    for (int i = 0; i < polygonsCount; i++) {
        int pointsCount;
        int r, g, b;
        Polygon polygon;
        in >> pointsCount;
        in >> r >> g >> b;
        polygon.color.r = r;
        polygon.color.g = g;
        polygon.color.b = b;
        for (int j = 0; j < pointsCount; j++) {
            float x, y;
            in >> x >> y;
            polygon.points.push_back(sf::Vector2f(x * scale + shiftX, y * scale + shiftY));
        }
        polygons.push_back(polygon);
    }
    in.close();
    return polygons;
}

int test(int test_number, std::string algorithm, std::function<void(sf::RenderWindow&, const std::vector<sf::Vector2f>&, sf::Color)> drawFunc)
{
    std::cout << "Test #" << test_number << std::endl;
    std::cout << "Drawing " << algorithm << std::endl;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lab 3 - Variant 1");
    window.clear(sf::Color::Black);
    auto polygons = readPolygonsFromFile("../polygons" + std::to_string(test_number) + ".txt", SCALE, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    int polygonNumber = 1;

    for (auto& polygon : polygons) {
        window.clear();

        auto start = std::chrono::high_resolution_clock::now();
        for (int _ = 0; _ < 1000; _++)
            drawFunc(window, polygon.points, polygon.color);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start) / 1000;
        std::cout << duration.count()
                  << std::endl;

        auto point = getPointInsidePolygon(polygon.points);
        // floodFill(window, point.x, point.y, polygon.color, polygon.color);
        window.display();

        std::ofstream file;
        file.open("output" + std::to_string(test_number) + "_" + algorithm + "_" + std::to_string(polygonNumber) + ".txt");
        sf::Image screenshot = window.capture();
        sf::Vector2u imageSize = screenshot.getSize();

        for (unsigned int y = 0; y < imageSize.y; ++y) {
            for (unsigned int x = 0; x < imageSize.x; ++x) {
                sf::Color pixelColor = screenshot.getPixel(x, y);
                if (pixelColor == sf::Color::Black) {
                    file << "0";
                } else {
                    file << "1";
                }
            }
            file << "\n";
        }
        file.close();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        polygonNumber++;
    }

    window.display();

    return 0;
}

int testFill(std::string algorithm, /* fill func */ std::function<void(sf::RenderWindow&, int, int, sf::Color, sf::Color)> fillFunc)
{
    std::cout << "Drawing " << algorithm << std::endl;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lab 3 - Variant 1");
    window.clear(sf::Color::Black);
    auto polygons = readPolygonsFromFile("../surface1.txt");
    int polygonNumber = 1;

    for (auto& polygon : polygons) {
        window.clear();

        auto point = getPointInsidePolygon(polygon.points);

        double sumDurations = 0;
        for (int _ = 0; _ < 600; _++) {
            if (_ == 100)
                sumDurations = 0;
            window.clear(sf::Color::Black);
            drawPolygonA(window, polygon.points, polygon.color);
            auto start = std::chrono::high_resolution_clock::now();
            fillFunc(window, point.x, point.y, polygon.color, polygon.color);
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            sumDurations += duration.count();
        }
        std::cout << sumDurations / 500
                  << std::endl;

        window.display();

        std::ofstream file;
        file.open("output1_" + algorithm + "_" + std::to_string(polygonNumber) + ".txt");
        sf::Image screenshot = window.capture();
        sf::Vector2u imageSize = screenshot.getSize();

        for (unsigned int y = 0; y < imageSize.y; ++y) {
            for (unsigned int x = 0; x < imageSize.x; ++x) {
                sf::Color pixelColor = screenshot.getPixel(x, y);
                if (pixelColor == sf::Color::Black) {
                    file << "0";
                } else {
                    file << "1";
                }
            }
            file << "\n";
        }
        file.close();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        polygonNumber++;
    }

    window.display();

    return 0;
}

int main()
{
    // for (int i = 1; i <= 7; i++) {
    //     test(i, "DDA", drawPolygonA);
    // }
    // for (int i = 1; i <= 7; i++) {
    //     test(i, "Bresenham", drawPolygonB);
    // }

    testFill("FloodFill", floodFill);
    testFill("StackFloodFill", modifiedStackFloodFill);
}
