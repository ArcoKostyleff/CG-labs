#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <windows.h>
#include "funcs.h"

const int SCALE = 30;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct mPolygon {
    sf::Color color;
    std::vector<sf::Vector2f> points;
};

std::vector<mPolygon> readPolygonsFromFile(const std::string& filename, int scale = 1, int shiftX = 0, int shiftY = 0) {
    std::ifstream in(filename);
    std::vector<mPolygon> polygons;
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
        mPolygon mPolygon;
        in >> pointsCount;
        in >> r >> g >> b;
        mPolygon.color.r = r;
        mPolygon.color.g = g;
        mPolygon.color.b = b;
        for (int j = 0; j < pointsCount; j++) {
            float x, y;
            in >> x >> y;
			y =  5- y;
            mPolygon.points.push_back(sf::Vector2f(x * scale + shiftX, y * scale + shiftY));
        }
        polygons.push_back(mPolygon);
    }
    in.close();
    return polygons;
}
struct layer {
    uint16_t num{};
    mPolygon triangle;
};


// Returns x-value of point of intersection of two
// lines
int x_intersect(int x1, int y1, int x2, int y2,
	int x3, int y3, int x4, int y4)
{
	int num = (x1 * y2 - y1 * x2) * (x3 - x4) -
		(x1 - x2) * (x3 * y4 - y3 * x4);
	int den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	return num / den;
}

// Returns y-value of point of intersection of
// two lines
int y_intersect(int x1, int y1, int x2, int y2,
	int x3, int y3, int x4, int y4)
{
	int num = (x1 * y2 - y1 * x2) * (y3 - y4) -
		(y1 - y2) * (x3 * y4 - y3 * x4);
	int den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	return num / den;
}

void clipOutside(mPolygon& poly_points,
	int x1, int y1, int x2, int y2)
{
	std::vector<std::pair<int, int>> new_points;
	int new_poly_size = 0;

	// (ix,iy),(kx,ky) are the co-ordinate values of
	// the points
	for (int i = 0; i < poly_points.points.size(); i++)
	{
		// i and k form a line in mPolygon
		int k = (i + 1) % poly_points.points.size();;
		int ix = poly_points.points[i].x, iy = poly_points.points[i].y;
		int kx = poly_points.points[k].x, ky = poly_points.points[k].y;

		// Calculating position of first point
		// w.r.t. clipper line
		int i_pos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);

		// Calculating position of second point
		// w.r.t. clipper line
		int k_pos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1);

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

			int x = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			int y = y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			new_points.push_back({ x,y });

		}

		// Case 3: When only second point is outside
		else if (i_pos < 0 && k_pos >= 0)
		{
			//Only point of intersection with edge is added
			int x = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			int y = y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
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
void clip(mPolygon &poly_points,
	int x1, int y1, int x2, int y2)
{
	std::vector<std::pair<int, int>> new_points;
	int new_poly_size = 0;

	// (ix,iy),(kx,ky) are the co-ordinate values of
	// the points
	for (int i = 0; i < poly_points.points.size(); i++)
	{
		// i and k form a line in mPolygon
		int k = (i + 1) % poly_points.points.size();;
		int ix = poly_points.points[i].x, iy = poly_points.points[i].y;
		int kx = poly_points.points[k].x, ky = poly_points.points[k].y;

		// Calculating position of first point
		// w.r.t. clipper line
		int i_pos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);

		// Calculating position of second point
		// w.r.t. clipper line
		int k_pos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1);

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
			int x = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			int y = y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			new_points.push_back({ x,y });
	
			new_points.push_back({ kx,ky });
		}

		// Case 3: When only second point is outside
		else if (i_pos < 0 && k_pos >= 0)
		{
			//Only point of intersection with edge is added
			int x = x_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
			int y = y_intersect(x1, y1, x2, y2, ix, iy, kx, ky);
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
	
	//std::cout << "size before clip:" << poly_points.points.size()<<std::endl;
	poly_points.points.clear();
	
	for (int i = 0; i < new_points.size(); i++)
	{
		poly_points.points.push_back(sf::Vector2f(new_points[i].first,new_points[i].second));
	}
	//std::cout << "size after clip:" << poly_points.points.size() << std::endl;

}

// алгоритм кадрирования
// Сазерленд Коен


// отсечение фигурой внутренней областью
// Сазерленд Хогман
// Implements Sutherland–Hodgman algorithm
mPolygon suthHodgClip(const mPolygon &poly_points, const mPolygon &clipper_points)
{
	mPolygon newP = poly_points;
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

	// Printing vertices of clipped mPolygon
	for (int i = 0; i < newP.points.size(); i++)
		std::cout << '(' << newP.points[i].x <<
		", " << newP.points[i].y << ") ";
	return newP;
}

const int LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8;
int calculateCode(sf::Vector2f& p, sf::Vector2f& min, sf::Vector2f& max)
{
    int code = 0;
    if (p.x < min.x)
        code |= LEFT;
    else if (p.x > max.x)
        code |= RIGHT;

    if (p.y < min.y)
        code |= BOTTOM;
    else if (p.y > max.y)
        code |= TOP;

    return code;
}

