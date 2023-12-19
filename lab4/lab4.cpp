#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#include "funcs.h"

const int SCALE = 30;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct Polygon {
    sf::Color color;
    std::vector<sf::Vector2f> points;
};

std::vector<Polygon> readPolygonsFromFile(const std::string& filename, int scale = 1, int shiftX = 0, int shiftY = 0) {
    std::ifstream in(filename);
    std::vector<Polygon> polygons;
    if (!in.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return polygons;
    }
    // Количество полигонов
    int polygonsCount;
    in >> polygonsCount;
    std::cout << "Polygons count: " << polygonsCount << std::endl;
    // Считываем полигоны
    for (int i = 0; i < polygonsCount; i++) {
        int pointsCount;
        int r, g, b;
        Polygon polygon;
        in >> pointsCount;
        in >> r >> g >> b;
        polygon.color.r = r;
        polygon.color.g = g;
        polygon.color.b = b;
        for (int j = 0; j < pointsCount; j++) {
            float x, y;
            in >> x >> y;
            polygon.points.push_back(sf::Vector2f(x * scale + shiftX, y * scale + shiftY));
        }
        polygons.push_back(polygon);
    }
    in.close();
    return polygons;
}
struct layer {
    uint16_t num{};
    Polygon triangle;
};


// Returns x-value of point of intersection of two
// lines
float x_intersect(float x1, float y1, float x2, float y2,
	float x3, float y3, float x4, float y4)
{
	float num = (x1 * y2 - y1 * x2) * (x3 - x4) -
		(x1 - x2) * (x3 * y4 - y3 * x4);
	float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	return num / den;
}

// Returns y-value of point of intersection of
// two lines
float y_intersect(float x1, float y1, float x2, float y2,
	float x3, float y3, float x4, float y4)
{
	float num = (x1 * y2 - y1 * x2) * (y3 - y4) -
		(y1 - y2) * (x3 * y4 - y3 * x4);
	float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	return num / den;
}

void clipOutside(Polygon& poly_points,
	float x1, float y1, float x2, float y2)
{
	std::vector<std::pair<float, float>> new_points;
	int new_poly_size = 0;

	// (ix,iy),(kx,ky) are the co-ordinate values of
	// the points
	for (int i = 0; i < poly_points.points.size(); i++)
	{
		// i and k form a line in polygon
		int k = (i + 1) % poly_points.points.size();;
		float ix = poly_points.points[i].x, iy = poly_points.points[i].y;
		float kx = poly_points.points[k].x, ky = poly_points.points[k].y;

		// Calculating position of first point
		// w.r.t. clipper line
		float i_pos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);

		// Calculating position of second point
		// w.r.t. clipper line
		float k_pos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1);

		// Case 1 : Если обе точки внутри то пропускаем
		if (i_pos < 0 && k_pos < 0)
		{
			// ничего не делаем

		}

		// Case 2: если первая снаружи
		else if (i_pos >= 0 && k_pos < 0)
		{
			// добавляем точку пересечения и наружнюю
			new_points.push_back({ ix,iy });

			float x = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			float y = y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			new_points.push_back({ x,y });

		}

		// Case 3: When only second point is outside
		else if (i_pos < 0 && k_pos >= 0)
		{
			//Only point of intersection with edge is added
			float x = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			float y = y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			new_points.push_back({ x,y });

		}

		// Case 4: When both points are outside
		else
		{
			//No points are added
		}
	}

	// Copying new points into original array
	// and changing the no. of vertices

	poly_points.points.clear();

	for (int i = 0; i < new_points.size(); i++)
	{
		poly_points.points.push_back(sf::Vector2f(new_points[i].first, new_points[i].second));
	}
	//return newPoints;
}

