#ifndef POINT_H
#define POINT_H

namespace raytracer {

struct Point
{
  union {
    struct {
      float x, y, z, w;
    };
    struct {
      float r, g, b, a;
    };
  };

  Point() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
  Point(float x, float y, float z) : x(x), y(y), z(z), w(0.0f) {}
  Point(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

} // namespace raytracer

#endif // POINT_H
