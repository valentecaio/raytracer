#ifndef BOX_HPP
#define BOX_HPP

#include "../utils/common.hpp"
#include "../utils/interval.hpp"
#include "../hittable/hit_record.hpp"
#include "../hittable/hittable_geometry.hpp"
#include "../material.hpp"
#include "quad.hpp"

namespace raytracer {

// A hittable box in 3D space defined by a list of 6 quads.
class Box : public HittableList, public Primitive {
  public:
    Box(const Point& a, const Point& b, shared_ptr<Material> mat) {
      auto pmin = Point(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
      auto pmax = Point(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));

      auto dx = Vec(pmax.x - pmin.x, 0, 0);
      auto dy = Vec(0, pmax.y - pmin.y, 0);
      auto dz = Vec(0, 0, pmax.z - pmin.z);

      add(make_shared<Quad>(Point(pmin.x, pmin.y, pmax.z), dx, dy, mat)); // front
      add(make_shared<Quad>(Point(pmin.x, pmin.y, pmin.z), dx, dy, mat)); // back
      add(make_shared<Quad>(Point(pmin.x, pmin.y, pmin.z), dy, dz, mat)); // left
      add(make_shared<Quad>(Point(pmax.x, pmin.y, pmin.z), dy, dz, mat)); // right
      add(make_shared<Quad>(Point(pmin.x, pmax.y, pmin.z), dx, dz, mat)); // top
      add(make_shared<Quad>(Point(pmin.x, pmin.y, pmin.z), dx, dz, mat)); // bottom

      material = mat;
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& hitrec) const override {
      // the ray intersects the box if it intersects any of the 6 quads
      // the only correction needed is to set the object pointer to the box
      if (HittableList::hit(r, ray_t, hitrec)) {
        hitrec.object = shared_from_this();
        return true;
      }
      return false;
    }

    // returns a random point in one of the 6 box faces
    Point get_sample() const override {
      int face = utils::random_int(0, 5);
      return objects[face].get()->get_sample();
    }
};

} // namespace raytracer

#endif // BOX_HPP
