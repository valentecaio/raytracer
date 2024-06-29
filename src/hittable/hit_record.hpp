#pragma once

#include "../utils/common.hpp"
#include "../ray.hpp"

namespace raytracer {

// Forward declarations to avoid circular dependencies.
class Primitive;

// The HitRecord class stores information about a ray-object intersection.
class HitRecord {
  public:
    Point p;                             // hit point
    shared_ptr<const Primitive> object;  // object that was hit
    double t;                            // ray parametrized distance at hit point

    // getters
    Vec normal() const { return m_normal; }
    bool front_face() const { return m_front_face; }

    // Sets the hit record normal vector and face orientation
    // NOTE: the parameter `outward_normal` is assumed to be normalized
    void set_normal(const Ray& ray, const Vec& outward_normal) {
      m_front_face = glm::dot(ray.direction(), outward_normal) < 0;
      m_normal = m_front_face ? outward_normal : -outward_normal;
    }

  private:
    Vec m_normal;                          // normal vector at the hit point, normalized
    bool m_front_face;                     // true if the ray hit the front face of the object
};

} // namespace raytracer
