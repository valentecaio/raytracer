#ifndef HIT_LIST_H
#define HIT_LIST_H

#include "common.hpp"
#include "hittable_geometry.hpp"
#include "hittable.hpp"
#include "hit_record.hpp"

namespace raytracer {

// The HittableList class is a list of hittable geometry objects.
class HittableList : public Hittable {
  public:
    std::vector<shared_ptr<HittableGeometry>> objects;

    HittableList() = default;
    ~HittableList() = default;

    void clear() {
      objects.clear();
    }

    void add(shared_ptr<HittableGeometry> object) {
      objects.push_back(object);
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& hitrec) const override {
      HitRecord temp_hitrec;
      bool hit_anything = false;
      double closest_so_far = ray_t.max;

      for (const auto& object : objects) {
        if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_hitrec)) {
          hit_anything = true;
          closest_so_far = temp_hitrec.t;
          hitrec = temp_hitrec;
        }
      }

      return hit_anything;
    }
};

} // namespace raytracer

#endif // HIT_LIST_H
