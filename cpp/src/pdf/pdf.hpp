#pragma once

#include "../utils/common.hpp"

namespace raytracer {

// Abstract class that represents a probability density function (PDF)
class Pdf {
  public:
    virtual ~Pdf() = default;

    // returns the value of the PDF for a given direction
    virtual double value(const Vec& direction) const = 0;

    // generate a random direction according to the PDF
    virtual Vec generate() const = 0;
};

} // namespace raytracer
