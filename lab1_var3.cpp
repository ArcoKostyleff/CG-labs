

// Computer Graphics Lab 1
// Crew: Kostylev Maleev Zverev
// var 3

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Shape.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include <deque>
#include <iostream>
#include <math.h>

static bool gamePause = false;

int main() {
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
                            "lab1");

    float circleRadius{40};
    sf::CircleShape cr1(circleRadius);
    cr1.setOutlineThickness(5);

    cr1.setOutlineColor(sf::Color::Yellow);
    cr1.setPosition({30, 100});
    cr1.setFillColor(sf::Color(0xFF, 0, 0));

    sf::Vector2f circlePos;

    sf::Clock timer;

    constexpr float circleSpeedX = 30.f;
    float deltaTime;

    float radIncr = 1;
    while (window.isOpen()) {

        window.clear(sf::Color::Blue);

        deltaTime = timer.restart().asSeconds();
        circlePos = cr1.getPosition();
        circlePos.x += circleSpeedX * deltaTime; // передвижение
        circlePos.y -=
            10 * radIncr * deltaTime; // чтобы изменение размера шло из центра
        circlePos.x -=
            10 * radIncr * deltaTime; // чтобы изменение размера шло из центра
        circleRadius += 10 * radIncr * deltaTime;

        if (circleRadius >= 100 || circleRadius <= 10)
            radIncr *= -1;

        cr1.setRadius(circleRadius);
        cr1.setPosition(circlePos);

        window.draw(cr1);

        window.display();
    }
}
