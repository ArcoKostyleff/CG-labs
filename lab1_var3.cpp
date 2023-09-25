// Computer Graphics Lab 1
// Crew: Kostylev Maleev Zverev
// var 3

#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include <string>
#include <vector>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
static bool gamePause = false;
sf::Clock timer;
float circleRadius{ 40 };
constexpr float circleSpeedX{ 100.f };
constexpr float circleSpeedY{ 50.f };
int x_dir = 1;
int y_dir = 1;
float radIncr = 1;

class AbstractButton {
public:
    // ;
    virtual bool clicked(int m_x, int m_y) = 0;
    virtual void renderButton() = 0;
    sf::Shape* shape_ptr;
    sf::Text* text_ptr;
    int pos_x{};
    int pos_y{};
    int size_x{};
    int size_y{};

private:
};


class menu {
private:
    std::vector<AbstractButton*> buttons;
public:
    void addButton(AbstractButton*b) {
        buttons.push_back(b);
    }
    void showMenu() {
        for (auto p_ptr : buttons) {
            p_ptr->renderButton();
        }
    }
    void clickEvent(int m_x, int m_y) {
        for (auto p_ptr : buttons)
        {
            if (p_ptr->clicked(m_x, m_y));
        }
    }
};

class button : public AbstractButton {
public:
    button(int x, int y, int w, int h, sf::RenderWindow& window, std::string text="") : m_window(window) {
        pos_x = x;
        pos_y = (y);
        size_x = (w);
        size_y = (h);

        shape_ptr = new sf::RectangleShape(sf::Vector2f(w, h));
        shape_ptr->setOutlineThickness(5);
        shape_ptr->setOutlineColor(sf::Color::Black);
        shape_ptr->setPosition(sf::Vector2f(x, y));

        if (!f.loadFromFile("ArialRegular.ttf")) {
            std::cout << "Error while load fonts\n";
        }
        text_ptr = new sf::Text(text, f);
        text_ptr->setFillColor(sf::Color::Green);
        text_ptr->setCharacterSize(20);
        text_ptr->setPosition(sf::Vector2f(x + size_x / 4, y + size_y / 4));
    }
    void renderButton() override {
        m_window.draw(*shape_ptr);
        m_window.draw(*text_ptr);
    }
    sf::RenderWindow &m_window;
private:
    sf::Font f;
};

class resumeButton : public button {
public:
    resumeButton(int x, int y, int w, int h, sf::RenderWindow &window, std::string text="resume") : button(x, y, w, h, window, text) {}
    bool clicked(int m_x, int m_y) override {
        if (m_x >= pos_x && m_x <= pos_x + size_x && m_y >= pos_y && m_y <= pos_y + size_y) {
            gamePause = false;
            return true;
        }
        return false;
    }

private:

};

class quitButton : public button {
public:
    quitButton(int x, int y, int w, int h, sf::RenderWindow& window, std::string text = "quit") : button(x, y, w, h, window, text) {}

    bool clicked(int m_x, int m_y) override {
        if (m_x >= pos_x && m_x <= pos_x + size_x && m_y >= pos_y && m_y <= pos_y + size_y) {
            m_window.close();
            return true;
        }
        return false;
    }
};
class restartButton : public button {
public:
    restartButton(int x, int y, int w, int h, sf::RenderWindow& window,  sf::CircleShape& cr,std::string text = "restart") :  button(x, y, w, h, window, text), cr(cr) {}

    bool clicked(int m_x, int m_y) override {
        if (m_x >= pos_x && m_x <= pos_x + size_x && m_y >= pos_y && m_y <= pos_y + size_y) {
            cr.setRadius(40.f);
            circleRadius = 40.f;
            cr.setPosition({ 0,0 });
            gamePause = false;
            timer.restart();

            return true;
        }
        return false;
    }
private:
    sf::CircleShape &cr;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH,WINDOW_HEIGHT }), "lab1");

    menu mainMenu;

    resumeButton rb(WINDOW_WIDTH/2 - 100,200, 100,40,window, "resume");
    quitButton qb(WINDOW_WIDTH / 2 - 100, 300, 100, 40, window, "quit");

    mainMenu.addButton(&qb);
    mainMenu.addButton(&rb);
    
    sf::CircleShape cr1(circleRadius);

    restartButton rsb(WINDOW_WIDTH / 2 - 100, 400, 100, 40, window, cr1);
    mainMenu.addButton(&rsb);

    cr1.setOutlineThickness(5);
    
    cr1.setOutlineColor(sf::Color::Yellow);
    cr1.setPosition({ 0,0 });
    cr1.setFillColor(sf::Color(0xFF, 0, 0));
    sf::Vector2f circlePos{};

    float deltaTime{};

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
            if (ev.type == sf::Event::MouseButtonPressed)
            {
                if (ev.mouseButton.button == sf::Mouse::Left)
                {
                    std::cout << "lmb pressed" << std::endl;
                    std::cout << "mouse x: " << ev.mouseButton.x << std::endl;
                    std::cout << "mouse y: " << ev.mouseButton.y << std::endl;
                    if (gamePause){
                        mainMenu.clickEvent(ev.mouseButton.x, ev.mouseButton.y);
                        
                }
            }

            }
        }
        if (gamePause) {
            timer.restart();  
        }
        else {
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
        if (gamePause) {
            mainMenu.showMenu();
        }
        window.display();
    }
}
