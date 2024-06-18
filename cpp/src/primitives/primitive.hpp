#pragma once

#include "../utils/common.hpp"
#include "../hittable/hittable.hpp"

namespace raytracer {

// forward declarations to avoid circular dependencies.
class Material;


// A sample point (with its normal) on a surface of a primitive
class Sample {
  public:
    Point p;    // sample point in global coordinates
    Vec normal; // normal at the sample point
    double pdf; // probability density function ponderation - TODO: not used yet
};


// Abstract class that represents a primitive of a geometric object in the scene.
class Primitive : public std::enable_shared_from_this<Primitive>, public Hittable {
  public:
    shared_ptr<Material> material; // material of the object
    double area;                   // area of the surface of the object

    virtual ~Primitive() = default;

    // returns a random point on the surface of the primitive
    virtual Point sample() const = 0;

    // returns a random point on the primitive with its normal and PDF
    // TODO: transform in pure virtual
    virtual Sample pdf_sample() const {
      return Sample{sample(), Vec(0, 0, 0)};
    }

    // returns the probability density function of the primitive for a given ray
    // TODO: transform in pure virtual
    virtual double pdf_value(const Ray& r) const {
      return 0.0;
    }
};

} // namespace raytracer
