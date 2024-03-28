// image.cpp

#include "image.hpp"

#include <iostream>

namespace raytracer {

Image::Image(int width, int height) : image_width(width), image_height(height) {}

void Image::generateImage(std::vector<std::vector<Point>>& points) {
  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
  for (int j = 0; j < image_height; ++j) {
    std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
      std::cout << points[j][i].r << ' ' << points[j][i].b << ' ' << points[j][i].b << '\n';
    }
  }
  std::clog << "\rDone.\n";
}

} // namespace raytracer

/*
// Example usage
int main() {
  int width = 800;
  int height = 600;
  std::vector<std::vector<raytracer::Point>> points;

  for (int j = 0; j < height; ++j) {
    points.push_back(std::vector<raytracer::Point>());
    for (int i = 0; i < width; ++i) {
      points[j].push_back(raytracer::Point(rand() % 256, rand() % 256, rand() % 256));
    }
  }

  raytracer::Image image(width, height);
  image.generateImage(points);

  return 0;
}
*/
