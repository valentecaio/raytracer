#ifndef PRIMITIVE2D_H
#define PRIMITIVE2D_H

#include "../utils/common.hpp"
#include "../utils/interval.hpp"
#include "../hittable/hit_record.hpp"
#include "../material.hpp"
#include "primitive.hpp"

namespace raytracer {


// Abstract class that represents an instance of a 2D geometric object in the scene.
// These primitives are defined by an origin point and two vectors that define the plane
// where they lie. Each derived class must implement the is_hit() method that checks
// if a point with planar coordinates (alpha, beta) is inside the primitive boundaries.
class Primitive2D : public Primitive {
  public:
    virtual ~Primitive2D() = default;

    // the intersection of a ray with a 2D primitive is the intersection of the
    // ray with the plane where the primitive lies.
    // the intersection point is then checked to be inside the primitive
    bool hit(const Ray& r, Interval ray_t, HitRecord& hit) const override {
      double denom = glm::dot(normal, r.direction());

      // ray and plane are parallels -> no intersection
      if (std::fabs(denom) < NEAR_ZERO)
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
      // this is the only part where the derived primitives differ
      if(!is_hit(alpha, beta))
        return false;

      // HIT !
      hit.t = t;
      hit.p = p;
      hit.set_face_normal(r, normal);
      hit.object = shared_from_this();
      return true;
    }


  // the following members must be defined at the constructor of the derived class
  protected:
    Point origin; // origin point of the primitive, in the plane
    Vec u, v;     // two vectors that define the plane where the primitive lies

    // should be called by the constructor of the derived class
    // after setting the origin, u and v fields
    void set_constants() {
      // the normal is ortogonal to the two vectors that define the quad
      Vec n = glm::cross(u, v);
      normal = glm::normalize(n);

      // d is the constant term of the plane equation [ax + by + cz = d]
      // where (a, b, c) is the normal vector and (x, y, z) is the origin point
      d = glm::dot(normal, origin);

      // w is the constant used to find the planar coordinates alpha & beta
      // of a point P in the uv plane (P = origin + u*alpha + v*beta)
      w = n / glm::dot(n, n);
    }


  // the following members are calculated by the set_constants method
  private:
    Vec normal; // normal vector to the plane that contains the primitive, normalized
    Vec w;      // constant used to find the planar coordinates of a point
    double d;   // constant term of the plane equation [ax + by + cz = d]

    // check if a 2D point with planar coordinates (alpha, beta) is inside the 2D primitive
    virtual bool is_hit(double alpha, double beta) const = 0;
};



// A Quad is defined by an origin point and two vectors that define the plane where the quad lies.
class Quad : public Primitive2D {
  public:
    Quad(const Point& _origin, const Vec& _u, const Vec& _v, const shared_ptr<Material>& _material) {
      material = _material;
      origin = _origin;
      u = _u;
      v = _v;
      set_constants();
    }

    Point get_sample() const override {
      return utils::sample_quad(origin, u, v);
    }


  private:
    bool is_hit(double alpha, double beta) const {
      return alpha >= 0 && beta >= 0 && alpha <= 1 && beta <= 1;
    }
};



// A Triangle is defined by three points in the 2D plane.
class Triangle : public Primitive2D {
  public:
    Point a, b, c; // unused but useful for debug

    Triangle(const Point& _a, const Point& _b, const Point& _c, const shared_ptr<Material>& _material)
      : a(_a), b(_b), c(_c) {
      material = _material;
      origin = a;
      u = b - a;
      v = c - a;
      set_constants();
    }

    Point get_sample() const override {
      return utils::sample_triangle(origin, u, v);
    }

  private:
    bool is_hit(double alpha, double beta) const {
      return alpha > 0 && beta > 0 && (alpha + beta <= 1);
    }
};


} // namespace raytracer

#endif // PRIMITIVE2D_H
