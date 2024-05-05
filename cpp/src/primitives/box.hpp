#ifndef BOX_H
#define BOX_H

#include "../utils/common.hpp"
#include "../utils/interval.hpp"
#include "../hittable/hit_record.hpp"
#include "../material.hpp"
#include "primitive.hpp"
#include "2d.hpp"

namespace raytracer {

// A hittable box in 3D space defined by a list of 6 quads.
class Box : public Primitive {
  public:
    Point pmin, pmax; // unused but useful for debug

    Box() = default;
    ~Box() = default;

    Box(const Point& _a, const Point& _b, shared_ptr<Material> _mat) {
      pmin = Point(std::min(_a.x, _b.x), std::min(_a.y, _b.y), std::min(_a.z, _b.z));
      pmax = Point(std::max(_a.x, _b.x), std::max(_a.y, _b.y), std::max(_a.z, _b.z));

      auto dx = Vec(pmax.x - pmin.x, 0, 0);
      auto dy = Vec(0, pmax.y - pmin.y, 0);
      auto dz = Vec(0, 0, pmax.z - pmin.z);

      // using right-hand rule so that normal vectors point outwards
      faces.add(make_shared<Quad>(pmax, -dx, -dy, _mat)); // front
      faces.add(make_shared<Quad>(pmin,  dx,  dy, _mat)); // back
      faces.add(make_shared<Quad>(pmin,  dz,  dy, _mat)); // left
      faces.add(make_shared<Quad>(pmax, -dy, -dz, _mat)); // right
      faces.add(make_shared<Quad>(pmax, -dz, -dx, _mat)); // top
      faces.add(make_shared<Quad>(pmin,  dx,  dz, _mat)); // bottom

      material = _mat;
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& hitrec) const override {
      // the ray intersects the box if it intersects any of the 6 quads
      // the only correction needed is to set the object pointer to the box
      return faces.hit(r, ray_t, hitrec) && (hitrec.object = shared_from_this());
    }

    // returns a random point in one of the 6 box faces
    Point get_sample() const override {
      int idx = utils::random_int(0, 5);
      return faces.objects[idx].get()->get_sample();
    }

  private:
    HittableList faces;
};

} // namespace raytracer

#endif // BOX_H
