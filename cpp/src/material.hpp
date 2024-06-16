#pragma once

#include "utils/common.hpp"
#include "utils/utils.hpp"
#include "utils/random.hpp"
#include "pdf/cosine_pdf.hpp"
#include "scene.hpp"
#include "ray.hpp"

namespace raytracer {

// Forward declarations to avoid circular dependencies
class HitRecord;
class Sphere;


// A record that contains the result of evaluating a material at a hit point.
class EvalRecord {
  public:
    Colour colour; // evaluated colour of the material at the hit point
    bool bounced;  // true if a new ray should be cast, false if light was absorbed
    Ray ray;       // new ray to cast, if bounced is true
    double pdf;    // Probability Density Function (PDF) ponderation for the new ray
    double brdf_f; // Bidirectional Reflectance Distribution Function (BRDF) ponderation
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
      return EvalRecord{Colour(0,0,0), false, Ray(), 0, 0};
    }

    // Bidirectional Reflectance Distribution Function (BRDF) ponderation for the material
    virtual double brdf_factor(const Ray& r_in, const Ray& r_out) const {
      return 0;
    }

    // returns true if the material emits light, false otherwise
    virtual bool is_emissive() const { return false; }
};



// A material that emits light
class LightMat : public Material {
  public:
    Colour colour;    // colour of the light source
    double intensity; // intensity of the light source, used to calculate radiance

    LightMat(const Colour& _colour, double _intensity) : colour(_colour), intensity(_intensity) {}

    EvalRecord evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit) const override {
      return EvalRecord{
        hit.front_face() ? radiance(0) : Colour(0,0,0),
        false,
        Ray(),
        0,
        0,
      };
    }

    // calculate the radiance of the light at a given distance
    Colour radiance(double t) const {
      // return colour * intensity / (t*t); // TODO
      return colour * intensity;
    }

    // light sources are emissive
    bool is_emissive() const override { return true; }
};



// A Phong material that combines ambient, diffuse and specular lighting
class Phong : public Material {
  public:
    Phong(const Colour& _albedo, double _shininess)
      : albedo(_albedo), shininess(_shininess) {}

    Phong(const Colour& _albedo, double _shininess, double _ka, double _kd, double _ks)
      : albedo(_albedo), shininess(_shininess), ka(_ka), kd(_kd), ks(_ks) {}

    EvalRecord evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit) const override {
      return EvalRecord{
        phong_shade(r_in, hit, scene),
        false,
        Ray(),
        0,
        0,
      };
    }

  protected:
    Colour phong_shade(const Ray& r_in, const HitRecord& hit, const Scene& scene) const {
      Colour total_amb = scene.ambient_light;
      Colour total_diff = Colour(0,0,0);
      Colour total_spec = Colour(0,0,0);

      // view direction is the opposite of the incoming ray direction (already normalized)
      Vec view_dir = -r_in.direction();

      // try to hit lights in the scene to calculate the shading
      for (const auto& light : scene.lights.objects) {
        HitRecord shadow_hit;
        auto diff = Colour(0,0,0);
        auto spec = Colour(0,0,0);

        // point lights are sampled once, area lights are sampled multiple times
        int nsamples = (std::dynamic_pointer_cast<Sphere>(light)) ? 1 : 10;
        for (int i = 0; i < nsamples; i++) {
          Point sample = light->sample();
          Vec light_dir = glm::normalize(sample - hit.p);

          auto shadow_ray = Ray(hit.p, light_dir);
          if (scene.hit(shadow_ray, Interval(0.0001, infinity), shadow_hit)
              && shadow_hit.object == light) {
            // light is visible from the hit point
            auto light_mat = std::dynamic_pointer_cast<LightMat>(light->material);

            // diffuse
            Vec light_radiance = light_mat->radiance(glm::length(sample - hit.p));
            double attenuation = std::max(glm::dot(hit.normal(), light_dir), 0.0);
            diff += attenuation * light_radiance;

            // specular
            Vec reflect_dir = glm::normalize(glm::reflect(-light_dir, hit.normal()));
            double RdotV = std::pow(std::max(glm::dot(reflect_dir, view_dir), 0.0), shininess);
            spec += light_radiance * RdotV;
          }
        }
        total_diff += diff/(double)nsamples;
        total_spec += spec/(double)nsamples;
      }
      return albedo * (ka*total_amb + kd*total_diff + ks*total_spec);
    }

  private:
    Colour albedo;    // colour of the material
    double shininess; // shininess of the material
    double ka = 0.5;  // ambient coefficient
    double kd = 0.5;  // diffuse coefficient
    double ks = 0.5;  // specular coefficient
};



// A PhongMirror material that combines Phong shading with reflection using Schlick's approximation
class PhongMirror : public Phong {
  public:
    PhongMirror(const Colour& _albedo, double _shininess, double _refract_idx)
      : Phong(_albedo, _shininess), refract_idx(_refract_idx) {}

    PhongMirror(const Colour& _albedo, double _shininess, double _ka, double _kd, double _ks, double _refract_idx)
      : Phong(_albedo, _shininess, _ka, _kd, _ks), refract_idx(_refract_idx) {}

    EvalRecord evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit) const override {
      // reflection ray
      Vec reflected = glm::normalize(glm::reflect(r_in.direction(), hit.normal()));
      Ray reflect_ray(hit.p, reflected);

      // evaluate the reflected ray colour
      HitRecord reflect_hit;
      Colour reflect_colour;
      if (scene.hit(reflect_ray, Interval(0.0001, infinity), reflect_hit)) {
        // hit, evaluate the material
        EvalRecord reflect_eval = reflect_hit.object->material->evaluate(scene, reflect_ray, reflect_hit);
        reflect_colour = reflect_eval.colour;
      } else {
        // miss, use scene background colour
        reflect_colour = scene.background;
      }

      // reflectance
      double cos_theta = std::min(glm::dot(-r_in.direction(), hit.normal()), 1.0);
      double R = utils::reflectance(cos_theta, refract_idx);

      // final colour is a mix of the phong shading and reflected colour
      return EvalRecord{
        (1-R)*phong_shade(r_in, hit, scene) + R*reflect_colour,
        false,
        Ray(),
        0,
        0,
      };
    }

  private:
    double refract_idx; // refractive index in vacuum or air,
};



// A Diffuse (Lambertian) material that bounces rays in random directions
class Diffuse : public Material {
  public:
    Diffuse(const Colour& _albedo) : albedo(_albedo) {}

    EvalRecord evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit) const override {
      // get a random direction from the surface and calculate the associated PDF
      CosinePdf surface_pdf(hit.normal());
      Ray out_ray = Ray(hit.p, surface_pdf.generate());
      double pdf = surface_pdf.value(out_ray.direction());
      return EvalRecord{
        albedo,
        true,
        out_ray,
        pdf,
        brdf_factor(r_in, out_ray)
      };
    }

    // BRDF for Lambertian material: 1/pi
    double brdf_factor(const Ray& r_in, const Ray& r_out) const override {
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
      Ray out_ray = Ray(hit.p, reflected);

      // absorb rays that bounce below the surface
      bool bounced = glm::dot(out_ray.direction(), hit.normal()) > 0;

      return EvalRecord{albedo, bounced, out_ray, 0, 0}; // TODO: pdf, brdf
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

      return EvalRecord{
        Colour(1, 1, 1), // dieletric material absorbs nothing
        true,
        Ray(hit.p, direction),
        0, // TODO
        0,
      };
    }

  private:
    // refractive index in vacuum or air,
    // or ratio of the material's refractive index over the refractive index of the enclosing medium
    double refract_idx;
};


} // namespace raytracer
