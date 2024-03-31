#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <memory> // shared_ptr, make_shared
#include <vector>

#include "hittable.hpp"

using std::shared_ptr;
// using std::make_shared;

namespace raytracer {

class Hittable_list : public Hittable {
  public:
    std::vector<shared_ptr<Hittable>> objects;

    Hittable_list() {}
    Hittable_list(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<Hittable> object) {
      objects.push_back(object);
    }

    bool hit(const Ray& r, double ray_tmin, double ray_tmax, Hit_record& rec) const override {
      Hit_record temp_rec;
      bool hit_anything = false;
      auto closest_so_far = ray_tmax;

      for (const auto& object : objects) {
        if (object->hit(r, ray_tmin, closest_so_far, temp_rec)) {
          hit_anything = true;
          closest_so_far = temp_rec.t;
          rec = temp_rec;
        }
      }

      return hit_anything;
    }
};

} // namespace raytracer

#endif // HITTABLE_LIST_H
