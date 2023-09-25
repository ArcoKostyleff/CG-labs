// Computer Graphics Lab 1
// Crew: Kostylev Maleev Zverev
// var 3

#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Shape.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include <deque>
#include <math.h>
#include <unistd.h>

static bool gamePause = false;
static int FPS = 30;
static int defaultFPS = 15;

int main() {
    constexpr int WINDOW_WIDTH = 1024;
    constexpr int WINDOW_HEIGHT = 768;
    constexpr float FIGURE_SPEED = 20.f;
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
                            "lab1"/*, sf::Style::Fullscreen*/);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"))
    {
        throw std::runtime_error("bebra");
    }
    
    /* FPS Text */
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(30);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 10);

    /* Circle */
    float circleRadius{40};
    sf::CircleShape cr1(circleRadius);
    cr1.setOutlineThickness(5);
    cr1.setOutlineColor(sf::Color::Yellow);
    cr1.setPosition({0, 0});
    cr1.setFillColor(sf::Color(0xFF, 0, 0));
    sf::Vector2f circlePos{};

    /* Timers */
    sf::Clock timer;
    sf::Clock fpsTimer;

    float circleSpeedX{0.f};
    float circleSpeedY{0.f};
    float deltaTime = 0;
    float realFrameTime = 0; 

    int x_dir = 1;
    int y_dir = 1;
    float radIncr = 0;

    bool takeScreenshot = false;

    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;

    while (window.isOpen()) {
        timer.restart();

        /* Event loop */
        sf::Event ev;
        window.clear(sf::Color::Blue);
        window.draw(cr1);
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed)
                window.close();
            if (ev.type == sf::Event::KeyReleased) {
                switch(ev.key.code) {
                    case (sf::Keyboard::Delete):
                        window.close();
                        break;
                    case (sf::Keyboard::Escape):
                        gamePause = !gamePause;
                        break;
                    case (sf::Keyboard::Up):
                        if (FPS < 60) {
                            FPS += 15;
                        }
                        break;
                    case (sf::Keyboard::Down):
                        if (FPS > 15) {
                            FPS -= 15;
                        }
                        break;
                    case (sf::Keyboard::F2):
                        takeScreenshot = true;
                        break;
                }
            }

            // Движение фигуры
            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::W) {
                    upPressed = true;
                }
                if (ev.key.code == sf::Keyboard::S) {
                    downPressed = true;
                }
                if (ev.key.code == sf::Keyboard::A) {
                    leftPressed = true;
                }
                if (ev.key.code == sf::Keyboard::D) {
                    rightPressed = true;
                }
            }
            if (ev.type == sf::Event::KeyReleased) {
                if (ev.key.code == sf::Keyboard::W) {
                    upPressed = false;
                }
                if (ev.key.code == sf::Keyboard::S) {
                    downPressed = false;
                }
                if (ev.key.code == sf::Keyboard::A) {
                    leftPressed = false;
                }
                if (ev.key.code == sf::Keyboard::D) {
                    rightPressed = false;
                }
            }
        }

        if (!gamePause) {
            realFrameTime = 1.0 / fpsTimer.restart().asSeconds();
            fpsText.setString(std::to_string(int(ceil(realFrameTime))));
            
            circleSpeedY = 0;
            circleSpeedX = 0;
            if (upPressed) circleSpeedY = -FIGURE_SPEED;
            if (downPressed) circleSpeedY = FIGURE_SPEED;
            if (leftPressed) circleSpeedX = -FIGURE_SPEED;
            if (rightPressed) circleSpeedX = FIGURE_SPEED;

            circlePos = cr1.getPosition();
            circlePos.x += x_dir * circleSpeedX * (defaultFPS / realFrameTime); // передвижение
            circlePos.y += y_dir * circleSpeedY * (defaultFPS / realFrameTime); // передвижение

            if (circlePos.y < 0) circlePos.y = 0;
            if (circlePos.x < 0) circlePos.x = 0;
            if (circlePos.y > WINDOW_HEIGHT - circleRadius * 2) circlePos.y = WINDOW_HEIGHT - circleRadius * 2;
            if (circlePos.x > WINDOW_WIDTH - circleRadius * 2) circlePos.x = WINDOW_WIDTH - circleRadius * 2;

            cr1.setPosition(circlePos);
            window.draw(fpsText);
        }

        deltaTime = timer.restart().asSeconds();

        /* FPS limit */
        sf::sleep(sf::Time(sf::seconds(1.f / FPS - deltaTime)));

        window.display();

        if (takeScreenshot) {
            sf::Texture texture;
            texture.create(window.getSize().x, window.getSize().y);
            texture.update(window);
            sf::Image screenshot = texture.copyToImage();

            if (screenshot.saveToFile("screenshot.png"))
            {
                std::cout << "Изображение успешно сохранено в файл 'screenshot.png'" << std::endl;
            }
            else
            {
                std::cerr << "Ошибка при сохранении изображения" << std::endl;
            }
            takeScreenshot = false;
        }
    }
}
