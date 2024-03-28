// main.cpp

#include <iostream>
#include <vector>

#include "image.hpp"
#include "point.hpp"

int main() {
  int image_width = 800;
  int image_height = 600;
  std::vector<std::vector<raytracer::Point>> points;

  // Generate random points
  for (int j = 0; j < image_height; ++j) {
    points.push_back(std::vector<raytracer::Point>());
    for (int i = 0; i < image_width; ++i) {
      points[j].push_back(raytracer::Point(rand() % 256, rand() % 256, rand() % 256));
    }
  }

  // Generate and output the image
  raytracer::Image image(image_width, image_height);
  image.generateImage(points);

  return 0;
}

