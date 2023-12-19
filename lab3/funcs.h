#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <set>
#include <stack>
#include <unordered_set>

sf::Vector2f getPointInsidePolygon(const std::vector<sf::Vector2f>& vertices)
{
    sf::Vector2f center(0.f, 0.f);
    // Вычисляем сумму координат вершин многоугольника
    for (const auto& vertex : vertices) {
        center += vertex;
    }
    // Делим сумму на количество вершин, чтобы получить центр тяжести
    return center / static_cast<float>(vertices.size());
}

/// @brief Рисует пиксель
/// @param window Основное окно
/// @param color Цвет пикселя
/// @param x Координата X
/// @param y Координата Y
void putPixel(sf::RenderWindow& window, sf::Color color, int x, int y)
{
    sf::RectangleShape pixel(sf::Vector2f(1, 1));
    pixel.setPosition(x, y);
    pixel.setFillColor(color);
    window.draw(pixel);
}

/// @brief Рисует линию с использованием алгоритма ЦДА (Цифрового дифференциального анализатора).
/// @param window Основное окно
/// @param point1 Первая точка
/// @param point2 Вторая точка
/// @param color Цвет линии
void drawLineDDA(sf::RenderWindow& window, sf::Vector2f point1, sf::Vector2f point2, sf::Color color)
{
    // Определяем разницу между двумя точками
    float dx = point2.x - point1.x;
    float dy = point2.y - point1.y;
    // Определяем количество шагов
    int steps = std::max(abs(dx), abs(dy));
    // Определяем инкременты
    float xIncrement = dx / (float)steps;
    float yIncrement = dy / (float)steps;
    // Задаем начальные значения
    float x = point1.x;
    float y = point1.y;
    // Отрисовка отрезка
    for (int i = 0; i <= steps; i++) {
        putPixel(window, color, round(x), round(y));
        x += xIncrement;
        y += yIncrement;
    }
}

/// @brief Рисует линию с использованием алгоритма Брезенхема.
/// @param window Основное окно
/// @param point1 Начало отрезка
/// @param point2 Конец отрезка
/// @param color Цвет линии
void drawLineBresenham(sf::RenderWindow& window, sf::Vector2f point1, sf::Vector2f point2, sf::Color color)
{
    int x1 = point1.x;
    int y1 = point1.y;
    int x2 = point2.x;
    int y2 = point2.y;
    // Разница между координатами
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    // Определяем направления инкремента
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    // Определяем ошибку
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;
    // Отрисовка отрезка
    while (true) {
        // Отрисовка пикселя
        putPixel(window, color, x1, y1);
        // Если достигнут конец отрезка
        if (x1 == x2 && y1 == y2)
            break;
        // Определяем новую ошибку
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y1 += sy;
        }
    }
}

/// @brief Рисует линию с использованием алгоритма Брезенхема.
/// @param window Основное окно
/// @param point1 Начало отрезка
/// @param point2 Конец отрезка
/// @param color Цвет линии
void drawLineBresenham(sf::Image& image, sf::Vector2f point1, sf::Vector2f point2, sf::Color color)
{
    int x1 = point1.x;
    int y1 = point1.y;
    int x2 = point2.x;
    int y2 = point2.y;
    // Разница между координатами
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    // Определяем направления инкремента
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    // Определяем ошибку
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;
    // Отрисовка отрезка
    while (true) {
        // Отрисовка пикселя
        image.setPixel(x1, y1, color);
        // Если достигнут конец отрезка
        if (x1 == x2 && y1 == y2)
            break;
        // Определяем новую ошибку
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y1 += sy;
        }
    }
}

// Отрисовка многоугольника
void drawPolygon(sf::RenderWindow& window, const std::vector<sf::Vector2f>& points, sf::Color color)
{
    // Соединяем точки между собой
    for (size_t i = 0; i < points.size() - 1; i++) {
        drawLineBresenham(window, points[i], points[i + 1], color);
    }
    // Соединяем первую и последнюю точку
    drawLineBresenham(window, points.back(), points.front(), color);
}

// Рекурсивная заливка
void _floodFillImage(sf::Image& image, int x, int y, sf::Color outlineColor, sf::Color newColor)
{
    // Проверяем выход за границы
    if (x < 0 || y < 0)
        return;
    if (x >= image.getSize().x || y >= image.getSize().y)
        return;
    // Проверяем пиксель на цвет
    sf::Color color = image.getPixel(x, y);
    if (color == outlineColor)
        return; // Граничный цвет
    if (color == newColor)
        return; // Цвет заливки
    // Устанавливаем цвет пискелю
    image.setPixel(x, y, newColor);
    // Рекурсивно заполняем соседние пиксели
    _floodFillImage(image, x + 1, y, outlineColor, newColor);
    _floodFillImage(image, x - 1, y, outlineColor, newColor);
    _floodFillImage(image, x, y + 1, outlineColor, newColor);
    _floodFillImage(image, x, y - 1, outlineColor, newColor);
}

// Заливка
void floodFill(sf::Image& window, int x, int y, sf::Color outlineColor, sf::Color newColor)
{
    // Конвертируем в image
    sf::Texture texture;
    texture.create(window.getSize().x, window.getSize().y);
    texture.update(window);
    sf::Image image = texture.copyToImage();
    // Начинаем рекурсивную заливку
    _floodFillImage(window, x, y, outlineColor, newColor);
    // Отображаем залитое изображение
    // window.create(window.getSize().x, window.getSize().y, sf::Color::Transparent);
    // texture.loadFromImage(image);
    // sf::Sprite sprite(texture);
    // window.draw(sprite);
}

// Стековый алгоритм заливки
void modifiedStackFloodFill(sf::Image& window, int x, int y, sf::Color newColor, sf::Vector2f min, sf::Vector2f max)
{
    std::set<std::pair<int, int>> visited;

    sf::Color currentColor = window.getPixel(x, y);
    // Конвертируем в image
    // sf::Texture texture;
    // texture.create(window.getSize().x, window.getSize().y);
    // texture.update(window);
    sf::Image& image = window;
    // Созадем стек и помещаем в него первую точку
    std::stack<std::pair<int, int>> s;
    s.push({ x, y });
    // Заливаем фигуру цветом
    while (!s.empty()) {
        // Извлекаем точку
        int x = s.top().first;
        int y = s.top().second;
        s.pop();
        // Проверяем выход за границы
        if (x < 0 || y < 0)
            continue;
        if (x >= image.getSize().x || y >= image.getSize().y)
            continue;
        if (visited.find({ x, y }) != visited.end())
            continue;
        // if (x < min.x || y < min.y)
        //     continue;
        // if (x > max.x || y > max.y)
        //     continue;
        // Проверяем пиксель на цвет
        sf::Color color = image.getPixel(x, y);
        // if (color == outlineColor)
        //     continue;
        // if (color == newColor)
        //     continue;
        if (color != currentColor)
            continue;
        // Устанавливаем цвет пискелю
        if (!(x < min.x || y < min.y) && !(x > max.x || y > max.y))
            image.setPixel(x, y, newColor);
        else
            visited.insert({ x, y });

        // Добавляем в стек соседние точки
        s.push({ x + 1, y });
        s.push({ x - 1, y });
        s.push({ x, y + 1 });
        s.push({ x, y - 1 });
    }
}
