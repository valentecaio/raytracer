#pragma once

#include "common.hpp"
#include "utils.hpp"

using namespace raytracer;

// The vec namespace contains utility functions for vectors.
namespace raytracer::vec {

// returns a random Vec in [0,1)^3.
inline Vec random() {
  return Vec(utils::random(), utils::random(), utils::random());
}

// returns a random Vec in [min,max)^3.
inline Vec random(double min, double max) {
  return Vec(utils::random(min, max), utils::random(min, max), utils::random(min, max));
}

// returns a random unit vector (a point on the surface of the unit sphere).
inline Vec random_unit() {
  double z = 1 - 2*utils::random();    // z = random in [-1,1]

  double r = std::sqrt(1 - z*z);
  double phi = 2*M_PI*utils::random(); // phi = random in [0, 2pi)

  double x = r*cos(phi);
  double y = r*sin(phi);

  return glm::normalize(Vec(x, y, z));
}

// returns a unit vector in the hemisphere of the normal.
inline Vec random_unit_hemisphere(const Vec& normal) {
  Vec vec = random_unit();
  return (glm::dot(vec, normal) > 0.0) ? vec : -vec;
}

// true if the vector is close to zero in all dimensions.
inline bool is_near_zero(const Vec& v) {
  return (std::fabs(v.x) < NEAR_ZERO) && (std::fabs(v.y) < NEAR_ZERO) && (std::fabs(v.z) < NEAR_ZERO);
}

inline double squared_length(const Vec& v) {
  return glm::dot(v, v);
}

// returns the reflection of a vector v around a normal n.
// v and n must be normalized
inline Vec reflect(const Vec& v, const Vec& n) {
  return v - 2*glm::dot(v,n)*n;
}

// returns the refraction of a vector uv through a normal n.
inline Vec refract(const Vec& uv, const Vec& n, double etai_over_etat) {
  double cos_theta = std::min(dot(-uv, n), 1.0);
  Vec r_out_perp =  etai_over_etat * (uv + cos_theta*n);
  Vec r_out_parallel = -std::sqrt(std::fabs(1.0 - squared_length(r_out_perp))) * n;
  return r_out_perp + r_out_parallel;
}

// print a Vec
inline void print(const Vec& v) {
  std::clog << "{" << v.x << ", " << v.y << ", " << v.z << "}" << std::endl;
}

} // namespace raytracer::vec