class Renderer {
public:
	void wievA() {

		window.display();
	}
	void wievB() {
		
		window.display();
	}
	void DLDDA() {
		drawLineDDA(window, { min.x, min.y }, { max.x, min.y }, sf::Color::Green);
		drawLineDDA(window, { max.x, min.y }, { max.x, max.y }, sf::Color::Green);
		drawLineDDA(window, { max.x, max.y }, { min.x, max.y }, sf::Color::Green);
		drawLineDDA(window, { min.x, max.y }, { min.x, min.y }, sf::Color::Green);
	}
	void toggleWievMode() {
		if (modeA) {
			modeA = false;
			wievA();
		}
		else {
			modeA = true;
			wievB();
		}
	}
	sf::RenderWindow window{ sf::VideoMode{WINDOW_WIDTH, WINDOW_HEIGHT}, "Lab 3 - Variant 1" };
	Renderer() {
		window.clear(sf::Color::Black);
		//polygons = readPolygonsFromFile("pollygons.txt", SCALE, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		//std::reverse(polygons.begin(), polygons.end());
		
	}
	std::vector<mPolygon> polygons;
	void drawCircle(int xc, int yc, int x, int y)
	{
		putPixel(xc + x, yc + y, sf::Color::Blue);
		putPixel(xc - x, yc + y, sf::Color::Blue);
		putPixel(xc + x, yc - y, sf::Color::Blue);
		putPixel(xc - x, yc - y, sf::Color::Blue);
		putPixel(xc + y, yc + x, sf::Color::Blue);
		putPixel(xc - y, yc + x, sf::Color::Blue);
		putPixel(xc + y, yc - x, sf::Color::Blue);
		putPixel(xc - y, yc - x, sf::Color::Blue);
	}
	void putPixel(  int x, int y,sf::Color color ) {
		sf::RectangleShape pixel(sf::Vector2f(1, 1));
		pixel.setPosition(x, y);
		pixel.setFillColor(color);
		window.draw(pixel);
	}
	// Function for circle-generation 
	// using Bresenham's algorithm 
	void circleBres(int xc, int yc, int r)
	{
		int x = 0, y = r;
		int d = 3 - 2 * r;
		drawCircle(xc, yc, x, y);
		while (y >= x)
		{
			// for each pixel we will 
			// draw all eight pixels 

			x++;

			// check for decision parameter 
			// and correspondingly  
			// update d, x, y 
			if (d > 0)
			{
				y--;
				d = d + 4 * (x - y) + 10;
			}
			else
				d = d + 4 * x + 6;
			drawCircle(xc, yc, x, y);
		
		}
	}
	void drawSpiralArc( float radius, int pixelCount, float delta, sf::Vector2f center) {
		// Create an image to draw on

		// Calculate the number of pixels to draw
		//int numPixels = 2 * M_PI * radius * spinsCount;

		

		double phi, r;
		// Draw each pixel
		for (int i = 0; i < pixelCount; i++) {
			phi = i * delta;
			r = delta * i;
			// Calculate the angle for this pixel
	
			// Calculate the position in Cartesian coordinates
			float x = r * cos(phi);
			float y = r * sin(phi);

			// Set the pixel color
			putPixel(center.x + x, center.y + y, sf::Color::White);
			//image.setPixel(center.x + x, center.y + y, sf::Color::White);
		}
		int n;

	}
	void testTime(float radius, int pixelCount, float delta, sf::Vector2f center) {
		long long first = GetTickCount64();
		drawSpiralArc(radius, pixelCount, delta, center);
		long long second = GetTickCount64();
		double time = (second - first) ;
		std::cout << "time of Spiral drawing with " << pixelCount << " pixels is" << ": " << time<<" ms" << std::endl;
	}
private:
	const double M_PI = 3.14156295;
	sf::Vector2f p1{ 0, 0 };
	sf::Vector2f p2{ 300, 250 };
	sf::Vector2f min{ 150, 100 };
	sf::Vector2f max{ 250, 200 };
	bool modeA = 1;
};

int main() {
	Renderer wnd;

	//wnd.circleBres(100,100,50);
	
	wnd.testTime(20,  200, 0.05, { 100,100 });
	wnd.testTime(50,  500, 0.05, { 100,200 });
	wnd.testTime(50,  700, 0.05, { 200,100 });
	wnd.testTime(50,  700, 0.05, { 200,200 });
	wnd.testTime(50,  1500, 0.05, { 400,200 });
	wnd.testTime(100,  10000, 0.05, { 500,300 });

    //for (auto& mPolygon : wnd.polygons) {
    //    std::cout << "Draw pollygon" << std::endl;
    //    std::cout << "Points count: " << mPolygon.points.size() << std::endl;
    //   // std::cout << "Color: " << (int)mPolygon.color.r << " " << (int)mPolygon.color.g << " " << (int)mPolygon.color.b << std::endl;
    //    for (auto& point : mPolygon.points)
    //        std::cout << '(' << point.x << "; " << point.y << ')' << std::endl;
    //    std::cout << std::endl;

    //    drawPolygon(wnd.window, mPolygon.points, sf::Color::Black);
		
		
    //}
/*
	mPolygon obrezka;

	for (int i = 0; i < wnd.polygons.size()-4 ; i++) {
		for (int k = 0; k < wnd.polygons.size()-4; k++) {
			if (i != k) {

				obrezka = suthHodgClip(wnd.polygons[k], wnd.polygons[i]);
				if (obrezka.points.size()) {
					drawPolygon(wnd.window, obrezka.points, sf::Color::Blue);
					std::cout << "\n" << i << " " << k << '\n';
				}	

			}
		}
	}
	*/
	wnd.window.display();

    while (wnd.window.isOpen()) {
        sf::Event event;

        while (wnd.window.pollEvent(event)) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				wnd.toggleWievMode();
			}
            if (event.type == sf::Event::Closed) {
				wnd.window.close();
            }
        }
    }

    return 0;
}

 
