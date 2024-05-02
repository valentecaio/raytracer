#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "../utils/common.hpp"
#include "../utils/interval.hpp"
#include "../utils/utils.hpp"
#include "../hittable/hit_record.hpp"
#include "../material.hpp"
#include "primitive2d.hpp"

namespace raytracer {

// A Triangle is defined by three points in the 2D plane.
class Triangle : public Primitive2D {
  public:
    Triangle(const Point& _a, const Point& _b, const Point& _c, const shared_ptr<Material>& _material) {
      // set superclass fields
      material = _material;
      origin = _a;
      u = _b - _a;
      v = _c - _a;

      // more info in the Quad class
      Vec n = glm::cross(u, v);
      normal = glm::normalize(n);
      d = glm::dot(normal, origin);
      w = n / glm::dot(n, n);
    }

    bool is_hit(double alpha, double beta) const {
      return alpha > 0 && beta > 0 && (alpha + beta <= 1);
    }

    Point get_sample() const override {
      return utils::sample_triangle(origin, u, v);
    }
};

} // namespace raytracer

#endif // TRIANGLE_HPP
