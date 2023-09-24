// Computer Graphics Lab 1
// Crew: Kostylev Maleev Zverev
// var 3

#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

static bool gamePause = false;

class button {
public:
   // button(int x, int y, int w, int h) : pos_x(x), pos_y(y), size_x(w), size_y(h) {};
    virtual void clicked() = 0;
    sf::Shape* shape_ptr;
    int pos_x{};
    int pos_y{};
    int size_x{};
    int size_y{};

private:
};

class resumeButton : public button {
public:
    resumeButton(int x, int y, int w, int h) /* : pos_x(x), pos_y(y), size_x(w), size_y(h)*/ {
        pos_x = x;
        pos_y = (y);
        size_x = (w);
        size_y = (h);
        shape_ptr = new sf::RectangleShape(sf::Vector2f(w,h));

    }
    void clicked() override {
        gamePause = false;

    }

};

int main()
{
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH,WINDOW_HEIGHT }), "lab1");

    float circleRadius{ 40 };
    sf::CircleShape cr1(circleRadius);
    cr1.setOutlineThickness(5);
    
    cr1.setOutlineColor(sf::Color::Yellow);
    cr1.setPosition({ 0,0 });
    cr1.setFillColor(sf::Color(0xFF, 0, 0));
    sf::Vector2f circlePos{};

    sf::Clock timer;

    constexpr float circleSpeed{ 100.f };
    float deltaTime{};

    int x_dir = 1;
    int y_dir = 1;
    float radIncr = 1;
    while (window.isOpen()) {
     
        sf::Event ev;
        window.clear(sf::Color::Blue);
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed) window.close();
            if (ev.type == sf::Event::KeyReleased) {
                if (ev.key.code == sf::Keyboard::Escape)
                {
                    gamePause = !gamePause;
                }
            }
        }
        if (gamePause) {
            timer.restart();
        }
        else {
            deltaTime = timer.restart().asSeconds();
            circlePos = cr1.getPosition();
            circlePos.x += x_dir * circleSpeed * deltaTime; // передвижение 
            circleRadius += 10 * radIncr * deltaTime;

            if (circleRadius >= 100 || circleRadius <= 10) radIncr *= -1;
            cr1.setRadius(circleRadius);

            if (circlePos.x + 2 * circleRadius >= WINDOW_WIDTH || circlePos.x <= 0) { // отскоки
                x_dir *= -1;
            }
            cr1.setPosition(circlePos);

        }
        window.draw(cr1);

        window.display();
    }
}
