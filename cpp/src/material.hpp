#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils/common.hpp"
#include "utils/utils.hpp"
#include "utils/vec.hpp"
// #include "primitives/2d.hpp"
#include "scene.hpp"
#include "ray.hpp"

namespace raytracer {

// Forward declarations to avoid circular dependencies
class HitRecord;
class Quad;

// Abstract class that represents a material that can be applied to objects in the scene.
class Material {
  public:
    // constructors and destructors
    Material() = default;
    virtual ~Material() = default;

    // returns true if the a new ray should be cast, false otherwise
    // out_colour is the colour absorbed by the material
    // out_ray is the new ray to cast, only valid if the return value is true
    virtual bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hitrec, Colour& out_colour, Ray& out_ray) const {
      return false;
    }
};



// A material that emits light
class Light : public Material {
  public:
    Colour colour;    // colour of the light source
    double intensity; // intensity of the light source, used to calculate radiance

    Light(const Colour& _colour, double _intensity) : colour(_colour), intensity(_intensity) {}

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hitrec, Colour& out_colour, Ray& out_ray) const override {
      out_colour = colour;
      return false;
    }

    // calculate the radiance of the light at a given distance
    Colour radiance(double t) const {
      // return colour * intensity / (t*t);
      return colour * intensity;
    }
};



// A Phong material that combines ambient, diffuse and specular lighting
class Phong : public Material {
  public:
    Phong(const Colour& _albedo, double _shininess) : albedo(_albedo), shininess(_shininess) {}

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hitrec, Colour& out_colour, Ray& out_ray) const override {
      out_colour = phong_shade(r_in, hitrec, scene);
      return false;
    }

  protected:
    Colour phong_shade(const Ray& r_in, const HitRecord& hitrec, const Scene& scene) const {
      Colour ambient_term = albedo * scene.ambient_light;
      Colour total_diff = Colour(0,0,0);
      Colour total_spec = Colour(0,0,0);

      // view direction is the opposite of the incoming ray direction (already normalized)
      Vec view_dir = -r_in.direction();

      // try to hit lights in the scene to calculate the shading
      for (const auto& light : scene.lights.objects) {
        HitRecord shadow_hitrec;
        auto diff = Colour(0,0,0);
        auto spec = Colour(0,0,0);

        // point lights are sampled once, area lights are sampled multiple times
        int nsamples = (std::dynamic_pointer_cast<Quad>(light)) ? 10 : 1;
        for (int i = 0; i < nsamples; i++) {
          Point sample = light->get_sample();
          Vec light_dir = glm::normalize(sample - hitrec.p);
          auto shadow_ray = Ray(hitrec.p, light_dir);
          if (scene.hit(shadow_ray, Interval(0.0001, infinity), shadow_hitrec) && shadow_hitrec.object == light) {
            // light is visible from the hit point
            auto light_mat = std::dynamic_pointer_cast<Light>(light->material);

            // diffuse
            double distance = glm::length(sample - hitrec.p);
            Colour radiance = light_mat->radiance(distance);
            double attenuation = std::max(glm::dot(hitrec.normal, light_dir), 0.0);
            diff += albedo * radiance * attenuation;

            // specular
            Vec reflect_dir = glm::normalize(glm::reflect(-light_dir, hitrec.normal));
            double RdotV = std::pow(std::max(glm::dot(reflect_dir, view_dir), 0.0), shininess);
            spec += light_mat->colour * light_mat->intensity * RdotV;
          }
        }
        total_diff += diff/(double)nsamples;
        total_spec += spec/(double)nsamples;
      }
      return ambient_term + total_diff + total_spec;
    }

  private:
    Colour albedo;   // mamb, mdiff
    double shininess;
};



// A PhongMirror material that combines Phong shading with reflection using Schlick's approximation
// This material works differently from the other materials.
// Instead of calling the evaluate() method, the raytracer must first use the get_reflected_ray()
// method to calculate the reflection and then use the evaluate_mirror() method
// to mix the phong and reflection colours.
class PhongMirror : public Phong {
  public:
    PhongMirror(const Colour& _albedo, double _shininess, double _refraction_index)
     : Phong(_albedo, _shininess), refraction_index(_refraction_index) {}

    // get the ray that must be cast to calculate the reflection
    Ray get_reflected_ray(const Ray& r_in, const HitRecord& hitrec) const {
      Vec reflected = glm::normalize(glm::reflect(r_in.direction(), hitrec.normal));
      return Ray(hitrec.p, reflected);
    }

    // mix light colours according to reflectance
    Colour evaluate_mirror(const Scene& scene, const Ray& r_in, const HitRecord& hitrec, const Colour& reflection_colour) const {
      double cos_theta = std::min(glm::dot(-r_in.direction(), hitrec.normal), 1.0);
      double R = utils::reflectance(cos_theta, refraction_index);
      return (1-R)*phong_shade(r_in, hitrec, scene) + R*reflection_colour;
    }

  private:
    double refraction_index; // refractive index in vacuum or air,
};



// A Diffuse (Lambertian) material that bounces rays in random directions
class Diffuse : public Material {
  public:
    Diffuse(const Colour& _albedo) : albedo(_albedo) {}

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hitrec, Colour& out_colour, Ray& out_ray) const override {
      out_colour = albedo;

      // bounce the ray in a random direction
      Vec bounce_direction = hitrec.normal + vec::random_unit();
      if (vec::is_near_zero(bounce_direction)) // degenerate direction
        bounce_direction = hitrec.normal;
      out_ray = Ray(hitrec.p, bounce_direction);

      return true;
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

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hitrec, Colour& out_colour, Ray& out_ray) const override {
      out_colour = albedo;

      // bounce the ray in a fuzzy direction
      Vec reflected = glm::reflect(r_in.direction(), hitrec.normal);
      reflected = glm::normalize(reflected) + (fuzz*vec::random_unit());
      out_ray = Ray(hitrec.p, reflected);

      // absorb rays that bounce below the surface
      return glm::dot(out_ray.direction(), hitrec.normal) > 0;
    }

  private:
    Colour albedo; // colour of the material
    double fuzz;   // zero for a shiny surface, one for a completely random reflection
};


// A Dielectric (glass) material that refracts rays when possible and reflects them otherwise
class Dielectric : public Material {
  public:
    Dielectric(double _refraction_index) : refraction_index(_refraction_index) {}

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hitrec, Colour& out_colour, Ray& out_ray) const override {
      // dieletric material absorbs nothing
      out_colour = Colour(1, 1, 1);

      double ri = hitrec.front_face ? (1.0/refraction_index) : refraction_index;
      double cos_theta = std::min(glm::dot(-r_in.direction(), hitrec.normal), 1.0);
      double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
      bool can_refract = ri * sin_theta <= 1.0;

      Vec direction;
      if (!can_refract || utils::reflectance(cos_theta, ri) > utils::random())
        direction = glm::reflect(r_in.direction(), hitrec.normal);
      else
        direction = glm::refract(r_in.direction(), hitrec.normal, ri);
      out_ray = Ray(hitrec.p, direction);

      return true;
    }

  private:
    // refractive index in vacuum or air,
    // or ratio of the material's refractive index over the refractive index of the enclosing medium
    double refraction_index;
};


} // namespace raytracer

#endif // MATERIAL_H
