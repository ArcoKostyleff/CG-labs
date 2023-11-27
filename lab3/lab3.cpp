#include <SFML/Graphics.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#include "criterion.hpp"
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

BENCHMARK(DDA)
{
    SETUP_BENCHMARK(
        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lab 3 - Variant 1");
        window.clear(sf::Color::Black);
        window.setVisible(false);
        auto polygons = readPolygonsFromFile("../pollygons.txt", SCALE, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);)

    for (auto& polygon : polygons) {
        std::cout << "Draw pollygon" << std::endl;
        std::cout << "Points count: " << polygon.points.size() << std::endl;
        std::cout << "Color: " << (int)polygon.color.r << " " << (int)polygon.color.g << " " << (int)polygon.color.b << std::endl;
        for (auto& point : polygon.points)
            std::cout << '(' << point.x << "; " << point.y << ')' << std::endl;
        std::cout << std::endl;

        drawPolygon(window, polygon.points, polygon.color);
        auto point = getPointInsidePolygon(polygon.points);
        // floodFill(window, point.x, point.y, polygon.color, polygon.color);
        // window.display();
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    TEARDOWN_BENCHMARK(

        window.clear(sf::Color::Black);
        // window.display();

        // while (window.isOpen()) {
        //     sf::Event event;
        //     while (window.pollEvent(event)) {
        //         if (event.type == sf::Event::Closed) {
        //             window.close();
        //         }
        //     }
        // }
    )

    // // Отрисовка координатной прямой посередине экрана
    // // Создание вертикальной линии
    // sf::Vertex verticalLine[] =
    // {
    //     sf::Vertex(sf::Vector2f(window.getSize().x / 2, 0), sf::Color::White),
    //     sf::Vertex(sf::Vector2f(window.getSize().x / 2, window.getSize().y), sf::Color::White)
    // };
    // // Создание горизонтальной линии
    // sf::Vertex horizontalLine[] =
    // {
    //     sf::Vertex(sf::Vector2f(0, window.getSize().y / 2), sf::Color::White),
    //     sf::Vertex(sf::Vector2f(window.getSize().x, window.getSize().y / 2), sf::Color::White)
    // };

    // window.draw(verticalLine, 2, sf::Lines);
    // window.draw(horizontalLine, 2, sf::Lines);

    // return 0;
}

CRITERION_BENCHMARK_MAIN()