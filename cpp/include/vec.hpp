// this header defines the vec utils in the vec namespace 
#ifndef VEC_H
#define VEC_H

#include "common.hpp"
#include "utils.hpp"

using namespace raytracer;

namespace vec {

  // returns a random Vec in [0,1)^3.
  inline Vec random() {
    return Vec(random_double(), random_double(), random_double());
  }

  // returns a random Vec in [min,max)^3.
  inline Vec random(double min, double max) {
    return Vec(random_double(min, max), random_double(min, max), random_double(min, max));
  }

  // returns a random unit vector (a point on the surface of the unit sphere).
  inline Vec random_unit() {
    auto z = 1 - 2*random_double();

    auto r = sqrt(1 - z*z);
    auto phi = 2*M_PI*random_double();

    auto x = r*cos(phi);
    auto y = r*sin(phi);

    return glm::normalize(Vec(x, y, z));
  }

  inline Vec random_unit_old() {
    while (true) {
      auto p = random(-1, 1);
      if (glm::length(p) < 1)
        // if the point is inside the unit sphere, return it normalized
        // so that it lies on the surface of the unit sphere
        return glm::normalize(p);
    }
  }

  // returns a unit vector in the hemisphere of the normal.
  inline Vec random_on_hemisphere(const Vec& normal) {
    Vec vec = random_unit();
    return (glm::dot(vec, normal) > 0.0) ? vec : -vec;
  }

  // returns a random Vec in the unit disk (z=0)
  inline Vec random_in_unit_disk() {
    while (true) {
      auto p = Vec(random_double(-1, 1), random_double(-1, 1), 0);
      if (glm::length(p) < 1)
        return p;
    }
  }

  // true if the vector is close to zero in all dimensions.
  inline bool is_near_zero(const Vec& v) {
    auto s = 1e-8;
    return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
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
    auto cos_theta = min(dot(-uv, n), 1.0);
    Vec r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    Vec r_out_parallel = -sqrt(fabs(1.0 - squared_length(r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
  }

  // print a Vec
  inline void print(const Vec& v) {
    std::clog << "{" << v.x << ", " << v.y << ", " << v.z << "}" << std::endl;
  }

} // namespace vec

#endif // VEC_H
