#pragma once

#include "utils/common.hpp"
#include "utils/interval.hpp"
#include "utils/random.hpp"
#include "hittable/hittable_list.hpp"
#include "pdf.hpp"
#include "material.hpp"

namespace raytracer {

// A hittable scene in 3D space.
class Scene : public Hittable {
  public:
    Colour ambient_light = Colour(0); // scene ambient light colour
    Colour background = Colour(0);    // scene background colour - only used by Phong materials
    HittableList primitives;          // scene geometric instanced objects
    HittableList lights;              // light sources

    Scene() = default;
    Scene(Colour _ambient_light) : ambient_light(_ambient_light) {}

    void clear() {
      primitives.clear();
      lights.clear();
    }

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

    // sample a light source from the scene using the pre-calculated CDF
    shared_ptr<Primitive> sample_light() const {
      return lights.objects[random::sample_cdf(light_cdf)];
    }

    // get the radiance of the light source at the hit point
    Colour get_light_radiance(const HitRecord& hit) const {
      // sample a light from the scene
      shared_ptr<Primitive> light = sample_light();
      auto lmat = std::static_pointer_cast<LightMat>(light->material);
      double pdf = lmat->intensity / total_power;

      // sample a point on the light source
      // spheres are point lights, other primitives are area lights
      Sample sample;
      Vec wi;
      auto point_light = std::dynamic_pointer_cast<Sphere>(light);
      if (point_light) {
        sample.p = point_light->center;
        wi = glm::normalize(sample.p - hit.p);
        sample.normal = -wi;
      } else {
        sample = light->pdf_sample();
        wi = glm::normalize(sample.p - hit.p);
      }
      auto ray = Ray(hit.p, wi);
      // pdf *= light->pdf_value(ray); // TODO: not working, gets too dark

      // TODO: MIS not working
      // auto surface_pdf = make_shared<CosinePdf>(hit.normal());
      // ray = random::rand() < 0.5 ? ray : Ray(hit.p, surface_pdf->generate());
      // pdf = 0.5 * pdf + 0.5 * surface_pdf->value(ray.direction());

      // launch ray and try to hit the light source
      HitRecord hitrec;
      if (this->hit(ray, Interval(0.0001, infinity), hitrec) && hitrec.object == light) {
        double distance = glm::length(sample.p - hitrec.p);
        double cos1 = std::max(glm::dot(hit.normal(), wi), 0.0);
        double cos2 = std::max(glm::dot(-wi, sample.normal), 0.0);
        Colour radiance = lmat->radiance(distance);
        return radiance * cos1 * cos2 / pdf;
      } else {
        return Colour(0);
      }
    }


  private:
    std::vector<double> light_cdf; // CDF for light sampling by power
    double total_power = 0;        // total power of all light sources

    // update the CDFs for light sampling
    void update_light_cdf() {
      // clear the old CDFs
      light_cdf.clear();
      total_power = 0;

      for (const auto& light : lights.objects) {
        auto lmat = std::static_pointer_cast<LightMat>(light->material);
        total_power += lmat->intensity;
        light_cdf.push_back(total_power);
      }

      // normalize the CDF
      for (auto& cdf : light_cdf)
        cdf /= total_power;

      // print the new CDF
      std::clog << "Updated lights CDF (by power): {";
      for (auto cdf : light_cdf) std::clog << cdf << ", ";
      std::clog << "}" << std::endl;
    }
};

} // namespace raytracer
