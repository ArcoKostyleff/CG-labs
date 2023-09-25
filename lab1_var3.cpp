
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

template <typename Shape>
void deleteTransparentTraces(std::deque<Shape> &shapes) {
    while (shapes.size() &&
           (shapes[0].getFillColor().toInteger() & 0xFF) == 0) {
        shapes.erase(shapes.begin());
    }
}

double opacityDecreased = 0;
template <typename Shape>
void decreaseOpacity(std::deque<Shape> &shapes, int reduceBy) {
    opacityDecreased += reduceBy;
    for (int i = 0; i < shapes.size(); i++) {
        int opacity = shapes[i].getFillColor().toInteger() & 0xFF;
        opacity -= reduceBy;
        opacity = std::max(opacity, 0);

        shapes[i].setFillColor(sf::Color(
            shapes[i].getFillColor().toInteger() & 0xFFFFFF00 | opacity));
        shapes[i].setOutlineColor(sf::Color(
            shapes[i].getOutlineColor().toInteger() & 0xFFFFFF00 | opacity));
    }
}

int main() {
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
    constexpr int TRACES_SPAWN_RATE =
        30; // чем больше, тем реже появляются следы
    constexpr float TRACE_LIFETIME_SECONDS = 0.25f;
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
                            "lab1");

    float circleRadius{40};
    sf::CircleShape cr1(circleRadius);
    cr1.setOutlineThickness(5);

    cr1.setOutlineColor(sf::Color::Yellow);
    cr1.setPosition({0, 0});
    cr1.setFillColor(sf::Color(0xFF, 0, 0));

    std::deque<sf::CircleShape> traces;

    sf::Vector2f circlePos{};

    sf::Clock timer;

    constexpr float circleSpeedX{100.f};
    constexpr float circleSpeedY{50.f};
    float deltaTime{};

    int x_dir = 1;
    float radIncr = 1;
    int bufferFramesToSkip = TRACES_SPAWN_RATE;
    float timeBeforeReducingOpacity = 0;
    while (window.isOpen()) {

        sf::Event ev;
        window.clear(sf::Color::Blue);
        window.draw(cr1);
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed)
                window.close();
            if (ev.type == sf::Event::KeyReleased) {
                if (ev.key.code == sf::Keyboard::Escape) {
                    gamePause = !gamePause;
                }
            }
        }
        if (gamePause) {
            timer.restart();
        } else {
            deltaTime = timer.restart().asSeconds();
            circlePos = cr1.getPosition();
            circlePos.x += x_dir * circleSpeedX * deltaTime; // передвижение
            circleRadius += 10 * radIncr * deltaTime;

            if (circleRadius >= 100 || circleRadius <= 10)
                radIncr *= -1;
            cr1.setRadius(circleRadius);

            if (circlePos.x + 2 * circleRadius >= WINDOW_WIDTH)
                x_dir = -1;
            else if (circlePos.x <= 0) // отскоки
                x_dir = 1;

            circlePos.y = sin(circlePos.x / WINDOW_WIDTH * 20) * 200 +
                          WINDOW_HEIGHT / 2 - circleRadius;

            cr1.setPosition(circlePos);
        }

        // Угасающие следы фигуры
        if (bufferFramesToSkip == 1) {
            traces.push_back(cr1);
        }
        timeBeforeReducingOpacity +=
            deltaTime * (1 / TRACE_LIFETIME_SECONDS) * 255;
        decreaseOpacity(traces, timeBeforeReducingOpacity);
        timeBeforeReducingOpacity -= (int)timeBeforeReducingOpacity;
        deleteTransparentTraces(traces);
        for (int i = 0; i < traces.size(); i++) {
            window.draw(traces[i]);
        }
        // Сбрасываем счетчик, если нужно
        if (--bufferFramesToSkip == 0) {
            bufferFramesToSkip = TRACES_SPAWN_RATE;
        }

        window.draw(cr1);

        window.display();
    }
}
