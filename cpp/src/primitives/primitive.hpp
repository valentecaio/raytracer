#pragma once

#include "../utils/common.hpp"
#include "../hittable/hittable.hpp"

namespace raytracer {

// forward declarations to avoid circular dependencies.
class Material;
class PdfSample;

// Abstract class that represents a primitive of a geometric object in the scene.
class Primitive : public std::enable_shared_from_this<Primitive>, public Hittable {
  public:
    shared_ptr<Material> material; // material of the object
    double area;                   // area of the surface of the object

    virtual ~Primitive() = default;

    // returns a random point on the surface of the primitive
    virtual Point sample() const = 0;

    // returns a random point on the primitive with its normal and PDF
    virtual PdfSample pdf_sample() const = 0;

    // TODO: transform in pure virtual
    virtual double pdf_value(const Ray& r) const {
      return 0.0;
    }
};

} // namespace raytracer
