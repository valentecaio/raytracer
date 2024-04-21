#ifndef UTILS_H
#define UTILS_H

#include "common.hpp"

namespace raytracer {

inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

// returns a random real in [0,1).
inline double random_double() {
  return rand() / (RAND_MAX + 1.0);
}

// returns a random real in [min,max).
inline double random_double(double min, double max) {
  return min + (max-min)*random_double();
}

} // namespace raytracer

#endif // UTILS_H
