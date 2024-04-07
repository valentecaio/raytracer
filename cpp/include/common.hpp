#ifndef COMMON_H
#define COMMON_H

// includes
#include <iostream>    // std::cout, std::clog, std::flush
#include <memory>      // shared_ptr, make_shared
#include <vector>
#include <cstdlib>     // rand
#include <cmath>       // sqrt
#include <limits>      // infinity
#include <algorithm>   // max, min
#include <glm/glm.hpp> // glm::dvec3, glm::dot, glm::normalize

namespace raytracer {


/* USINGS */
using std::shared_ptr;
using std::make_shared;
using std::sqrt;
using std::min;
using std::max;


/* CLASS ALIASES */
using Vec = glm::dvec3;
using Point = glm::dvec3;
using Colour = glm::dvec3;


/* CONSTANTS */
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;


/* UTILS */
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

#endif // COMMON_H
