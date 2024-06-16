#pragma once

#include "utils/common.hpp"
#include "utils/interval.hpp"
#include "utils/random.hpp"
#include "hittable/hittable_list.hpp"
#include "pdf/pdf_sample.hpp"

namespace raytracer {

// Forward declarations to avoid circular dependencies.
class LightMat;

// A hittable scene in 3D space.
class Scene : public Hittable {
  public:
    Colour ambient_light = Colour(0, 0, 0); // scene ambient light colour, used in Phong
    Colour background = Colour(0, 0, 0);    // scene background colour
    HittableList primitives;                // scene geometric instanced objects
    HittableList lights;                    // light sources
    bool sample_lights_per_area = true;     // sample light sources per area (true) or power (false)

    Scene() = default;
    Scene(Colour _ambient_light) : ambient_light(_ambient_light) {}

    void clear() {
      primitives.clear();
      lights.clear();
    }

    // void add(shared_ptr<Hittable> object) {
    //   if (std::dynamic_pointer_cast<Light>(object)) {
    void add(shared_ptr<Primitive> object) {
      if (std::dynamic_pointer_cast<LightMat>(object->material)) {
        lights.add(object);
        update_light_cdf();
      } else {
        primitives.add(object);
      }
    }

    // check if the ray intersects any object or light
    bool hit(const Ray& r, Interval ray_t, HitRecord& hit) const {
      HitRecord hit1, hit2;

      bool hit_object = primitives.hit(r, ray_t, hit1);
      bool hit_light = lights.hit(r, ray_t, hit2);

      if (hit_light && hit_object)
        hit = hit1.t < hit2.t ? hit1 : hit2;
      else if (hit_light)
        hit = hit2;
      else if (hit_object)
        hit = hit1;

      return hit_object || hit_light;;
    }

    // get the radiance of the light source at the hit point
    // Colour get_light_radiance(const HitRecord& hit) const {
    //   // return Colour(0,0,0);

    //   // sample a light from the scene
    //   double lpdf;
    //   shared_ptr<Primitive> light = sample_light(lpdf);

    //   // sample a point on the light source and a ray towards it
    //   PdfSample lsample = light->pdf_sample();
    //   Vec wi = glm::normalize(lsample.p - hit.p);
    //   Ray ray(hit.p, wi);

    //   // launch ray and try to hit the light source
    //   HitRecord hitrec;
    //   if (this->hit(ray, Interval(0.0001, infinity), hitrec) && hitrec.object == light) {
    //     double d = glm::length(lsample.p - hitrec.p);
    //     double attenuation = std::max(glm::dot(hitrec.normal(), wi), 0.0)
    //                        * std::max(glm::dot(lsample.normal, -wi), 0.0);
    //     Colour radiance = std::dynamic_pointer_cast<LightMat>(light->material)->radiance(d);
    //     return radiance * attenuation / (lpdf * lsample.pdf); // / (d * d);
    //   } else {
    //     return Colour(0, 0, 0);
    //   }
    // }


  private:
    std::vector<double> light_cdf_area;  // CDF for light sampling by area
    std::vector<double> light_cdf_power; // CDF for light sampling by power
    double total_area = 0;               // total area of all light sources
    double total_power = 0;              // total power of all light sources

    void update_light_cdf() {
      light_cdf_area.clear();
      light_cdf_power.clear();

      for (const auto& light : lights.objects) {
        total_area += light->area;
        light_cdf_area.push_back(total_area);
        // auto l = std::dynamic_pointer_cast<LightMat>(light->material);
        // total_power += light_material->intensity;
        // light_cdf_power.push_back(total_power);
      }

      // normalize the cdf
      for (auto& cdf : light_cdf_area)  cdf /= total_area;
      for (auto& cdf : light_cdf_power) cdf /= total_power;
    }

    // sample a light source from the scene using the CDF
    // the pdf is light power / total power
    // shared_ptr<Primitive> sample_light(double& pdf) const {
    //   int i;
    //   if (sample_lights_per_area) {
    //     i = random::sample_cdf(light_cdf_area);
    //     pdf = lights.objects[i]->area / total_area;
    //   } else {
    //     // i = random::sample_cdf(light_cdf_power);
    //     // auto light_material = std::dynamic_pointer_cast<LightMat>(lights.objects[i]->material);
    //     // pdf = light_material->intensity() / total_power;
    //   }
    //   return lights.objects[i].get();
    // }
};

} // namespace raytracer
