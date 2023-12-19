#include "funcs.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <fstream>
#include <iostream>
#include <thread>

const int SCALE = 15;
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 300;

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
void cohenSutherland(sf::Image& window, sf::Vector2f p1,
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
void drawPolygonCropped(sf::Image& window, std::vector<sf::Vector2f> points, sf::Vector2f min, sf::Vector2f max, sf::Color color, bool drawCropped = false)
{
    // Соединяем точки между собой
    for (size_t i = 0; i < points.size() - 1; i++) {
        cohenSutherland(window, points[i], points[i + 1], min, max, color, drawCropped);
    }
    // Соединяем первую и последнюю точку
    cohenSutherland(window, points.back(), points.front(), min, max, color, drawCropped);
}

sf::Vector2f min(150, 100);
sf::Vector2f max(250, 200);

void drawFox(sf::RenderWindow& window, std::vector<Polygon>& polygons)
{
    window.clear();
    std::vector<sf::Image> layers;
    for (auto& polygon : polygons) {

        sf::Image currentLayer;
        currentLayer.create(window.getSize().x, window.getSize().y, sf::Color::Transparent);

        polygon.color.a = 160;

        drawPolygonCropped(currentLayer, polygon.points, min, max, sf::Color::Black, true);

        auto middlePoint = getPointInsidePolygon(polygon.points);
        modifiedStackFloodFill(currentLayer, middlePoint.x, middlePoint.y, polygon.color, min, max);

        layers.push_back(currentLayer);
    }

    sf::Image image = layers[0];

    for (int i = 1; i < layers.size(); i++) {
        for (int y = 0; y < image.getSize().y; y++) {
            for (int x = 0; x < image.getSize().x; x++) {

                // Смешиваем цвета с учетом альфа-канала
                sf::Color blendedColor;
                sf::Color newColor = layers[i].getPixel(x, y);
                sf::Color color = image.getPixel(x, y);
                blendedColor.r = (newColor.r * newColor.a + color.r * (255 - newColor.a)) / 255.0;
                blendedColor.g = (newColor.g * newColor.a + color.g * (255 - newColor.a)) / 255.0;
                blendedColor.b = (newColor.b * newColor.a + color.b * (255 - newColor.a)) / 255.0;

                image.setPixel(x, y, blendedColor);
            }
        }
    }

    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);
    window.draw(sprite);

    drawLineDDA(window, { min.x, min.y }, { max.x, min.y }, sf::Color::Green);
    drawLineDDA(window, { max.x, min.y }, { max.x, max.y }, sf::Color::Green);
    drawLineDDA(window, { max.x, max.y }, { min.x, max.y }, sf::Color::Green);
    drawLineDDA(window, { min.x, max.y }, { min.x, min.y }, sf::Color::Green);

    window.display();
}
int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lab4");
    window.clear(sf::Color::Black);
    auto polygons = readPolygonsFromFile("../pollygons.txt", SCALE, WINDOW_WIDTH / 10, WINDOW_HEIGHT / 10);

    drawFox(window, polygons);

    window.display();

    bool isDragging = false; // Флаг, указывающий, идет ли в данный момент перетаскивание

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseMoved:
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                min = mousePos - sf::Vector2f(30, 30);
                max = mousePos + sf::Vector2f(30, 30);
                drawFox(window, polygons);
                break;
            }
        }
    }

    return 0;
}
