#pragma once

#include "../utils/common.hpp"
#include "pdf.hpp"

namespace raytracer {

// PDF for a sphere
class SpherePdf : public Pdf {
  public:
    SpherePdf() = default;

    // uniform PDF for a sphere of radius 1
    double value(const Vec& direction) const override {
      return 1 / (4*M_PI);
    }

    // generate a random direction on the unit sphere
    Vec generate() const override {
      return random_unit_vector();
    }
};

} // namespace raytracer
