#include "funcs.h"
#include <SFML/Graphics.hpp>

void cohenSutherland(sf::Vector2f& p1, sf::Vector2f& p2, sf::Vector2f min, sf::Vector2f max)
{

    sf::RenderWindow window(sf::VideoMode(800, 600), "Cohen-Sutherland Algorithm");

    window.clear();

    const int LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8;

    int code1 = 0, code2 = 0;

    if (p1.x < min.x)
        code1 |= LEFT;
    else if (p1.x > max.x)
        code1 |= RIGHT;

    if (p1.y < min.y)
        code1 |= BOTTOM;
    else if (p1.y > max.y)
        code1 |= TOP;

    if (p2.x < min.x)
        code2 |= LEFT;
    else if (p2.x > max.x)
        code2 |= RIGHT;

    if (p2.y < min.y)
        code2 |= BOTTOM;
    else if (p2.y > max.y)
        code2 |= TOP;

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

                drawLineBresenham(window, p1, { x, y }, sf::Color::Red);

                p1.x = x;
                p1.y = y;
                code1 = 0;
                if (p1.x < min.x)
                    code1 |= LEFT;
                else if (p1.x > max.x)
                    code1 |= RIGHT;

                if (p1.y < min.y)
                    code1 |= BOTTOM;
                else if (p1.y > max.y)
                    code1 |= TOP;
            } else {
                drawLineBresenham(window, { x, y }, p2, sf::Color::Red);
                p2.x = x;
                p2.y = y;
                code2 = 0;
                if (p2.x < min.x)
                    code2 |= LEFT;
                else if (p2.x > max.x)
                    code2 |= RIGHT;

                if (p2.y < min.y)
                    code2 |= BOTTOM;
                else if (p2.y > max.y)
                    code2 |= TOP;
            }
        }
    }

    // Рисуем рамку кадрирования
    sf::RectangleShape rect;
    rect.setPosition(min);
    rect.setSize(max - min);
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineThickness(1);
    rect.setOutlineColor(sf::Color::Green);
    window.draw(rect);

    // drawLineBresenham(window, p1, p2, sf::Color::White);

    window.display();

    while (window.isOpen()) {

        // Event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }
}

int main()
{
    sf::Vector2f p1(0, 0);
    sf::Vector2f p2(300, 250);
    sf::Vector2f min(150, 100);
    sf::Vector2f max(250, 200);

    cohenSutherland(p1, p2, min, max);

    return 0;
}
