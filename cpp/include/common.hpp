#ifndef COMMON_H
#define COMMON_H

#include <cmath>     // sqrt
#include <limits>    // infinity
#include <memory>    // shared_ptr, make_shared
#include <algorithm> // max, min

// usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;
using std::min;
using std::max;

// constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// utils
inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

#endif // COMMON_H
