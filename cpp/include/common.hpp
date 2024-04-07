#ifndef COMMON_H
#define COMMON_H

// includes
#include <iostream>    // std::cout, std::clog, std::flush
#include <memory>      // shared_ptr, make_shared
#include <vector>
#include <cmath>       // sqrt
#include <limits>      // infinity
#include <algorithm>   // max, min
#include <glm/glm.hpp> // glm::dvec3, glm::dot, glm::normalize

namespace raytracer {

// usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;
using std::min;
using std::max;

// class aliases
using Vec = glm::dvec3;
using Point = glm::dvec3;
using Colour = glm::dvec3;

// constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// utils
inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

} // namespace raytracer

#endif // COMMON_H
