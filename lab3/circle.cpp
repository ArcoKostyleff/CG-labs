#include <SFML/Graphics.hpp>
#include <cmath>

/// @brief Рисует дугу окружности в заданную в полярных координатах.
/// @param window Окно SFML RenderWindow, на котором будет происходить отрисовка.
/// @param radius Радиус дуги окружности.
/// @param startAngle Начальный угол дуги в градусах.
/// @param endAngle Конечный угол дуги в градусах.
/// @param center Центральная точка дуги.
void drawPolarCircleArc(sf::RenderWindow& window, float radius, float startAngle, float endAngle, sf::Vector2f center)
{
    // Create an image to draw on
    sf::Image image;
    image.create(window.getSize().x, window.getSize().y, sf::Color::Black);

    // Calculate the number of pixels to draw
    int numPixels = 2 * M_PI * radius * abs(endAngle - startAngle) / 360;

    // Draw each pixel
    for (int i = 0; i < numPixels; i++) {
        // Calculate the angle for this pixel
        float pixelAngle = startAngle + i * ((endAngle - startAngle) / numPixels);

        // Convert to radians
        pixelAngle = pixelAngle * M_PI / 180;

        // Calculate the position in Cartesian coordinates
        float x = radius * cos(pixelAngle);
        float y = radius * sin(pixelAngle);

        // Set the pixel color
        image.setPixel(center.x + x, center.y + y, sf::Color::White);
    }

    // Create a texture and sprite from the image
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);
    // Draw the sprite window.draw(sprite);ASDASDLKJ
}

ASDAS