#ifndef SCENE_H
#define SCENE_H

#include "../utils/common.hpp"
#include "../utils/interval.hpp"
#include "hittable_list.hpp"

namespace raytracer {

// Forward declarations to avoid circular dependencies.
class Light;

// A hittable scene in 3D space.
class Scene : public Hittable {
  public:
    Colour ambient_light = Colour(0, 0, 0); // scene background colour
    HittableList geometries;                // scene geometric instanced objects
    HittableList lights;                    // light sources

    Scene() = default;
    Scene(Colour _ambient_light) : ambient_light(_ambient_light) {}

    void clear() {
      geometries.clear();
      lights.clear();
    }

    void add(shared_ptr<Primitive> object) {
      if (std::dynamic_pointer_cast<Light>(object->material)) {
        lights.add(object);
      } else {
        geometries.add(object);
      }
    }

    // check if the ray intersects any object or light
    bool hit(const Ray& r, Interval ray_t, HitRecord& hitrec) const {
      HitRecord hitrec1, hitrec2;

      bool hit_object = geometries.hit(r, ray_t, hitrec1);
      bool hit_light = lights.hit(r, ray_t, hitrec2);

      if (hit_light && hit_object)
        hitrec = hitrec1.t < hitrec2.t ? hitrec1 : hitrec2;
      else if (hit_light)
        hitrec = hitrec2;
      else if (hit_object)
        hitrec = hitrec1;

      return hit_object || hit_light;;
    }
};

} // namespace raytracer

#endif // SCENE_H
