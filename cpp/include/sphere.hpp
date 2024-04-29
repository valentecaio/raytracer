#ifndef SPHERE_H
#define SPHERE_H

#include "common.hpp"
#include "hittable.hpp"
#include "interval.hpp"
#include "material.hpp"

namespace raytracer {

class Sphere : public Hittable {
  public:
    Sphere(const Point& _center, double _radius, shared_ptr<Material> _material)
      : center(_center), radius(max(0.0, _radius)), material(_material) {}

    bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override {
      // t = (-b +- sqrt(b*b - 4*a*c)) / 2*a
      Vec oc = ray.origin() - center;                      // oc = A-C
      double a = glm::dot(ray.direction(), ray.direction()); // a = dot(B, B)
      double half_b = glm::dot(oc, ray.direction());         // b = 2*dot(oc, B)
      double oc_length_squared = glm::dot(oc, oc);
      double c = oc_length_squared - radius*radius;          // c = dot(oc, oc) - R*R
      double delta = half_b*half_b - a*c;                    // delta = b*b - 4*a*c

      // if delta is negative, there are no real roots
      // if delta is zero, there is one real root
      // if delta is positive, there are two real roots and we return the smallest one
      if (delta < 0)
        return false;

      // find the nearest root that lies in the acceptable range.
      double sqrtd = sqrt(delta);
      double root = (-half_b - sqrtd) / a; // try nearest root
      if (!ray_t.contains(root)) {
        root = (-half_b + sqrtd) / a;    // try second root
        if (!ray_t.contains(root))
          return false; // both roots are outside the acceptable range
      }

      // HIT !
      rec.t = root;
      rec.p = ray.at(rec.t);
      rec.material = material;
      Vec outward_normal = (rec.p-center)/radius;  // normalized outward normal
      rec.set_face_normal(ray, outward_normal);    // store the face orientation
      return true;
    }

  private:
    Point center;
    double radius;
    shared_ptr<Material> material;
};

} // namespace raytracer

#endif // SPHERE_H
