#pragma once

#include "../utils/common.hpp"
#include "../utils/interval.hpp"
#include "../ray.hpp"
#include "hit_record.hpp"

namespace raytracer {

// Abstract class that represents any type of objects that can be hit by a ray.
class Hittable {
  public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& hit) const = 0;
};

} // namespace raytracer
