#ifndef HITRECORD_H
#define HITRECORD_H

#include "../utils/common.hpp"
#include "../ray.hpp"

namespace raytracer {

// Forward declarations to avoid circular dependencies.
class Primitive;

// The HitRecord class stores information about a ray-object intersection.
class HitRecord {
  public:
    Point p;                             // hit point
    Vec normal;                          // normal vector at the hit point, normalized
    shared_ptr<const Primitive> object;  // object that was hit
    double t;                            // ray parametrized distance at hit point
    bool front_face;                     // true if the ray hit the front face of the object

    // Sets the hit record normal vector and face orientation
    // NOTE: the parameter `outward_normal` is assumed to be normalized
    void set_face_normal(const Ray& ray, const Vec& outward_normal) {
      front_face = glm::dot(ray.direction(), outward_normal) < 0;
      normal = front_face ? outward_normal : -outward_normal;
    }
};

} // namespace raytracer

#endif // HITRECORD_H
