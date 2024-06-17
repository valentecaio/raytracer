#pragma once

#include "common.hpp"

using namespace raytracer;

// The vec namespace contains utility functions for vectors.
namespace raytracer::vec {

// Calculates the orthonormal basis vectors u,v,w from a normal,
// and then returns the linear combination of them,
// weighted by the components of the input vector vec.
inline Vec change_basis(const Vec& normal, const Vec& vec) {
  Vec w = normal;
  Vec a = (fabs(w.x) < 0.9) ? Vec(1,0,0) : Vec(0,1,0); // a vector that is not parallel to w
  Vec v = glm::normalize(glm::cross(w, a));            // v = w x a, v is perpendicular to w
  Vec u = glm::cross(w, v);                            // u = w x v, u is perpendicular to w and v

  // TODO: this gives a different result
  // u,v,w are the basis vectors of the local coordinate system.
  // to convert a vector from world coordinates to local coordinates,
  // we multiply the vector by the inverse of the basis matrix.
  // which is equivalent to multiplying the vector by the transpose of the basis matrix.
  // M = {u, v, w}; M^-1 = M^T (M is orthonormal); return M^T * vec
  // return glm::transpose(glm::mat3(u, v, w)) * vec;
  // return Vec(glm::dot(vec, u), glm::dot(vec, v), glm::dot(vec, w));

  // linear combination of the basis vectors weighted by vec
  return u*vec.x + v*vec.y + w*vec.z;
}

// true if the vector is close to zero in all dimensions.
inline bool is_near_zero(const Vec& v) {
  return (std::fabs(v.x) < NEAR_ZERO) && (std::fabs(v.y) < NEAR_ZERO) && (std::fabs(v.z) < NEAR_ZERO);
}

inline double length_squared(const Vec& v) {
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
  Vec r_out_parallel = -std::sqrt(std::fabs(1.0 - length_squared(r_out_perp))) * n;
  return r_out_perp + r_out_parallel;
}

// print a Vec
inline void print(const Vec& v) {
  std::clog << "{" << v.x << ", " << v.y << ", " << v.z << "}" << std::endl;
}

} // namespace raytracer::vec
