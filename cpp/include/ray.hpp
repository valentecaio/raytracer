#ifndef RAY_H
#define RAY_H

#include "vec.hpp"

namespace raytracer {

class Ray {
  public:
    Ray() {}
    Ray(const Point& origin, const Vec& direction) : orig(origin), dir(direction) {}

    Point origin() const { return orig; }
    Vec direction() const { return dir; }

    Point at(double t) const {
      return orig + t*dir;
    }

  private:
    Point orig; // origin
    Vec dir;    // direction
};

} // namespace raytracer

#endif // RAY_H
