#include "funcs.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <fstream>
#include <iostream>
#include <thread>

const int SCALE = 15;
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 300;

inline void Load(const uint8_t* src, __m128i a[3])
{
	a[0] = _mm_loadu_si128((__m128i*)(src - 1)); //загрузка 128 битного вектора по невыровненному по 16 битной границе адресу
	a[1] = _mm_loadu_si128((__m128i*)(src));
	a[2] = _mm_loadu_si128((__m128i*)(src + 1));
}

inline void Load(const uint8_t* src, size_t stride, __m128i a[9])
{
	Load(src - stride, a + 0);
	Load(src, a + 3);
	Load(src + stride, a + 6);
}

inline void Sort(__m128i& a, __m128i& b)
{
	__m128i t = a;
	a = _mm_min_epu8(t, b); //нахождение минимума 2-х 8 битных беззнаковых чисел для каждого из 16 значений вектора
	b = _mm_max_epu8(t, b); //нахождение максимума 2-х 8 битных беззнаковых чисел для каждого из 16 значений вектора
}

inline void Sort(__m128i a[9]) //частично сортирует весь массив
{
	Sort(a[1], a[2]); Sort(a[4], a[5]); Sort(a[7], a[8]);
	Sort(a[0], a[1]); Sort(a[3], a[4]); Sort(a[6], a[7]);
	Sort(a[1], a[2]); Sort(a[4], a[5]); Sort(a[7], a[8]);
	Sort(a[0], a[3]); Sort(a[5], a[8]); Sort(a[4], a[7]);
	Sort(a[3], a[6]); Sort(a[1], a[4]); Sort(a[2], a[5]);
	Sort(a[4], a[7]); Sort(a[4], a[2]); Sort(a[6], a[4]);
	Sort(a[4], a[2]);
}

 void MedianFilter(const uint8_t* src, size_t srcStride, size_t width, size_t height, uint8_t* dst, size_t dstStride)
{
	__m128i a[9];
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; x += sizeof(__m128i))
		{
			Load(src + x, srcStride, a);
			Sort(a);
			_mm_storeu_si128((__m128i*)(dst + x), a[4]); //сохранение 128 битного вектора по невыровненному по 16 битной границе адресу
		}
		src += srcStride;
		dst += dstStride;
	}
}
class imgPrep {
private:
	std::bitset<64> hash;
	sf::Image &img;
	sf::Image erased;
	std::vector<std::pair<uint8_t, uint8_t>> objCoords;
	int countOfObj{ 0 };
	// w - x
	// h - y
	int w, h;

	sf::Color px;
	std::vector<uint8_t> greyPixs;
	std::vector<uint8_t> greyPixsFiltered;
	std::vector<uint8_t> erasedPixs;

	void RGBtoGrayScale() {

		int gray;
		for (int i = 0; i < w; ++i) {
			for (int j = 0; j < h; ++j) {
				px = img.getPixel(i, j);
				gray = (px.r * 0.3 + px.g * 0.59 + px.b * 0.11);
				px.r = gray;
				px.g = gray;
				px.b = gray;
				img.setPixel(i, j, px);
			}
		}

		if (img.saveToFile("gray.png")) {
			std::cout << "grayscaled\n";
		}
	}

	void ImageToMonochrome(sf::Image& img, int t) {
		sf::Color c;
		int gray;

		for (int i = 0; i < img.getSize().x; i++)
		{
			for (int k = 0; k < img.getSize().y; k++)
			{
				c = img.getPixel(i, k);
				gray = (c.r * 0.3 + c.g * 0.59 + c.b * 0.11);
				if (gray > t) {
					c.r = 255;
					c.g = 255;
					c.b = 255;
				}
				else {
					c.r = 0;
					c.g = 0;
					c.b = 0;
				}
				img.setPixel(i, k, c);
			}
		}
		if (img.saveToFile("chb.png")) {
			std::cout << "cherno beloe\n";
		}
	}

