#ifndef QUAD_H
#define QUAD_H

#include "../utils/common.hpp"
#include "../utils/interval.hpp"
#include "../utils/utils.hpp"
#include "../hittable/hit_record.hpp"
#include "../hittable/hittable_geometry.hpp"
#include "../material.hpp"
#include "primitive2d.hpp"

namespace raytracer {

// A Quad is defined by an origin point and two vectors that define the plane where the quad lies.
class Quad : public Primitive2D {
  public:
    Quad(const Point& _origin, const Vec& _u, const Vec& _v, const shared_ptr<Material>& _material) {
      // set superclass fields
      material = _material;
      origin = _origin;
      u = _u;
      v = _v;

      // the normal is ortogonal to the two vectors that define the quad
      Vec n = glm::cross(u, v);
      normal = glm::normalize(n);

      // calculate the constant term of the plane equation [ax + by + cz = d]
      // where (a, b, c) is the normal vector and (x, y, z) is the origin point
      d = glm::dot(normal, origin);

      // w is the constant used to find the planar coordinates alpha & beta
      // of a point P in the uv plane (P = origin + u*alpha + v*beta)
      // op = P - origin
      // alpha = w * cross(op, v)
      // beta  = w * cross(u, op)
      w = n / glm::dot(n, n);
    }

    bool is_hit(double alpha, double beta) const {
      Interval boundaries = Interval(0, 1);
      return boundaries.contains(alpha) && boundaries.contains(beta);
    }

    Point get_sample() const override {
      return utils::sample_quad(origin, u, v);
    }
};

} // namespace raytracer

#endif // QUAD_H