// This functions clips all the edges w.r.t one clip
// edge of clipping area
void clip(Polygon &poly_points,
	float x1, float y1, float x2, float y2)
{
	std::vector<std::pair<float, float>> new_points;
	int new_poly_size = 0;

	// (ix,iy),(kx,ky) are the co-ordinate values of
	// the points
	for (int i = 0; i < poly_points.points.size(); i++)
	{
		// i and k form a line in polygon
		int k = (i + 1) % poly_points.points.size();;
		float ix = poly_points.points[i].x, iy = poly_points.points[i].y;
		float kx = poly_points.points[k].x, ky = poly_points.points[k].y;

		// Calculating position of first point
		// w.r.t. clipper line
		float i_pos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);

		// Calculating position of second point
		// w.r.t. clipper line
		float k_pos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1);

		// Case 1 : When both points are inside
		if (i_pos < 0 && k_pos < 0)
		{
			//Only second point is added
			new_points.push_back({ kx, ky });
		
		}

		// Case 2: When only first point is outside
		else if (i_pos >= 0 && k_pos < 0)
		{
			// Point of intersection with edge
			// and the second point is added
			float x = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			float y = y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			new_points.push_back({ x,y });
	
			new_points.push_back({ kx,ky });
		}

		// Case 3: When only second point is outside
		else if (i_pos < 0 && k_pos >= 0)
		{
			//Only point of intersection with edge is added
			float x = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			float y = y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			new_points.push_back({ x,y });

		}

		// Case 4: When both points are outside
		else
		{
			//No points are added
		}
	}

	// Copying new points into original array
	// and changing the no. of vertices
	
	std::cout << "size before clip:" << poly_points.points.size()<<std::endl;
	poly_points.points.clear();
	
	for (int i = 0; i < new_points.size(); i++)
	{
		poly_points.points.push_back(sf::Vector2f(new_points[i].first,new_points[i].second));
	}
	std::cout << "size after clip:" << poly_points.points.size() << std::endl;

}

// алгоритм кадрирования
// Сазерленд Коен


// отсечение фигурой внутренней областью
// Сазерленд Хогман
// Implements Sutherland–Hodgman algorithm
Polygon suthHodgClip(const Polygon &poly_points, const Polygon &clipper_points)
{
	Polygon newP = poly_points;
	//i and k are two consecutive indexes
	for (int i = 0; i < clipper_points.points.size(); i++)
	{
		int k = (i + 1) % clipper_points.points.size();
		
		// We pass the current array of vertices, it's size
		// and the end points of the selected clipper line
		clip(newP,  clipper_points.points[i].x,
			clipper_points.points[i].y, clipper_points.points[k].x,
			clipper_points.points[k].y);
	}

	// Printing vertices of clipped polygon
	for (int i = 0; i < newP.points.size(); i++)
		std::cout << '(' << newP.points[i].x <<
		", " << newP.points[i].y << ") ";
	return newP;
}


int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lab 3 - Variant 1");
    window.clear(sf::Color::Black);
    auto polygons = readPolygonsFromFile("pollygons.txt", SCALE, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);


    for (auto& polygon : polygons) {
        std::cout << "Draw pollygon" << std::endl;
        std::cout << "Points count: " << polygon.points.size() << std::endl;
       // std::cout << "Color: " << (int)polygon.color.r << " " << (int)polygon.color.g << " " << (int)polygon.color.b << std::endl;
        for (auto& point : polygon.points)
            std::cout << '(' << point.x << "; " << point.y << ')' << std::endl;
        std::cout << std::endl;

        drawPolygon(window, polygon.points, polygon.color);
       // auto point = getPointInsidePolygon(polygon.points);
       // floodFill(window, point.x, point.y, polygon.color, polygon.color);
       
       // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
	Polygon obrezka= suthHodgClip(polygons[0], polygons[1]);
	if (obrezka.points.size())drawPolygon(window, obrezka.points, sf::Color::Red);
	//window.display();
     // Отрисовка координатной прямой посередине экрана
     // Создание вертикальной линии
     sf::Vertex verticalLine[] =
     {
         sf::Vertex(sf::Vector2f(window.getSize().x / 2, 0), sf::Color::White),
         sf::Vertex(sf::Vector2f(window.getSize().x / 2, window.getSize().y), sf::Color::White)
     };
     // Создание горизонтальной линии
     sf::Vertex horizontalLine[] =
     {
         sf::Vertex(sf::Vector2f(0, window.getSize().y / 2), sf::Color::White),
         sf::Vertex(sf::Vector2f(window.getSize().x, window.getSize().y / 2), sf::Color::White)
     };

     window.draw(verticalLine, 2, sf::Lines);
     window.draw(horizontalLine, 2, sf::Lines);

    window.display();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }

    return 0;
}
