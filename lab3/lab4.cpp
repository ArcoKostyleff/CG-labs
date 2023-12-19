#include "funcs.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <thread>

const int SCALE = 15;
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
            y = 15 - y;
            polygon.points.push_back(sf::Vector2f(x * scale + shiftX, y * scale + shiftY));
        }
        polygons.push_back(polygon);
    }
    in.close();
    return polygons;
}

const int LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8;
int calculateCode(sf::Vector2f& p, sf::Vector2f& min, sf::Vector2f& max)
{
    int code = 0;
    if (p.x < min.x)
        code |= LEFT;
    else if (p.x > max.x)
        code |= RIGHT;

    if (p.y < min.y)
        code |= BOTTOM;
    else if (p.y > max.y)
        code |= TOP;

    return code;
}
void cohenSutherland(sf::RenderWindow& window, sf::Vector2f p1,
    sf::Vector2f p2, sf::Vector2f min, sf::Vector2f max,
    sf::Color color, bool drawCropped = false)
{

    int code1 = 0, code2 = 0;

    code1 = calculateCode(p1, min, max);
    code2 = calculateCode(p2, min, max);

    bool accept = false;

    while (true) {
        if (code1 == 0 && code2 == 0) {
            // Обе точки внутри прямоугольника отсечения
            accept = true;
            break;
        } else if (code1 & code2) {
            // Обе точки снаружи одной из сторон прямоугольника отсечения
            break;
        } else {
            // Определение и отсечение точки снаружи прямоугольника отсечения
            int codeOut = code1 ? code1 : code2;
            float x, y;

            if (codeOut & TOP) {
                // Начало прямой + её длина по Х * расстояние от начала прямой до верха кадрирования / длина прямой по Y
                // через подобие тругольников считаем где закончится X
                x = p1.x + (p2.x - p1.x) * (max.y - p1.y) / (p2.y - p1.y);
                y = max.y;
            } else if (codeOut & BOTTOM) {
                x = p1.x + (p2.x - p1.x) * (min.y - p1.y) / (p2.y - p1.y);
                y = min.y;
            } else if (codeOut & RIGHT) {
                y = p1.y + (p2.y - p1.y) * (max.x - p1.x) / (p2.x - p1.x);
                x = max.x;
            } else if (codeOut & LEFT) {
                y = p1.y + (p2.y - p1.y) * (min.x - p1.x) / (p2.x - p1.x);
                x = min.x;
            }

            if (codeOut == code1) {
                if (drawCropped)
                    drawLineBresenham(window, p1, { x, y }, sf::Color::Red);

                p1.x = x;
                p1.y = y;
                code1 = 0;

                code1 = calculateCode(p1, min, max);

            } else {
                if (drawCropped)
                    drawLineBresenham(window, { x, y }, p2, sf::Color::Red);

                p2.x = x;
                p2.y = y;
                code2 = 0;

                code2 = calculateCode(p2, min, max);
            }
        }
    }

    if (accept)
        drawLineBresenham(window, p1, p2, color);
    else if (drawCropped)
        drawLineBresenham(window, p1, p2, sf::Color::Red);
}

// Отрисовка многоугольника
void drawPolygonCropped(sf::RenderWindow& window, std::vector<sf::Vector2f>& points, sf::Vector2f min, sf::Vector2f max, sf::Color color, bool drawCropped = false)
{
    // Соединяем точки между собой
    for (size_t i = 0; i < points.size() - 1; i++) {
        cohenSutherland(window, points[i], points[i + 1], min, max, color, drawCropped);
    }
    // Соединяем первую и последнюю точку
    cohenSutherland(window, points.back(), points.front(), min, max, color, drawCropped);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lab4");
    window.clear(sf::Color::Black);
    auto polygons = readPolygonsFromFile("../pollygons.txt", SCALE, WINDOW_WIDTH / 10, WINDOW_HEIGHT / 10);

    sf::Vector2f p1(0, 0);
    sf::Vector2f p2(300, 250);
    sf::Vector2f min(150, 100);
    sf::Vector2f max(250, 200);
    for (int i = 0; i < 100; i++) {
        window.clear();
        max.y++;

        for (auto& polygon : polygons) {
            // break;
            std::cout << "Draw pollygon" << std::endl;
            std::cout << "Points count: " << polygon.points.size() << std::endl;
            std::cout << "Color: " << (int)polygon.color.r << " " << (int)polygon.color.g << " " << (int)polygon.color.b << std::endl;
            for (auto& point : polygon.points)
                std::cout << '(' << point.x << "; " << point.y << ')' << std::endl;
            std::cout << std::endl;
            // window.clear();
            drawLineDDA(window, { min.x, min.y }, { max.x, min.y }, sf::Color::Green);
            drawLineDDA(window, { max.x, min.y }, { max.x, max.y }, sf::Color::Green);
            drawLineDDA(window, { max.x, max.y }, { min.x, max.y }, sf::Color::Green);
            drawLineDDA(window, { min.x, max.y }, { min.x, min.y }, sf::Color::Green);

            // drawPolygon(window, polygon.points, sf::Color::Cyan);
            drawPolygonCropped(window, polygon.points, min, max, polygon.color, false);
            // window.display();

            // auto point = getPointInsidePolygon(polygon.points);
            // floodFill(window, point.x, point.y, polygon.color, polygon.color);
            // drawPolygon(window, polygon.points, sf::Color::Magenta);
            // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        drawLineDDA(window, { min.x, min.y }, { max.x, min.y }, sf::Color::Green);
        drawLineDDA(window, { max.x, min.y }, { max.x, max.y }, sf::Color::Green);
        drawLineDDA(window, { max.x, max.y }, { min.x, max.y }, sf::Color::Green);
        drawLineDDA(window, { min.x, max.y }, { min.x, min.y }, sf::Color::Green);

        // cohenSutherland(window, p1, p2, min, max, sf::Color::Blue, true);

        window.display();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    while (window.isOpen()) {

        // Event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }

    return 0;
}
