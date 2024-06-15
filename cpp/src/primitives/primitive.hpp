#pragma once

#include "../utils/common.hpp"
#include "../hittable/hittable.hpp"

namespace raytracer {

// forward declarations to avoid circular dependencies.
class Material;

// Abstract class that represents a primitive of a geometric object in the scene.
class Primitive : public std::enable_shared_from_this<Primitive>, public Hittable {
  public:
    shared_ptr<Material> material; // material of the object

    virtual ~Primitive() = default;

    // returns a random point on the primitive
    virtual Point get_sample() const = 0;

    // generate a random direction for a ray departing from the origin,
    // according to the geometry of the primitive.
    // default implementation returns works for quads
    virtual Vec pdf_sample(const Point& origin) const {
      // Vec s = get_sample();
      // std::clog << "s: " << std::endl;
      // vec::print(s);
      // std::clog << "origin: " << std::endl;
      // vec::print(origin);
      // return glm::normalize(s - origin);
      return get_sample() - origin;
    }

    // returns the PDF of the primitive for the given ray
    virtual double pdf_value(const Ray& r) const {
      return 0.0;
    }
};

} // namespace raytracer
