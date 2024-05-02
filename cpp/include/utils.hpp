#ifndef UTILS_H
#define UTILS_H

#include "common.hpp"

// The utils namespace contains utility functions that are used throughout the raytracer.
namespace raytracer::utils {


// RANDOM GENERATION UTILS //

inline double random_int(int min, int max) {
  return min + rand() % (max - min + 1);
}

// returns a random real in [0,1).
inline double random() {
  return rand() / (RAND_MAX + 1.0);
}

// returns a random real in [min,max).
inline double random(double min, double max) {
  return min + (max-min)*random();
}

// returns a random sample in the quad defined by the point p and the vectors u and v
inline Point sample_quad(Point p, Vec u, Vec v) {
  return p + random()*u + random()*v;
}


// TEST UTILS //

inline void clock(const std::function<void()>& func) {
  auto start = std::chrono::high_resolution_clock::now();
  func();
  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration<double>(end-start).count();
  std::clog << "\nExecution time: " << duration << " seconds" << std::endl;
}


// IMAGE UTILS //

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
    // std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
      write_pixel(std::cout, pixels[j][i]);
    }
  }
}

} // namespace raytracer::utils

#endif // UTILS_H