	void erase( int seSize = 2) {
		erased = img;
		erasedPixs.resize(w * h, 0);
		int t = 0;
		for (int i = 0; i < w - seSize; i++)
		{
			for (int y = 0; y < h - seSize; y++)
			{
				t = 0;
				for (int k = i; k < i + seSize; k++)
				{
					for (int l = y; l < y + seSize; l++)
					{
						if (greyPixsFiltered[l * w + k]>128) t++;
					}
				}
				if (t == seSize * seSize) {
					for (int k = i; k < i + seSize; k++)
					{
						for (int l = y; l < y + seSize; l++)
						{
							erased.setPixel(k, l, sf::Color::Black);
							erasedPixs[l * w + k] = 1;// 255;
						}
					}
				}
				else {
					for (int k = i; k < i + seSize; k++)
					{
						for (int l = y; l < y + seSize; l++)
						{
							erased.setPixel(k, l, sf::Color::White);
							erasedPixs[l * w + k] = 0;
						}
					}
				}
			}
		}

		if (erased.saveToFile("erased.png")) {
			std::cout << "erased img\n";
		}
		else std::cout << "error erased";
	}
	void markObj(int x, int y) {
		if (x < w && y < h && x >= 0 && y >= 0 && erasedPixs[y * w + x] == 255) {

			erasedPixs[y * w + x] = countOfObj;
			markObj(x + 1, y);
			markObj(x, y + 1);
			markObj(x - 1, y);
			markObj(x, y - 1);
		}
		else {
			std::cout << "Count of obj: " << countOfObj << "\n";
			return;
		}
	}
	void findObj() {
		for (int x = 0;  x< w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				if (erasedPixs[y * w + x] == 255) {
					countOfObj++;

					markObj(x, y);
				}
			}
		}
		std::cout << "Count of obj: " << countOfObj<<"\n";
	}
	void ABCmask() {
		sf::Image ABCim (erased);
		int cur = 1;
		int km = 0, kn = 0;
		int A{}, B{}, C{};
		for (int i = 1; i < w; i++)
		{
			for (int j = 1; j < h; j++)
			{
				kn = j - 1;
				if (kn <= 0) {
					kn = 1;
					B = 0;
				}
				else {
					B = erasedPixs[kn * w + i];
				}
				km = i - 1;
				if (km <= 0) {
					km = 1;
					C = 0;
				}
				else {
					C = erasedPixs[j * w + km];
				}

				A = erasedPixs[j * w + i];
				if (A == 0) {

				}
				else if (B == 0 && C == 0) {  // Если вокруг нашего пикселя пусто а он не пуст — то это повод подумать о новом объекте
					cur += 1;
					erasedPixs[j * w + i] = cur;
					objCoords.push_back({ i,j });
					std::cout << i << " " << j << "\n";
					ABCim.setPixel(i, j, sf::Color(cur*50, cur*50, cur*50));
				}
				else if (B != 0 && C == 0) {
					erasedPixs[j * w + i] = B;
					//objCoords.push_back({ i,j });

					ABCim.setPixel(i, j, sf::Color(B, B, B));
				}
				else if (B == 0 && C != 0) {
					erasedPixs[j * w + i] = C;
					//objCoords.push_back({ i,j });

					ABCim.setPixel(i, j, sf::Color(C, C, C));

				}
				else if (B != 0 && C != 0) {
					if (B == C) {
						erasedPixs[j * w + i] = B;
					//	objCoords.push_back({ i,j });

						ABCim.setPixel(i, j, sf::Color(B, B, B));

					}
					else {
						// перенумеровать все С пиксели в В
						std::replace(erasedPixs.begin(), erasedPixs.end(), C, B);
						ABCim.setPixel(i, j, sf::Color(B, B, B));

						//erasedPixs[j * w + i] = B;
					}
				}
			}
		}
		std::cout << "count: " << cur << "\n";
		if (ABCim.saveToFile("ABCim.png")) {
			std::cout << "ABCim img\n";
		}
		else std::cout << "error ABCim";
	}

	void getHashWindow(std::vector<uint8_t> &res, sf::Image img, int x1,int x2,int y1,int y2) {

	}
	// выделение связных областей
	
	// преобразование связных областей в перцептивный хеш
public:
	imgPrep(sf::Image &image):img(image) {
		w = img.getSize().x;
		h = img.getSize().y;
		greyPixs.resize(w * h);
		greyPixsFiltered.resize(w * h);
		
	}
	void prepareImg() {

		RGBtoGrayScale();

		for (int i = 0; i < w; i++)
		{
			for (int y = 0; y < h; y++)
			{
				px = img.getPixel(i, y);
				greyPixs[i + w * y] = (px.r);
			}
		}
		MedianFilter(greyPixs.data(), 1000, w, h, greyPixsFiltered.data(), 1000);
		sf::Image filtr(img);
		for (int i = 0; i < w; i++)
		{
			for (int y = 0; y < h; y++)
			{
				filtr.setPixel(i, y, sf::Color(greyPixsFiltered[y * w + i], greyPixsFiltered[y * w + i], greyPixsFiltered[y * w + i]));
			}
		}
		if (filtr.saveToFile("filtr.png")) {
			std::cout << "filtered img\n";
		}
		else std::cout << "Erore filterer;\n";
		ImageToMonochrome(filtr, 150);

		if (filtr.saveToFile("prepared.png")) {
			std::cout << "prepared img\n";
		}
		else std::cout << "error prep";
		erase(15);
		ABCmask();
	}

	long long calcHash(uint8_t *data, int size) {
		for (int i = 0; i < size; i++)
		{
			hash[i] = data[i];
		}
		return hash.to_ullong();
	}

};
int main()
{
		Renderer wnd;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lab4");
    window.clear(sf::Color::Black);
   sf::Image img;

//sf::Color a;

if (!img.loadFromFile("img.png")) {
	std::cout << "Errore";
	return 0;
}
imgPrep prep(img);
prep.prepareImg();


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            }
        }
    }

    return 0;
}
