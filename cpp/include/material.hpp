#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.hpp"
#include "ray.hpp"
#include "utils.hpp"
#include "vec.hpp"

namespace raytracer {

class Hit_record;

class Material {
  public:
    virtual ~Material() = default;

    virtual bool scatter(const Ray& r_in,
                         const Hit_record& rec,
                         Colour& attenuation,
                         Ray& scattered) const {
      return false;
    }
};


// A Diffuse material that scatters rays in random directions
class Lambertian : public Material {
  public:
    Lambertian(const Colour& _albedo) : albedo(_albedo) {}

    bool scatter(const Ray& r_in, const Hit_record& rec, Colour& attenuation, Ray& scattered) const override {
      auto scatter_direction = rec.normal + vec::random_unit();

      // catch degenerate scatter direction
      if (vec::is_near_zero(scatter_direction))
        scatter_direction = rec.normal;

      scattered = Ray(rec.p, scatter_direction);
      attenuation = albedo;
      return true;
    }

  private:
    Colour albedo; // colour of the material
};


// A Metal material that reflects rays
class Metal : public Material {
  public:
    Metal(const Colour& _albedo, double _fuzz) : albedo(_albedo), fuzz(min(_fuzz, 1.0)) {}

    bool scatter(const Ray& r_in, const Hit_record& rec, Colour& attenuation, Ray& scattered) const override {
      auto reflected = glm::reflect(r_in.direction(), rec.normal);
      reflected = glm::normalize(reflected) + (fuzz*vec::random_unit());
      scattered = Ray(rec.p, reflected);
      attenuation = albedo;
      return glm::dot(scattered.direction(), rec.normal) > 0; // absorb rays that scatter below the surface
    }

  private:
    Colour albedo; // colour of the material
    double fuzz;   // zero for a shiny surface, one for a completely random reflection
};


// A Dielectric material that always refracts rays
class Dielectric : public Material {
  public:
    Dielectric(double _refraction_index) : refraction_index(_refraction_index) {}

    bool scatter(const Ray& r_in, const Hit_record& rec, Colour& attenuation, Ray& scattered) const override {
      attenuation = Colour(1, 1, 1);
      double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

      double cos_theta = min(glm::dot(-r_in.direction(), rec.normal), 1.0);
      double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

      bool can_refract = ri * sin_theta <= 1.0;

      Vec direction;
      if (can_refract)
        direction = glm::refract(r_in.direction(), rec.normal, ri);
      else
        direction = glm::reflect(r_in.direction(), rec.normal);

      scattered = Ray(rec.p, direction);
      return true;
    }

  private:
    // Refractive index in vacuum or air,
    // or ratio of the material's refractive index over the refractive index of the enclosing media
    double refraction_index;
};

} // namespace raytracer

#endif // MATERIAL_H
