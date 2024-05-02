#ifndef SCENE_H
#define SCENE_H

#include "common.hpp"
#include "instance.hpp"
#include "interval.hpp"

namespace raytracer {

// forward declarations
class Light;

class Scene {
  public:
    std::vector<shared_ptr<Instance>> objects;     // scene instanced objects
    std::vector<shared_ptr<Instance>> lights;      // light sources
    Colour ambient_light_colour = Colour(0, 0, 0); // scene background colour

    Scene() = default;
    Scene(Colour _ambient_light_colour) : ambient_light_colour(_ambient_light_colour) {}

    void clear() {
      objects.clear();
      lights.clear();
    }

    void add(shared_ptr<Instance> object) {
      if (std::dynamic_pointer_cast<Light>(object->material)) {
        lights.push_back(object);
      } else {
        objects.push_back(object);
      }
    }

    // check if the ray intersects any object in the list
    bool hit(const Ray& r, Interval ray_t, HitRecord& hitrec) const {
      HitRecord temp_hitrec;
      bool hit_anything = false;
      double closest_so_far = ray_t.max;

      // hit objects
      for (const auto& object : objects) {
        if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_hitrec)) {
          hit_anything = true;
          closest_so_far = temp_hitrec.t;
          hitrec = temp_hitrec;
        }
      }

      // hit lights
      for (const auto& light : lights) {
        if (light->hit(r, Interval(ray_t.min, closest_so_far), temp_hitrec)) {
          hit_anything = true;
          closest_so_far = temp_hitrec.t;
          hitrec = temp_hitrec;
        }
      }

      return hit_anything;
    }
};

} // namespace raytracer

#endif // SCENE_H
