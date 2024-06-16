#pragma once

#include "../utils/common.hpp"
#include "../primitives/primitive.hpp"
#include "../ray.hpp"
#include "pdf.hpp"

namespace raytracer {

class PdfSample {
  public:
    Point p;    // sample point
    Vec normal; // normal at the sample point
    double pdf; // probability density function ponderation
};

} // namespace raytracer
