#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#include "funcs.h"

const int SCALE = 30;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct Polygon {
    sf::Color color;
    std::vector<sf::Vector2f> points;
};

std::vector<Polygon> readPolygonsFromFile(const std::string& filename, int scale = 1, int shiftX = 0, int shiftY = 0) {
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


int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lab 3 - А6");
    window.clear(sf::Color::Black);
    Polygon polygon;
    polygon.points = {{1, 7}, {0, 10}, {-1, 11}, {-2, 10}, {0, 7}, {-2, 5}, {-7, 3}, {-8, 0}, {-9, 1}, {-9, 0}, {-7, -2}, {-2, -2}, {-3, -1}, {-4, -1}, {-1, 3}, {0, -2}, {1, -2}, {0, 0}, {0, 3}, {1, 4}, {2, 4}, {3, 5}, {2, 6}, {1, 9}, {0, 10}};

    for (auto& point : polygon.points)
        point.x *= SCALE;
    for (auto& point : polygon.points) {
        point.y = 11 - point.y;
        point.y *= SCALE;
    }

    for (auto& point : polygon.points)
        point.x += 500;
    for (auto& point : polygon.points)
        point.y += 200;

    for (auto &point : polygon.points) {
        std::cout << "Draw pollygon" << std::endl;
        std::cout << "Points count: " << polygon.points.size() << std::endl;
        for (auto& point : polygon.points)
            std::cout << '(' << point.x << "; " << point.y << ')' << std::endl;
        std::cout << std::endl;
    }

    drawPolygon(window, polygon.points, sf::Color::White);
    window.display();
    modifiedStackFloodFillLinearGradient(window, 300, 570, 300, sf::Color::Red, sf::Color::Blue);
    std::cout << "DONE!!!" << std::endl;

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