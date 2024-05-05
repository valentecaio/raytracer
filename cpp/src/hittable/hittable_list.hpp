#ifndef HIT_LIST_H
#define HIT_LIST_H

#include "../utils/common.hpp"
#include "../primitives/primitive.hpp"
#include "hittable.hpp"
#include "hit_record.hpp"

namespace raytracer {

// The HittableList class is a list of hittable geometry objects.
class HittableList : public Hittable {
  public:
    std::vector<shared_ptr<Primitive>> objects;

    HittableList() = default;
    ~HittableList() = default;

    void clear() {
      objects.clear();
    }

    void add(shared_ptr<Primitive> object) {
      objects.push_back(object);
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& hit) const override {
      HitRecord temp_hit;
      bool hit_anything = false;
      double closest_so_far = ray_t.max;

      for (const auto& object : objects) {
        if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_hit)) {
          hit_anything = true;
          closest_so_far = temp_hit.t;
          hit = temp_hit;
        }
      }

      return hit_anything;
    }
};

} // namespace raytracer

#endif // HIT_LIST_H
