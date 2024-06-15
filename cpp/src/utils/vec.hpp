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
  // return Vec(glm::dot(vec, u), glm::dot(vec, v), glm::dot(vec, w));

  // linear combination of the basis vectors weighted by vec
  return u*vec.x + v*vec.y + w*vec.z;
}

// returns a random unit vector (a point on the surface of the unit sphere).
inline Vec random_sphere_uniform() {
  double z   = utils::random(-1, 1);     // z   = random in [-1, 1)
  double phi = utils::random(0, 2*M_PI); // phi = random in [0, 2pi)
  double r = std::sqrt(1 - z*z);
  double x = r*cos(phi);                 // x = sqrt(1 - z*z) * cos(phi)
  double y = r*sin(phi);                 // y = sqrt(1 - z*z) * sin(phi)
  return Vec(x, y, z);                   // already unitary
}

// Returns an uniform sampled unit vector in the hemisphere of the z-axis.
// The returned vector is always in the hemisphere of the normal vector.
// This is uniform in the hemisphere of the z-axis,
// but not uniform in the hemisphere of the normal vector.
inline Vec random_hemisphere_uniform(const Vec& normal) {
  Vec vec = random_sphere_uniform();
  // if the normal and vec are NOT in the same hemisphere, invert vec
  return (glm::dot(vec, normal) > 0.0) ? vec : -vec;
}

// Returns a cosine sampled vector in the hemisphere of the z-axis.
// https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html#generatingrandomdirections/cosinesamplingahemisphere
inline Vec random_hemisphere_cosine() {
  double r   = utils::random();          // r   = random in [0, 1)
  double phi = utils::random(0, 2*M_PI); // phi = random in [0, 2pi)
  auto x = cos(phi)*sqrt(r);             // x = sqrt(r) * cos(phi)
  auto y = sin(phi)*sqrt(r);             // y = sqrt(r) * sin(phi)
  auto z = sqrt(1-r);                    // z = sqrt(1 - r)
  return Vec(x, y, z);
}

// Returns a cosine sampled vector in the hemisphere of the normal.
// This is cosine-weighted in the hemisphere of the normal vector.
inline Vec random_hemisphere_cosine(const Vec& normal) {
  Vec vec = random_hemisphere_cosine();
  return glm::normalize(change_basis(normal, vec)); // TODO: need to normalize ?
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
