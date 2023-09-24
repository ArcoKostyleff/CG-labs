// Computer Graphics Lab 1
// Crew: Kostylev Maleev Zverev
// var 3

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Shape.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include <deque>
#include <math.h>

static bool gamePause = false;

class button {
  public:
    // button(int x, int y, int w, int h) : pos_x(x), pos_y(y), size_x(w),
    // size_y(h) {};
    virtual void clicked() = 0;
    sf::Shape *shape_ptr;
    int pos_x{};
    int pos_y{};
    int size_x{};
    int size_y{};

  private:
};

class resumeButton : public button {
  public:
    resumeButton(int x, int y, int w,
                 int h) /* : pos_x(x), pos_y(y), size_x(w), size_y(h)*/ {
        pos_x = x;
        pos_y = (y);
        size_x = (w);
        size_y = (h);
        shape_ptr = new sf::RectangleShape(sf::Vector2f(w, h));
    }
    void clicked() override { gamePause = false; }
};

int main() {
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
                            "lab1");

    float circleRadius{40};
    sf::CircleShape cr1(circleRadius);
    cr1.setOutlineThickness(5);

    cr1.setOutlineColor(sf::Color::Yellow);
    cr1.setPosition({0, 0});
    cr1.setFillColor(sf::Color(0xFF, 0, 0));

    sf::Vector2f circlePos{};

    sf::Clock timer;

    constexpr float circleSpeedX{100.f};
    constexpr float circleSpeedY{50.f};
    float deltaTime{};

    int x_dir = 1;
    int y_dir = 1;
    float radIncr = 1;
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
            circlePos.y += y_dir * circleSpeedY * deltaTime; // передвижение
            circleRadius += 10 * radIncr * deltaTime;
            circlePos.y -= 10 * radIncr * deltaTime;

            if (circleRadius >= 100 || circleRadius <= 10)
                radIncr *= -1;
            cr1.setRadius(circleRadius);

            if (circlePos.x + 2 * circleRadius >= WINDOW_WIDTH)
                x_dir = -1;
            else if (circlePos.x <= 0) // отскоки
                x_dir = 1;

            if (circlePos.y + 2 * circleRadius >= WINDOW_HEIGHT)
                y_dir = -1;
            else if (circlePos.y <= 0) // отскоки
                y_dir = 1;

            cr1.setPosition(circlePos);
        }

        window.draw(cr1);

        window.display();
    }
}
