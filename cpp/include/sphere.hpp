#ifndef SPHERE_H
#define SPHERE_H

#include "common.hpp"
#include "hittable.hpp"
#include "interval.hpp"

namespace raytracer {

class Sphere : public Hittable {
  public:
    Sphere(Point _center, double _radius) : center(_center), radius(max(0.0, _radius)) {
      // TODO: Initialize the material pointer `mat`.
    }

    bool hit(const Ray& ray, Interval ray_t, Hit_record& rec) const override {
      // t = (-b +- sqrt(b*b - 4*a*c)) / 2*a
      Vec oc = ray.origin() - center;                      // oc = A-C
      auto a = glm::dot(ray.direction(), ray.direction()); // a = dot(B, B)
      auto half_b = glm::dot(oc, ray.direction());         // b = 2*dot(oc, B)
      auto oc_length_squared = glm::dot(oc, oc);
      auto c = oc_length_squared - radius*radius;          // c = dot(oc, oc) - R*R
      auto delta = half_b*half_b - a*c;                    // delta = b*b - 4*a*c

      // if delta is negative, there are no real roots
      // if delta is zero, there is one real root
      // if delta is positive, there are two real roots and we return the smallest one
      if (delta < 0) return false;
      auto sqrtd = sqrt(delta);

      // find the nearest root that lies in the acceptable range.
      auto root = (-half_b - sqrtd) / a;
      if (!ray_t.contains(root)) {
        root = (-half_b + sqrtd) / a; // try the other root
        if (!ray_t.contains(root))
          return false; // both roots are outside the acceptable range
      }

      // we have a hit! fill in the hit record
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
