#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#include "funcs.h"

const int SCALE = 30;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

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

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lab 3 - Variant 1");
    window.clear(sf::Color::White);

    // drawStar(window, sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), 70);

    // for (int h = 10; h <= 1000; h += 10) {
    //     drawStar(window, sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), h);
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //     // floodFill(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, sf::Color(255, 255, 255), sf::Color(255, 255, 0, 50));

    //     window.display();
    // }

    for (int h = 280; h >= 10; h -= 20) {
        drawStar(window, sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), h, sf::Color::Black);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        floodFill(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, sf::Color(255, 255, 255), sf::Color(255, (sf::Uint8)std::min(255, h), 0, 50));

        // window.display();
    }

    window.display();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }

    return 0;
}