// main.cpp

#include <iostream>
#include <vector>

#include "image.hpp"
#include "point.hpp"

int main() {
  int image_width = 800;
  int image_height = 600;
  std::vector<std::vector<raytracer::point3>> points;

  for (int j = 0; j < image_height; ++j) {
    points.push_back(std::vector<raytracer::point3>());
    for (int i = 0; i < image_width; ++i) {
      points[j].push_back(raytracer::point3(rand() % 256, rand() % 256, rand() % 256));
    }
  }

  raytracer::write_image(image_width, image_height, points);

  return 0;
}

