#pragma once

#include "../utils/common.hpp"
#include "../utils/interval.hpp"
#include "../utils/random.hpp"
#include "../hittable/hit_record.hpp"
#include "../pdf.hpp"
#include "../material.hpp"
#include "primitive.hpp"

namespace raytracer {

// A hittable sphere in 3D space.
class Sphere : public Primitive {
  public:
    Point center;
    double radius;

    Sphere(const Point& _center, double _radius, shared_ptr<Material> _material)
      : center(_center), radius(std::max(0.0, _radius)) {
      // primitive properties
      material = _material;
      area = 4*M_PI*radius*radius;
    }

    // Solves the quadratic equation for the ray-sphere intersection,
    // and returns the nearest intersection point within the acceptable range.
    // The normal is normalised by the radius.
    bool hit(const Ray& r, Interval ray_t, HitRecord& hit) const override {
      // t = (-b +- sqrt(b*b - 4*a*c)) / 2*a
      Vec oc = r.origin() - center;                      // oc = A-C
      double a = glm::dot(r.direction(), r.direction()); // a = dot(B, B)
      double half_b = glm::dot(oc, r.direction());       // b = 2*dot(oc, B)
      double oc_length_squared = glm::dot(oc, oc);
      double c = oc_length_squared - radius*radius;      // c = dot(oc, oc) - R*R
      double delta = half_b*half_b - a*c;                // delta = b*b - 4*a*c

      // if delta is negative, there are no real roots
      // if delta is zero, there is one real root
      // if delta is positive, there are two real roots and we return the smallest one
      if (delta < 0)
        return false;

      // find the nearest root that lies in the acceptable range.
      double sqrtd = std::sqrt(delta);
      double root = (-half_b - sqrtd) / a; // try nearest root
      if (!ray_t.contains(root)) {
        root = (-half_b + sqrtd) / a;      // try second root
        if (!ray_t.contains(root))
          return false; // both roots are outside the acceptable range
      }

      // HIT !
      hit.t = root;
      hit.p = r.at(hit.t);
      hit.object = shared_from_this();
      Vec outward_normal = (hit.p-center)/radius; // normalized outward normal
      hit.set_normal(r, outward_normal);          // store the face orientation
      return true;
    }

    Point sample() const override {
      return random::sample_sphere_uniform(center, radius);
    }

    Sample pdf_sample() const override {
      Point s = sample();
      return Sample{
        s,
        normal(s),
      };
    }
    // normalized outward normal for point p in the surface of the sphere
    Vec normal(Point p) const {
      return (p - center) / radius;
    }

    // TODO: support pdf sampling
    // double pdf_value(const Ray& r) const override {}
};

} // namespace raytracer
