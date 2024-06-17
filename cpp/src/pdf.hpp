#pragma once

#include "utils/common.hpp"
#include "utils/random.hpp"
#include "hittable/hittable.hpp"
#include "primitives/primitive.hpp"

namespace raytracer {

// forward declarations
// class Hittable;

// A sample point on a surface with its normal and PDF value
class PdfSample {
  public:
    Point p;    // sample point
    Vec normal; // normal at the sample point
    double pdf; // probability density function ponderation
};


// Abstract class that represents a probability density function (PDF)
class Pdf {
  public:
    virtual ~Pdf() = default;

    // returns the value of the PDF for a given direction
    virtual double value(const Vec& direction) const = 0;

    // generate a random direction according to the PDF
    virtual Vec generate() const = 0;
};


// PDF for a cosine-weighted hemisphere
class CosinePdf : public Pdf {
  public:
    CosinePdf(const Vec& _max_direction)
     : max_direction(glm::normalize(_max_direction)) {}

    // cosine-weighted PDF
    double value(const Vec& direction) const override {
      double cos = glm::dot(glm::normalize(direction), max_direction);
      return std::max(0.0, cos/M_PI);
    }

    // generate a random direction cosine-weighted in the hemisphere around direction dir
    Vec generate() const override {
      return random::sample_hemisphere_cosine(max_direction);
    }

  private:
    Vec max_direction; // the direction to which the PDF is cosine-weighted
};


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
      return random::sample_sphere_uniform();
    }
};


// PDF for a primitive object
class PrimitivePdf : public Pdf {
  public:
    PrimitivePdf(shared_ptr<Primitive> _object, const Point& _origin)
     : object(_object), origin(_origin) {}

    double value(const Vec& direction) const override {
      return object->pdf_value(Ray(origin, direction));
    }

    // generate a random direction on the object
    Vec generate() const override {
      PdfSample sample = object->pdf_sample();
      return sample.p - origin;
    }

  private:
    shared_ptr<Primitive> object;
    Point origin;
};


// PDF for a mixture of two PDFs
class MixturePdf : public Pdf {
  public:
    MixturePdf(shared_ptr<Pdf> _pdf1, shared_ptr<Pdf> _pdf2) {
      pdf1 = _pdf1;
      pdf2 = _pdf2;
    }
    //  : pdf1(_pdf1), pdf2(_pdf2) {}

    // mixture PDF
    double value(const Vec& direction) const override {
      return 0.99 * pdf1->value(direction) + 0.01 * pdf2->value(direction);
    }

    // generate a random direction according to the mixture PDF
    Vec generate() const override {
      return random::rand() < 0.99 ? pdf1->generate() : pdf2->generate();
    }

  private:
    shared_ptr<Pdf> pdf1, pdf2;
};

} // namespace raytracer
