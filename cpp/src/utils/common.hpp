#ifndef COMMON_H
#define COMMON_H

/* INCLUDES */
#include <iostream>         // std::cout, std::clog, std::flush
#include <memory>           // shared_ptr, make_shared, shared_ptr_cast
#include <vector>           // std::vector
#include <cstdlib>          // rand
#include <cmath>            // sqrt, fabs
#include <limits>           // infinity
#include <algorithm>        // max, min
#include <functional>       // std::function
#include <chrono>           // std::chrono::high_resolution_clock
#include <initializer_list> // std::initializer_list
#include <fstream>          // std::ifstream
#include <sstream>          // std::stringstream

// includes from lib/
#include <glm.hpp>            // algebra
// #include <tiny_obj_loader.h>  // object loading



/* USINGS */
using std::shared_ptr;
using std::make_shared;


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
