#pragma once

#include "../utils/common.hpp"
#include "../utils/vec.hpp"
#include "pdf.hpp"

namespace raytracer {

// PDF for a cosine-weighted hemisphere
class CosinePdf : public Pdf {
  public:
    CosinePdf(const Vec& _direction) : dir(_direction) {}

    // cosine-weighted PDF
    double value(const Vec& direction) const override {
      auto cosine_theta = dot(glm::normalize(direction), dir);
      return fmax(0, cosine_theta/M_PI);
    }

    // generate a random direction cosine-weighted in the hemisphere around direction dir
    Vec generate() const override {
      return vec::random_hemisphere_cosine(dir);
    }

  private:
    Vec dir; // the direction to which the PDF is cosine-weighted
};

} // namespace raytracer
