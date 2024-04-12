#ifndef IMAGE_H
#define IMAGE_H

#include "common.hpp"

namespace raytracer {

// convert linear RGB to gamma corrected RGB
inline double linear_to_gamma(double linear_component) {
  return (linear_component > 0) ? sqrt(linear_component) : 0;
}

void write_pixel(std::ostream &out, Colour pixel_colour) {
  out << static_cast<int>(255.999 * linear_to_gamma(pixel_colour.r)) << ' '
      << static_cast<int>(255.999 * linear_to_gamma(pixel_colour.g)) << ' '
      << static_cast<int>(255.999 * linear_to_gamma(pixel_colour.b)) << '\n';
}

void write_image(int image_width, int image_height, std::vector<std::vector<Colour>>& pixels) {
  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
  for (int j = 0; j < image_height; ++j) {
    std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
      write_pixel(std::cout, pixels[j][i]);
    }
  }
}

} // namespace raytracer

#endif // IMAGE_H
