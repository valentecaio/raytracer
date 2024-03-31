#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.hpp"
#include "vec.hpp"

namespace raytracer {

class Hit_record {
  public:
    Point p;
    Vec normal;
    double t;
    bool front_face;

    // Sets the hit record normal vector and face orientation
    // NOTE: the parameter `outward_normal` is assumed to be normalized
    void set_face_normal(const Ray& ray, const Vec& outward_normal) {
      front_face = glm::dot(ray.direction(), outward_normal) < 0;
      normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
  public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, double ray_tmin, double ray_tmax, Hit_record& rec) const = 0;
};

} // namespace raytracer

#endif // HITTABLE_H
