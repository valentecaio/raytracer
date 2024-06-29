#pragma once

#include "common.hpp"

using namespace raytracer;

// Miscelaneous utility functions that are used throughout the raytracer.
namespace raytracer::utils {


// MATH UTILS //

// returns the min of a list of values, example: min({1, 2, 3, 4}) -> 1
template<typename T>
inline T min(std::initializer_list<T> list) {
  return *std::min_element(list.begin(), list.end());
}

// returns the max of a list of values, example: max({1, 2, 3, 4}) -> 4
template<typename T>
inline T max(std::initializer_list<T> list) {
  return *std::max_element(list.begin(), list.end());
}

// Schlick's approximation for reflectance
inline double reflectance(double cos_theta, double refraction_idx) {
  double r0 = (1-refraction_idx) / (1+refraction_idx);
  r0 = r0*r0;
  return r0 + (1-r0)*pow((1 - cos_theta), 5);
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
  return (linear_component > 0) ? std::sqrt(linear_component) : 0;
}

void write_pixel(std::ostream &out, Colour pixel) {
  // fix NaN values
  if (pixel.r != pixel.r) pixel.r = 0;
  if (pixel.g != pixel.g) pixel.g = 0;
  if (pixel.b != pixel.b) pixel.b = 0;

  out << static_cast<int>(255.999 * linear_to_gamma(pixel.r)) << ' '
      << static_cast<int>(255.999 * linear_to_gamma(pixel.g)) << ' '
      << static_cast<int>(255.999 * linear_to_gamma(pixel.b)) << '\n';
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
