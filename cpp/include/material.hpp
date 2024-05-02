#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils/common.hpp"
#include "utils/utils.hpp"
#include "utils/vec.hpp"
#include "hittable/scene.hpp"
#include "ray.hpp"

namespace raytracer {

// Forward declarations to avoid circular dependencies
class HitRecord;

// Abstract class that represents a material that can be applied to objects in the scene.
class Material {
  public:
    // constructors and destructors
    Material() = default;
    virtual ~Material() = default;

    // returns true if the ray is bounced, false otherwise
    // attenuation is the colour absorbed by the material
    // bounced is the new ray after bouncing
    virtual bool bounce(const Ray& r_in, const HitRecord& hitrec, Colour& attenuation, Ray& bounced) const {
      return false;
    }

    // returns true if the material can be shaded, false otherwise
    virtual bool shade(const Ray& r_in, const HitRecord& hitrec, const Scene& scene, Colour& result) const {
      return false;
    }

    // returns true if the material can emit light, false otherwise
    virtual bool emit(Colour& emitted) const {
      return false;
    }
};


// A material that emits light
class Light : public Material {
  public:
    Colour colour;
    double intensity;

    Light(const Colour& _colour, double _intensity) : colour(_colour), intensity(_intensity) {}

    bool emit(Colour& emitted) const override {
      emitted = colour;
      return true;
    }
};


// A Phong material that combines ambient, diffuse and specular lighting
class Phong : public Material {
  public:
    Phong(const Colour& _albedo, double _shininess) : albedo(_albedo), shininess(_shininess) {}

    bool shade(const Ray& r_in, const HitRecord& hitrec, const Scene& scene, Colour& result) const override {
      Colour ambient_term = albedo * scene.ambient_light;
      Colour diffuse_term = Colour(0, 0, 0);
      Colour specular_term = Colour(0, 0, 0);
      double nsamples = 1;

      // view direction is the opposite of the incoming ray direction (already normalized)
      Vec view_dir = -r_in.direction();

      // try to hit lights in the scene to calculate the shading
      for (const auto& light : scene.lights.objects) {
        HitRecord tmp_hitrec;
        for (int i = 0; i < static_cast<int>(nsamples); i++) {
          Point sample = light->get_sample();
          Vec light_dir = glm::normalize(sample - hitrec.p);
          auto shadow_ray = Ray(hitrec.p, light_dir);
          if (scene.hit(shadow_ray, Interval(0.0001, infinity), tmp_hitrec) && tmp_hitrec.object == light) {
            // light is visible from the hit point
            auto light_mat = std::dynamic_pointer_cast<Light>(light->material);

            // diffuse
            double NdotL = max(glm::dot(hitrec.normal, light_dir), 0.0);
            diffuse_term += albedo * light_mat->colour * light_mat->intensity * NdotL;

            // specular
            Vec reflect_dir = glm::normalize(glm::reflect(-light_dir, hitrec.normal));
            double RdotV = std::pow(max(glm::dot(reflect_dir, view_dir), 0.0), shininess);
            specular_term += light_mat->colour * light_mat->intensity * RdotV;
          }
        }
      }
      result = ambient_term + (diffuse_term/nsamples) + (specular_term/nsamples);
      return true;
    }

  private:
    Colour albedo;
    double shininess;
};


// A Diffuse material that bounces rays in random directions
class Lambertian : public Material {
  public:
    Lambertian(const Colour& _albedo) : albedo(_albedo) {}

    bool bounce(const Ray& r_in, const HitRecord& hitrec, Colour& attenuation, Ray& bounced) const override {
      attenuation = albedo;
      Vec bounce_direction = hitrec.normal + vec::random_unit();

      // catch degenerate direction
      if (vec::is_near_zero(bounce_direction))
        bounce_direction = hitrec.normal;

      bounced = Ray(hitrec.p, bounce_direction);
      return true;
    }

  private:
    Colour albedo; // colour of the material
};


// A Metal / Mirror material that reflects rays
class Metal : public Material {
  public:
    Metal(const Colour& _albedo, double _fuzz) : albedo(_albedo), fuzz(min(_fuzz, 1.0)) {}

    bool bounce(const Ray& r_in, const HitRecord& hitrec, Colour& attenuation, Ray& bounced) const override {
      attenuation = albedo;
      Vec reflected = glm::reflect(r_in.direction(), hitrec.normal);
      reflected = glm::normalize(reflected) + (fuzz*vec::random_unit());
      bounced = Ray(hitrec.p, reflected);
      return glm::dot(bounced.direction(), hitrec.normal) > 0; // absorb rays that bounce below the surface
    }

  private:
    Colour albedo; // colour of the material
    double fuzz;   // zero for a shiny surface, one for a completely random reflection
};


// A Dielectric (glass) material that refracts rays when possible and reflects them otherwise
class Dielectric : public Material {
  public:
    Dielectric(double _refraction_index) : refraction_index(_refraction_index) {}

    bool bounce(const Ray& r_in, const HitRecord& hitrec, Colour& attenuation, Ray& bounced) const override {
      attenuation = Colour(1, 1, 1); // glass absorbs nothing

      double ri = hitrec.front_face ? (1.0/refraction_index) : refraction_index;
      double cos_theta = min(glm::dot(-r_in.direction(), hitrec.normal), 1.0);
      double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
      bool can_refract = ri * sin_theta <= 1.0;

      Vec direction;
      if (!can_refract || reflectance(cos_theta, ri) > utils::random())
        direction = glm::reflect(r_in.direction(), hitrec.normal);
      else
        direction = glm::refract(r_in.direction(), hitrec.normal, ri);

      bounced = Ray(hitrec.p, direction);
      return true;
    }

  private:
    // refractive index in vacuum or air,
    // or ratio of the material's refractive index over the refractive index of the enclosing media
    double refraction_index;

    // Schlick's approximation for reflectance
    double reflectance(double cosine, double refraction_idx) const {
      double r0 = (1-refraction_idx) / (1+refraction_idx);
      r0 = r0*r0;
      return r0 + (1-r0)*pow((1 - cosine), 5);
    }
};


} // namespace raytracer

#endif // MATERIAL_H
