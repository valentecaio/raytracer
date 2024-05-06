#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils/common.hpp"
#include "utils/utils.hpp"
#include "utils/vec.hpp"
#include "scene.hpp"
#include "ray.hpp"

namespace raytracer {

// Forward declarations to avoid circular dependencies
class HitRecord;
class Sphere;


// Abstract class that represents a material that can be applied to objects in the scene.
class Material {
  public:
    // constructors and destructors
    Material() = default;
    virtual ~Material() = default;

    // returns true if the a new ray should be cast, false otherwise
    // out_colour is filled with the colour of the material at the hit point
    // out_ray is filled with the new ray to cast, if any
    virtual bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit, Colour& out_colour, Ray& out_ray) const {
      return false;
    }
};



// A material that emits light
class LightMat : public Material {
  public:
    Colour colour;    // colour of the light source
    double intensity; // intensity of the light source, used to calculate radiance

    LightMat(const Colour& _colour, double _intensity) : colour(_colour), intensity(_intensity) {}

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit, Colour& out_colour, Ray& out_ray) const override {
      out_colour = colour;
      return false;
    }

    // calculate the radiance of the light at a given distance
    Vec radiance(double t) const {
      // return colour * intensity / (t*t); // TODO
      return colour * intensity;
    }
};



// A Phong material that combines ambient, diffuse and specular lighting
class Phong : public Material {
  public:
    Phong(const Colour& _albedo, double _shininess)
      : albedo(_albedo), shininess(_shininess) {}

    Phong(const Colour& _albedo, double _shininess, double _ka, double _kd, double _ks)
      : albedo(_albedo), shininess(_shininess), ka(_ka), kd(_kd), ks(_ks) {}

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit, Colour& out_colour, Ray& out_ray) const override {
      out_colour = phong_shade(r_in, hit, scene);
      return false;
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
          Point sample = light->get_sample();
          Vec light_dir = glm::normalize(sample - hit.p);

          auto shadow_ray = Ray(hit.p, light_dir);
          if (scene.hit(shadow_ray, Interval(0.0001, infinity), shadow_hit)
              && shadow_hit.object == light) {
            // light is visible from the hit point
            auto light_mat = std::dynamic_pointer_cast<LightMat>(light->material);

            // diffuse
            Vec light_radiance = light_mat->radiance(glm::length(sample - hit.p));
            double attenuation = std::max(glm::dot(hit.normal, light_dir), 0.0);
            diff += attenuation * light_radiance;

            // specular
            Vec reflect_dir = glm::normalize(glm::reflect(-light_dir, hit.normal));
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

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit, Colour& out_colour, Ray& out_ray) const override {
      // reflection ray
      Vec reflected = glm::normalize(glm::reflect(r_in.direction(), hit.normal));
      Ray reflect_ray(hit.p, reflected);

      // evaluate the reflected ray colour
      HitRecord reflect_hit;
      Colour reflect_colour;
      if (scene.hit(reflect_ray, Interval(0.0001, infinity), reflect_hit)) {
        // hit, evaluate the material
        reflect_hit.object->material->evaluate(scene, reflect_ray, reflect_hit, reflect_colour, out_ray);
      } else {
        // miss, use scene background colour
        reflect_colour = scene.background;
      }

      // reflectance
      double cos_theta = std::min(glm::dot(-r_in.direction(), hit.normal), 1.0);
      double R = utils::reflectance(cos_theta, refract_idx);

      // final colour is a mix of the phong shading and reflected colour
      out_colour = (1-R)*phong_shade(r_in, hit, scene) + R*reflect_colour;
      return false;
    }

  private:
    double refract_idx; // refractive index in vacuum or air,
};



// A Diffuse (Lambertian) material that bounces rays in random directions
class Diffuse : public Material {
  public:
    Diffuse(const Colour& _albedo) : albedo(_albedo) {}

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit, Colour& out_colour, Ray& out_ray) const override {
      out_colour = albedo;

      // bounce the ray in a random direction
      Vec bounce_direction = hit.normal + vec::random_unit();
      if (vec::is_near_zero(bounce_direction)) // degenerate direction
        bounce_direction = hit.normal;
      out_ray = Ray(hit.p, bounce_direction);

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

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit, Colour& out_colour, Ray& out_ray) const override {
      out_colour = albedo;

      // bounce the ray in a fuzzy direction
      Vec reflected = glm::reflect(r_in.direction(), hit.normal);
      reflected = glm::normalize(reflected) + (fuzz*vec::random_unit());
      out_ray = Ray(hit.p, reflected);

      // absorb rays that bounce below the surface
      return glm::dot(out_ray.direction(), hit.normal) > 0;
    }

  private:
    Colour albedo; // colour of the material
    double fuzz;   // zero for a shiny surface, one for a completely random reflection
};


// A Dielectric (glass) material that refracts rays when possible and reflects them otherwise
class Dielectric : public Material {
  public:
    Dielectric(double _refract_idx) : refract_idx(_refract_idx) {}

    bool evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit, Colour& out_colour, Ray& out_ray) const override {
      // dieletric material absorbs nothing
      out_colour = Colour(1, 1, 1);

      double ri = hit.front_face ? (1.0/refract_idx) : refract_idx;
      double cos_theta = std::min(glm::dot(-r_in.direction(), hit.normal), 1.0);
      double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
      bool can_refract = ri * sin_theta <= 1.0;

      Vec direction;
      if (!can_refract || utils::reflectance(cos_theta, ri) > utils::random())
        direction = glm::reflect(r_in.direction(), hit.normal);
      else
        direction = glm::refract(r_in.direction(), hit.normal, ri);
      out_ray = Ray(hit.p, direction);

      return true;
    }

  private:
    // refractive index in vacuum or air,
    // or ratio of the material's refractive index over the refractive index of the enclosing medium
    double refract_idx;
};


} // namespace raytracer

#endif // MATERIAL_H
