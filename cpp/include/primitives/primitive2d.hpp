// this file contains the class Primitive2D which is the base class for all 2D primitives
// such as Circle, Ellipse, Quad, Triangle, etc.

#ifndef PRIMITIVE2D_HPP
#define PRIMITIVE2D_HPP

#include "../utils/common.hpp"
#include "../utils/interval.hpp"
#include "../hittable/hit_record.hpp"
#include "../material.hpp"
#include "primitive.hpp"

namespace raytracer {

// Abstract class that represents an instance of a 2D geometric object in the scene.
class Primitive2D : public Primitive {
  public:
    virtual ~Primitive2D() = default;

    // check if a 2D point with planar coordinates (alpha, beta) is inside the primitive
    virtual bool is_hit(double alpha, double beta) const = 0;

    // the intersection of a ray with a 2D primitive is the intersection of the ray with the plane
    // where the primitive lies. The intersection point is then checked to be inside the primitive
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

      // intersection point outside of primitive boundaries
      if(!is_hit(alpha, beta))
        return false;

      // HIT !
      hitrec.t = t;
      hitrec.p = p;
      hitrec.set_face_normal(r, normal);
      hitrec.object = shared_from_this();
      return true;
    }

  // the following members must be defined at the constructor of the derived class
  protected:
    Point origin; // origin point of the primitive, in the plane
    Vec normal;   // normal vector to the plane that contains the primitive, normalized
    Vec u, v;     // two vectors that define the plane where the primitive lies
    Vec w;        // constant used to find the planar coordinates of a point
    double d;     // constant term of the plane equation [ax + by + cz = d]
};

} // namespace raytracer

#endif // PRIMITIVE2D_HPP
