#ifndef HITTABLE_H
#define HITTABLE_H

#include "common.hpp"
#include "ray.hpp"
#include "interval.hpp"
#include "hit_record.hpp"

namespace raytracer {

// Abstract class that represents any type of objects that can be hit by a ray.
class Hittable {
  public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& hitrec) const = 0;
};

} // namespace raytracer

#endif // HITTABLE_H
