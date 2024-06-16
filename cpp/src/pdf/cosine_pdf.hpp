#pragma once

#include "../utils/common.hpp"
#include "../utils/vec.hpp"
#include "pdf.hpp"

namespace raytracer {

// PDF for a cosine-weighted hemisphere
class CosinePdf : public Pdf {
  public:
    CosinePdf(const Vec& _max_direction)
     : max_direction(glm::normalize(_max_direction)) {}

    // cosine-weighted PDF
    double value(const Vec& direction) const override {
      auto cosine_theta = glm::dot(glm::normalize(direction), max_direction);
      return std::max(0.0, cosine_theta/M_PI);
    }

    // generate a random direction cosine-weighted in the hemisphere around direction dir
    Vec generate() const override {
      return vec::random_hemisphere_cosine(max_direction);
    }

  private:
    Vec max_direction; // the direction to which the PDF is cosine-weighted
};

} // namespace raytracer
