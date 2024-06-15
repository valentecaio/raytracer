#pragma once

#include "../utils/common.hpp"
#include "../primitives/primitive.hpp"
#include "../ray.hpp"
#include "pdf.hpp"

namespace raytracer {

// PDF for a geometric primitive
class PrimitivePdf : public Pdf {
  public:
    PrimitivePdf(const Primitive& _primitive, const Point& _origin)
      : primitive(_primitive), origin(_origin) {}

    // geometry-weighted PDF
    double value(const Vec& direction) const override {
      return primitive.pdf_value(Ray(origin, direction));
    }

    // generate a random direction according to the geometry of the primitive
    Vec generate() const override {
      // Vec v = primitive.pdf_sample(origin);
      // std::clog << "----\nPrimitivePdf::generate() generating ray from:" << std::endl;
      // vec::print(origin);
      // std::clog << "to: " << std::endl;
      // vec::print(v);
      // return v;
      return primitive.pdf_sample(origin);
    }

  private:
    const Primitive& primitive; // the primitive to which the PDF is geometry-weighted
    Point origin;               // the origin of the PDF
};

} // namespace raytracer
