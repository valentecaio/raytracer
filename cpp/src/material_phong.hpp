#pragma once

#include "utils/common.hpp"
#include "utils/utils.hpp"
#include "utils/random.hpp"
#include "pdf.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "material.hpp"

namespace raytracer {

// A Phong material that combines ambient, diffuse and specular lighting
class Phong : public Material {
  public:
    Phong(const Colour& _albedo, double _shininess)
      : albedo(_albedo), shininess(_shininess) {}

    Phong(const Colour& _albedo, double _shininess, double _ka, double _kd, double _ks)
      : albedo(_albedo), shininess(_shininess), ka(_ka), kd(_kd), ks(_ks) {}

    EvalRecord evaluate(const Scene& scene, const Ray& r_in, const HitRecord& hit) const override {
      return EvalRecord(phong_shade(r_in, hit, scene));
    }

  protected:
    Colour phong_shade(const Ray& r_in, const HitRecord& hit, const Scene& scene) const {
      Colour total_amb = scene.ambient_light;
      Colour total_diff = Colour(0);
      Colour total_spec = Colour(0);

      // view direction is the opposite of the incoming ray direction (already normalized)
      Vec view_dir = -r_in.direction();

      // try to hit lights in the scene to calculate the shading
      for (const auto& light : scene.lights.objects) {
        HitRecord shadow_hit;
        auto diff = Colour(0);
        auto spec = Colour(0);

        // point lights are sampled once, area lights are sampled multiple times
        int nsamples = (std::dynamic_pointer_cast<Sphere>(light)) ? 1 : 10;
        for (int i = 0; i < nsamples; i++) {
          Point sample = light->sample();
          Vec light_dir = glm::normalize(sample - hit.p);

          auto shadow_ray = Ray(hit.p, light_dir);
          if (scene.hit(shadow_ray, Interval(0.0001, infinity), shadow_hit)
              && shadow_hit.object == light) {
            // light is visible from the hit point
            auto lmat = std::static_pointer_cast<LightMat>(light->material);

            // diffuse
            Vec light_radiance = lmat->radiance(glm::length(sample - hit.p));
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
      return EvalRecord((1-R)*phong_shade(r_in, hit, scene) + R*reflect_colour);
    }

  private:
    double refract_idx; // refractive index in vacuum or air,
};

} // namespace raytracer
