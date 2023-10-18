#pragma once
#include "io.h"
#include "structs.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <filesystem>
#include <iostream>

void replaceColor(std::vector<sf::Color> &colors) {
  std::cout << "Replace Color? (y/n) \n > ";
  char upscaleResp;
  std::cin >> upscaleResp;
  if (upscaleResp != 'y') {
    return;
  }
  std::cout << "Palette ARGB: \n";
  for (int i = 0; i < colors.size(); i++) {
    std::cout << i << ". (" << (int)colors[i].a << ", " << (int)colors[i].r
              << ", " << (int)colors[i].g << ", " << (int)colors[i].b << ")\n";
  }

  uint32_t index;
  do {
    std::cout << "Index of color to replace? \n > ";
    std::cin >> index;
  } while (index < 0 || index > colors.size());

  uint32_t a, r, g, b;
  do {
    std::cout << "ARGB componenents separated by space in range [0; 255]\n > ";
    std::cin >> a >> r >> g >> b;
  } while (a < 0 || r < 0 || g < 0 || b < 0 || a > 255 || r > 255 || g > 255 ||
           b > 255);

  sf::Color newColor(r, g, b, a);
  colors[index] = newColor;

  std::cout << "Success!\n";
}