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

// returns a random Vec in [0,1)^3.
inline Vec random_vec() {
  return Vec(random_double(), random_double(), random_double());
}

// returns a random Vec in [min,max)^3.
inline Vec random_vec(double min, double max) {
  return Vec(random_double(min, max), random_double(min, max), random_double(min, max));
}

// returns a random unit vector (i.e. a point on the unit sphere).
inline Vec random_unit_vector() {
  while (true) {
    auto p = random_vec(-1, 1);
    if (glm::length(p) < 1)
      // if the point is inside the unit sphere, return it normalized
      // so that it lies on the surface of the unit sphere
      return glm::normalize(p);
  }
}

// returns a unit vector in the hemisphere of the normal.
inline Vec random_vec_on_hemisphere(const Vec& normal) {
  Vec vec = random_unit_vector();
  return (glm::dot(vec, normal) > 0.0) ? vec : -vec;
}

// true if the vector is close to zero in all dimensions.
inline bool vec_is_near_zero(const Vec& v) {
  auto s = 1e-8;
  return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}

// returns the reflection of a vector v around a normal n.
inline Vec vec_reflect(const Vec& v, const Vec& n) {
  return v - 2*glm::dot(v,n)*n;
}

// print a Vec
inline void print_vec(const Vec& v) {
  std::clog << "{" << v.x << ", " << v.y << ", " << v.z << "}" << std::endl;
}

} // namespace raytracer

#endif // UTILS_H
