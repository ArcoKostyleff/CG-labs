#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cmath>
#include <iostream>
#include <stack>
#include <thread>

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
// void _floodFillImage(sf::Image& image, int x, int y, sf::Color outlineColor, sf::Color newColor)
// {
//     // Проверяем выход за границы
//     if (x < 0 || y < 0)
//         return;
//     if (x >= image.getSize().x || y >= image.getSize().y)
//         return;
//     // Проверяем пиксель на цвет
//     sf::Color color = image.getPixel(x, y);
//     if (color == outlineColor)
//         return; // Граничный цвет
//     if (color == newColor)
//         return; // Цвет заливки
//     // Устанавливаем цвет пискелю
//     image.setPixel(x, y, newColor);
//     // Рекурсивно заполняем соседние пиксели
//     _floodFillImage(image, x + 1, y, outlineColor, newColor);
//     _floodFillImage(image, x - 1, y, outlineColor, newColor);
//     _floodFillImage(image, x, y + 1, outlineColor, newColor);
//     _floodFillImage(image, x, y - 1, outlineColor, newColor);
// }

void _floodFillImage(sf::Image& image, int x, int y, sf::Color currentColor, sf::Color newColor)
{
    // Проверяем выход за границы
    if (x < 0 || y < 0)
        return;
    if (x >= image.getSize().x || y >= image.getSize().y)
        return;
    // Проверяем пиксель на цвет
    sf::Color color = image.getPixel(x, y);
    if (color != currentColor)
        return;
    // if (color == outlineColor)
    //     return; // Граничный цвет
    // if (color == newColor)
    //     return; // Цвет заливки

    // Устанавливаем цвет пискелю
    // Смешиваем цвета с учетом альфа-канала
    sf::Color blendedColor = sf::Color::White;
    blendedColor.r = (newColor.r * newColor.a + color.r * (255 - newColor.a)) / 255.0;
    blendedColor.g = (newColor.g * newColor.a + color.g * (255 - newColor.a)) / 255.0;

    blendedColor.b = (newColor.b * newColor.a + color.b * (255 - newColor.a)) / 255.0;
    blendedColor.a = 255;
    image.setPixel(x, y, blendedColor);

    // Рекурсивно заполняем соседние пиксели
    _floodFillImage(image, x + 1, y, color, newColor);
    _floodFillImage(image, x - 1, y, color, newColor);
    _floodFillImage(image, x, y + 1, color, newColor);
    _floodFillImage(image, x, y - 1, color, newColor);
}

// Заливка
void floodFill(sf::RenderWindow& window, int x, int y, sf::Color outlineColor, sf::Color newColor)
{
    // Конвертируем в image
    sf::Texture texture;
    texture.create(window.getSize().x, window.getSize().y);
    texture.update(window);
    sf::Image image = texture.copyToImage();
    // Начинаем рекурсивную заливку
    _floodFillImage(image, x, y, image.getPixel(x, y), newColor);
    // Отображаем залитое изображение
    window.clear();
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);
    window.draw(sprite);
}

// Стековый алгоритм заливки
void modifiedStackFloodFill(sf::RenderWindow& window, int x, int y, sf::Color outlineColor, sf::Color newColor)
{
    // Конвертируем в image
    sf::Texture texture;
    texture.create(window.getSize().x, window.getSize().y);
    texture.update(window);
    sf::Image image = texture.copyToImage();
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
        // Проверяем пиксель на цвет
        sf::Color color = image.getPixel(x, y);
        if (color == outlineColor)
            continue;
        if (color == newColor)
            continue;
        // Устанавливаем цвет пискелю
        image.setPixel(x, y, newColor);
        // Добавляем в стек соседние точки
        s.push({ x + 1, y });
        s.push({ x - 1, y });
        s.push({ x, y + 1 });
        s.push({ x, y - 1 });
    }
    // Отображаем залитое изображение

    window.clear();

    texture.loadFromImage(image);
    sf::Sprite sprite(texture);
    window.draw(sprite);
}

void drawStar(sf::RenderWindow& window, sf::Vector2f basePoint, float h, sf::Color color)
{
    // Определение точек для построения звезды
    std::vector<std::pair<float, float>> polarCoordsOuter = {
        { h, -90 },
        { h, -18 },
        { h, -306 },
        { h, -234 },
        { h, -162 }
    };

    std::vector<std::pair<float, float>> polarCoordsInner = {
        { h * 30 / 75, -53 },
        { h * 30 / 75, -342 },
        { h * 30 / 75, -270 },
        { h * 30 / 75, -198 },
        { h * 30 / 75, -126 }
    };

    // Приведение градусов к радианам (2 компоненты пары)
    for (auto& pair : polarCoordsOuter) {
        pair.second *= M_PI / 180;
    }
    for (auto& pair : polarCoordsInner) {
        pair.second *= M_PI / 180;
    }

    // Перевод в декартовые координаты
    std::vector<sf::Vector2f> outerPoints;
    std::vector<sf::Vector2f> innerPoints;
    for (auto& pair : polarCoordsOuter) {
        outerPoints.push_back({ static_cast<float>(pair.first * cos(pair.second)), static_cast<float>(pair.first * sin(pair.second)) });
        outerPoints.back().x += basePoint.x;
        outerPoints.back().y += basePoint.y;
    }
    for (auto& pair : polarCoordsInner) {
        innerPoints.push_back({ static_cast<float>(pair.first * cos(pair.second)), static_cast<float>(pair.first * sin(pair.second)) });
        innerPoints.back().x += basePoint.x;
        innerPoints.back().y += basePoint.y;
    }

    // Соединение соответствующих точек
    for (int i = 0; i < 5; i++) {
        drawLineBresenham(window, outerPoints[i], innerPoints[i], color);
        drawLineBresenham(window, innerPoints[i], outerPoints[(i + 1) % 5], color);

        // window.display();

        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    window.display();
}