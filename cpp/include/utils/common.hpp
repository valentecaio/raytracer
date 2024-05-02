#ifndef COMMON_H
#define COMMON_H

/* INCLUDES */
#include <iostream>    // std::cout, std::clog, std::flush
#include <memory>      // shared_ptr, make_shared, shared_ptr_cast
#include <vector>
#include <cstdlib>     // rand
#include <cmath>       // sqrt, fabs
#include <limits>      // infinity
#include <algorithm>   // max, min
#include <glm.hpp>     // glm::dvec3, glm::dot, glm::normalize
#include <functional>  // std::function
#include <chrono>      // std::chrono::high_resolution_clock


/* USINGS */
using std::shared_ptr;
using std::make_shared;
using std::sqrt;
using std::min;
using std::max;
using std::fabs;


namespace raytracer {

/* CLASS ALIASES */
using Vec = glm::dvec3;
using Point = glm::dvec3;
using Colour = glm::dvec3;


/* CONSTANTS */
const double NEAR_ZERO = 1e-8;
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

} // namespace raytracer

#endif // COMMON_H
