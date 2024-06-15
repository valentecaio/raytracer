#pragma once

#include "utils/common.hpp"
#include "utils/interval.hpp"
#include "hittable/hittable_list.hpp"

namespace raytracer {

// Forward declarations to avoid circular dependencies.
class LightMat;

// A hittable scene in 3D space.
class Scene : public Hittable {
  public:
    Colour ambient_light = Colour(0, 0, 0); // scene ambient light colour, used in Phong
    Colour background = Colour(0, 0, 0);    // scene background colour
    HittableList primitives;                // scene geometric instanced objects
    HittableList lights;                    // light sources

    Scene() = default;
    Scene(Colour _ambient_light) : ambient_light(_ambient_light) {}

    void clear() {
      primitives.clear();
      lights.clear();
    }

    void add(shared_ptr<Primitive> object) {
      if (std::dynamic_pointer_cast<LightMat>(object->material)) {
        lights.add(object);
      } else {
        primitives.add(object);
      }
    }

    // check if the ray intersects any object or light
    bool hit(const Ray& r, Interval ray_t, HitRecord& hit) const {
      HitRecord hit1, hit2;

      bool hit_object = primitives.hit(r, ray_t, hit1);
      bool hit_light = lights.hit(r, ray_t, hit2);

      if (hit_light && hit_object)
        hit = hit1.t < hit2.t ? hit1 : hit2;
      else if (hit_light)
        hit = hit2;
      else if (hit_object)
        hit = hit1;

      return hit_object || hit_light;;
    }
};

} // namespace raytracer
