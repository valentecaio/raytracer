#ifndef HITTABLE_H
#define HITTABLE_H

#include "common.hpp"
#include "interval.hpp"
#include "ray.hpp"

namespace raytracer {

// this is a forward declaration of the material class
// to avoid circular dependencies between the hittable and material classes
class Material;

// The HitRecord class stores information about a ray-object intersection.
class HitRecord {
  public:
    Point p;                       // hit point
    Vec normal;                    // normal vector at the hit point
    shared_ptr<Material> material; // material of the object that was hit
    double t;                      // ray parametrized distance at hit point
    bool front_face;               // true if the ray hit the front face of the object

    // Sets the hit record normal vector and face orientation
    // NOTE: the parameter `outward_normal` is assumed to be normalized
    void set_face_normal(const Ray& ray, const Vec& outward_normal) {
      front_face = glm::dot(ray.direction(), outward_normal) < 0;
      normal = front_face ? outward_normal : -outward_normal;
    }
};


// The Hittable class is an abstract class that represents any object that can be hit by a ray.
class Hittable {
  public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;
};

} // namespace raytracer

#endif // HITTABLE_H
