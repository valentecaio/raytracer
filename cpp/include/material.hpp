#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.hpp"
#include "ray.hpp"
#include "utils.hpp"
#include "vec.hpp"

namespace raytracer {

class HitRecord;

class Material {
  public:
    // constructors and destructors
    Material() = default;
    virtual ~Material() = default;

    // returns true if the ray is bounced, false otherwise
    // attenuation is the colour absorbed by the material
    // bounced is the new ray after bouncing
    // rec is the hit record
    virtual bool bounce(const Ray& r_in, const HitRecord& rec, Colour& attenuation, Ray& bounced) const {
      return false;
    }

    // returns true if the material can emit light, false otherwise
    virtual bool emit(Colour& emitted) const {
      return false;
    }
};


// A Diffuse material that bounces rays in random directions
class Lambertian : public Material {
  public:
    Lambertian(const Colour& _albedo) : albedo(_albedo) {}

    bool bounce(const Ray& r_in, const HitRecord& rec, Colour& attenuation, Ray& bounced) const override {
      Vec bounce_direction = rec.normal + vec::random_unit();

      // catch degenerate direction
      if (vec::is_near_zero(bounce_direction))
        bounce_direction = rec.normal;

      bounced = Ray(rec.p, bounce_direction);
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

    bool bounce(const Ray& r_in, const HitRecord& rec, Colour& attenuation, Ray& bounced) const override {
      Vec reflected = glm::reflect(r_in.direction(), rec.normal);
      reflected = glm::normalize(reflected) + (fuzz*vec::random_unit());
      bounced = Ray(rec.p, reflected);
      attenuation = albedo;
      return glm::dot(bounced.direction(), rec.normal) > 0; // absorb rays that bounce below the surface
    }

  private:
    Colour albedo; // colour of the material
    double fuzz;   // zero for a shiny surface, one for a completely random reflection
};


// A Dielectric material that always refracts rays (glass)
class Dielectric : public Material {
  public:
    Dielectric(double _refraction_index) : refraction_index(_refraction_index) {}

    bool bounce(const Ray& r_in, const HitRecord& rec, Colour& attenuation, Ray& bounced) const override {
      attenuation = Colour(1, 1, 1);
      double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

      double cos_theta = min(glm::dot(-r_in.direction(), rec.normal), 1.0);
      double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

      bool can_refract = ri * sin_theta <= 1.0;

      Vec direction;
      if (!can_refract || reflectance(cos_theta, ri) > random_double())
        direction = glm::reflect(r_in.direction(), rec.normal);
      else
        direction = glm::refract(r_in.direction(), rec.normal, ri);

      bounced = Ray(rec.p, direction);
      return true;
    }

  private:
    // Refractive index in vacuum or air,
    // or ratio of the material's refractive index over the refractive index of the enclosing media
    double refraction_index;

    // Schlick's approximation for reflectance
    double reflectance(double cosine, double refraction_idx) const {
      double r0 = (1-refraction_idx) / (1+refraction_idx);
      r0 = r0*r0;
      return r0 + (1-r0)*pow((1 - cosine), 5);
    }
};


// A light material that emits light
class Light : public Material {
  public:
    Light(const Colour& _emission) { emission_colour = _emission; }

    bool bounce(const Ray& r_in, const HitRecord& rec, Colour& attenuation, Ray& bounced) const override {
      return false;
    }

    bool emit(Colour& emitted) const override {
      emitted = emission_colour;
      return true;
    }

  private:
    Colour emission_colour;
};


} // namespace raytracer

#endif // MATERIAL_H
