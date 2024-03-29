#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>

#include "point.hpp"
#include "colour.hpp"

namespace raytracer {

void write_image(int image_width, int image_height, std::vector<std::vector<colour3>>& pixels) {
  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
  for (int j = 0; j < image_height; ++j) {
    std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
      write_colour(std::cout, pixels[j][i]);
    }
  }
}

} // namespace raytracer

#endif // IMAGE_HPP
