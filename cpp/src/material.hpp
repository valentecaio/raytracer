#pragma once

#include "utils/common.hpp"
#include "utils/utils.hpp"
#include "utils/random.hpp"
#include "pdf.hpp"
#include "ray.hpp"

namespace raytracer {

// Forward declarations to avoid circular dependencies
class HitRecord;
class Scene;


// A record that contains the result of evaluating a material at a hit point.
class EvalRecord {
  public:
    Colour colour;       // evaluated colour of the material at the hit point
    shared_ptr<Pdf> pdf; // pointer to the pdf to be used to sample a new ray
    shared_ptr<Ray> ray; // new ray to cast, if pdf is null

    // constructor with only colour (when ray was absorbed)
    EvalRecord(const Colour& _colour)
      : colour(_colour), pdf(nullptr), ray(nullptr) {}

    // constructor with all parameters
    EvalRecord(const Colour& _colour, shared_ptr<Pdf> _pdf, shared_ptr<Ray> _ray)
      : colour(_colour), pdf(_pdf), ray(_ray) {}
};



// Abstract class that represents a material that can be applied to objects in the scene.
class Material {
  public:
    Material() = default;
    virtual ~Material() = default;

    // Evaluate a material at a hit point, returning the colour of the material,
    // a boolean indicating if a new ray should be cast, the new ray to cast
    // and the probability density function ponderation for the new ray.
    virtual EvalRecord evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit) const {
      return EvalRecord(Colour(0));
    }

    // Calculate the probability density function ponderation for the new ray
    virtual double scatter_pdf(const Vec& normal, const Ray& r_out) const {
      return 1;
    }

    // Bidirectional Reflectance Distribution Function (BRDF) ponderation for the material
    virtual double brdf_factor() const {
      return 1;
    }
};



// A material that emits light
class LightMat : public Material {
  public:
    Colour colour;    // colour of the light source
    double intensity; // intensity of the light source, used to calculate radiance

    LightMat(const Colour& _colour, double _intensity) : colour(_colour), intensity(_intensity) {}

    EvalRecord evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit) const override {
      return EvalRecord(hit.front_face() ? radiance(0) : Colour(0));
    }

    // calculate the radiance of the light at a given distance
    Colour radiance(double t) const {
      // return colour * intensity / (t*t); // TODO
      return colour * intensity;
    }
};



// A Diffuse (Lambertian) material that bounces rays in random directions
class Diffuse : public Material {
  public:
    Diffuse(const Colour& _albedo) : albedo(_albedo) {}

    EvalRecord evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit) const override {
      return EvalRecord{
        albedo,
        std::make_shared<CosinePdf>(hit.normal()),
        nullptr,
      };
    }

    // PDF for the new ray: cosine-weighted in the hemisphere around the normal
    double scatter_pdf(const Vec& normal, const Ray& r_out) const override {
      double cos_theta = std::max(glm::dot(normal, r_out.direction()), 0.0);
      return cos_theta * brdf_factor();
    }

    // BRDF for Lambertian material: 1/pi
    double brdf_factor() const override {
      return M_1_PI;
    }

  private:
    Colour albedo; // colour of the material
};



// A Metal / Mirror material that reflects rays
// if fuzz is zero, the reflection is perfect (Mirror)
class Metal : public Material {
  public:
    Metal(const Colour& _albedo, double _fuzz)
     : albedo(_albedo), fuzz(std::min(_fuzz, 1.0)) {}

    EvalRecord evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit) const override {
      // bounce the ray in a fuzzy direction
      Vec reflected = glm::reflect(r_in.direction(), hit.normal());
      reflected = glm::normalize(reflected) + (fuzz*random::sample_sphere_uniform());
      auto out_ray = make_shared<Ray>(hit.p, reflected);

      // absorb rays that bounce below the surface
      bool bounced = glm::dot(out_ray->direction(), hit.normal()) > 0;

       // TODO
      return EvalRecord{
        albedo,
        nullptr,
        bounced ? out_ray : nullptr,
      };
    }

  private:
    Colour albedo; // colour of the material
    double fuzz;   // zero for a shiny surface, one for a completely random reflection
};


// A Dielectric (glass) material that refracts rays when possible and reflects them otherwise
class Dielectric : public Material {
  public:
    Dielectric(double _refract_idx) : refract_idx(_refract_idx) {}

    EvalRecord evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit) const override {
      double ri = hit.front_face() ? (1.0/refract_idx) : refract_idx;
      double cos_theta = std::min(glm::dot(-r_in.direction(), hit.normal()), 1.0);
      double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
      bool can_refract = ri * sin_theta <= 1.0;

      Vec direction;
      if (!can_refract || utils::reflectance(cos_theta, ri) > random::rand())
        direction = glm::reflect(r_in.direction(), hit.normal());
      else
        direction = glm::refract(r_in.direction(), hit.normal(), ri);

      // TODO
      return EvalRecord{
        Colour(1), // dieletric material absorbs nothing
        nullptr,
        make_shared<Ray>(hit.p, direction),
      };
    }

  private:
    // refractive index in vacuum or air,
    // or ratio of the material's refractive index over the refractive index of the enclosing medium
    double refract_idx;
};


} // namespace raytracer
