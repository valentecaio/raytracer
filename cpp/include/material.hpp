#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.hpp"
#include "ray.hpp"
#include "utils.hpp"

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
    Lambertian(const Colour& albedo) : albedo(albedo) {}

    bool scatter(const Ray& r_in, const Hit_record& rec, Colour& attenuation, Ray& scattered) const override {
      auto scatter_direction = rec.normal + random_unit_vector();

      // Catch degenerate scatter direction
      if (vec_is_near_zero(scatter_direction))
        scatter_direction = rec.normal;

      scattered = Ray(rec.p, scatter_direction);
      attenuation = albedo;
      return true;
    }

  private:
    Colour albedo;
};


// A Metal material that reflects rays
class Metal : public Material {
  public:
    Metal(const Colour& albedo) : albedo(albedo) {}

    bool scatter(const Ray& r_in, const Hit_record& rec, Colour& attenuation, Ray& scattered) const override {
      Vec reflected = vec_reflect(r_in.direction(), rec.normal);
      scattered = Ray(rec.p, reflected);
      attenuation = albedo;
      return true;
    }

  private:
    Colour albedo;
};

} // namespace raytracer

#endif // MATERIAL_H
