#ifndef RAY_H
#define RAY_H

#include "common.hpp"

namespace raytracer {

class Ray {
  public:
    Ray() {}
    Ray(const Point& origin, const Vec& direction) : orig(origin), dir(glm::normalize(direction)) {}

    Point origin() const { return orig; }
    Vec direction() const { return dir; }

    Point at(double t) const {
      return orig + t*dir;
    }

  private:
    Point orig; // origin
    Vec dir;    // direction, normalized
};

} // namespace raytracer

#endif // RAY_H
