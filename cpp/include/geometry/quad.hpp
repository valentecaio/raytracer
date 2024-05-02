#ifndef QUAD_H
#define QUAD_H

#include "common.hpp"
#include "../hittable/hit_record.hpp"
#include "../hittable/hittable_geometry.hpp"
#include "../interval.hpp"
#include "../material.hpp"
#include "../utils.hpp"

namespace raytracer {

// A Quad is defined by an origin point and two vectors that define the plane where the quad lies.
class Quad : public HittableGeometry {
  public:
    Quad(const Point& _origin, const Vec& _u, const Vec& _v, const shared_ptr<Material>& _material)
      : origin(_origin), u(_u), v(_v) {
        material = _material;

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

    // the intersection of a ray with a quad is the intersection of the ray with the plane
    // where the quad lies. The intersection point is then checked to be inside the quad
    bool hit(const Ray& r, Interval ray_t, HitRecord& hitrec) const override {
      double denom = glm::dot(normal, r.direction());

      // ray and plane are parallels -> no intersection
      if (fabs(denom) < NEAR_ZERO)
        return false;

      // calculate the intersection point, t = (d - n*o) / n*d
      double t = (d - glm::dot(normal, r.origin())) / denom;

      // intersection point outside of ray interval
      if (!ray_t.contains(t))
        return false;

      // planar coordinates of the intersection point (P = origin + u*alpha + v*beta)
      Point p = r.at(t);
      Vec op = p - origin;
      double alpha = glm::dot(w, glm::cross(op, v));
      double beta  = glm::dot(w, glm::cross(u, op));

      // intersection point outside of quad boundaries
      Interval boundaries = Interval(0, 1);
      if(!boundaries.contains(alpha) || !boundaries.contains(beta))
        return false;

      // HIT !
      hitrec.t = t;
      hitrec.p = p;
      hitrec.set_face_normal(r, normal);
      hitrec.object = shared_from_this();
      return true;
    }

    Point get_sample() const override {
      return utils::sample_quad(origin, u, v);
    }

  private:
    Point origin; // origin point of the quad
    Vec normal;   // normal vector to the quad, normalized
    Vec u, v;     // two vectors that define the quad
    Vec w;        // constant used to find the planar coordinates of a point
    double d;     // constant term of the plane equation [ax + by + cz = d]
};

} // namespace raytracer

#endif // QUAD_H
