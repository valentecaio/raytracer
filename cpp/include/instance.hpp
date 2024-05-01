#ifndef INSTANCE_H
#define INSTANCE_H

#include "common.hpp"
#include "interval.hpp"
#include "ray.hpp"

namespace raytracer {

// these are forward declarations of the Material and Instance classes
// to avoid circular dependencies between them and the HitRecord and Instance classes
class Material;
class Instance;

// The HitRecord class stores information about a ray-object intersection.
class HitRecord {
  public:
    Point p;                            // hit point
    Vec normal;                         // normal vector at the hit point, normalized
    shared_ptr<const Instance> object;  // object that was hit
    double t;                           // ray parametrized distance at hit point
    bool front_face;                    // true if the ray hit the front face of the object

    // Sets the hit record normal vector and face orientation
    // NOTE: the parameter `outward_normal` is assumed to be normalized
    void set_face_normal(const Ray& ray, const Vec& outward_normal) {
      front_face = glm::dot(ray.direction(), outward_normal) < 0;
      normal = front_face ? outward_normal : -outward_normal;
    }
};


// The Instance class is an abstract class that represents any object that can be hit by a ray.
class Instance : public std::enable_shared_from_this<Instance> {
  public:
    shared_ptr<Material> material; // material of the object

    virtual ~Instance() = default;
    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& hitrec) const = 0;
    virtual Point get_sample() const = 0;
};

} // namespace raytracer

#endif // INSTANCE_H
