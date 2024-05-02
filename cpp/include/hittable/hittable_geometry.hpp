#ifndef HIT_GEOMETRY_HPP
#define HIT_GEOMETRY_HPP

#include "../common.hpp"
#include "hittable.hpp"

namespace raytracer {

// forward declarations to avoid circular dependencies.
class Material;

// Abstract class that represents an instance of a geometric object in the scene.
class HittableGeometry : public std::enable_shared_from_this<HittableGeometry>, public Hittable {
  public:
    shared_ptr<Material> material; // material of the object

    virtual ~HittableGeometry() = default;
    virtual Point get_sample() const = 0;
};

} // namespace raytracer

#endif // HIT_GEOMETRY_HPP
