#ifndef RAY_H
#define RAY_H

#include "vec.hpp"

namespace raytracer {

class Ray {
  public:
    Ray() {}
    Ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }

    point3 at(double t) const {
      return orig + t*dir;
    }

  private:
    point3 orig; // origin
    vec3 dir;    // direction
};

} // namespace raytracer

#endif // RAY_H
