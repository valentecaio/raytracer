#pragma once

#include "utils/common.hpp"
#include "utils/utils.hpp"
#include "utils/random.hpp"
#include "hittable/hit_record.hpp"
#include "scene.hpp"
#include "material.hpp"

namespace raytracer {

// A camera that can render a scene.
class Camera {
  public:
    double aspect_ratio = 1.0;      // ratio width/height
    int image_width = 100;          // image width in pixel count
    int samples_per_pixel = 9;      // number of random samples for each pixel, must be a square number for stratified sampling (1, 4, 9, 16, ...)
    int max_depth = 10;             // maximum number of ray bounces into scene
    int min_depth = 4;              // minimum number of ray bounces into scene, for russian roulette
    bool enable_MIS = false;        // enable Multiple Importance Sampling (MIS) for light and surface sampling
    bool russian_roulette = true;   // enable russian roulette for path termination

    double vfov = 90.0;             // vertical field of view in degrees
    Point look_from = Point(0,0,0); // camera location
    Point look_at = Point(0,0,-1);  // camera target
    Vec vup = Vec(0, 1, 0);         // camera up vector (view up)

    double defocus_angle = 0.0;     // angle of the cone with apex at the viewpoint and base at the camera center (0 = no defocus)
    double focus_dist = 1;          // distance from camera to focus plane


    // constructors and destructors
    Camera() = default;
    Camera(Scene scene) : scene(scene) {}
    ~Camera() = default;

    // render the image row by row, from top to bottom
    void render() {
      initialize();
      double pixels_sample_scale = 1.0 / samples_per_pixel;

    #ifdef OPENMP
      // CPU parallelization
      #pragma omp parallel for schedule(static)
    #endif

      for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
          auto pixel_colour = Colour(0, 0, 0);
          for (int sample_idx = 0; sample_idx < samples_per_pixel; ++sample_idx) {
            Ray r = ray_sample(i, j, sample_idx);
            // pixel_colour += ray_trace(r);
            pixel_colour += path_trace(r);
          }
          pixels[j][i] = pixel_colour * pixels_sample_scale;
        }
      }
      utils::write_image(image_width, image_height, pixels);
    }


  private:
    int image_height;         // image height in pixel count
    Point center;             // camera center
    Point viewport_origin;    // upper left corner of the viewport
    Vec pixel_delta_u;        // offset to pixel to the right
    Vec pixel_delta_v;        // offset to pixel below
    Vec u, v, w;              // camera coordinate system
    Vec defocus_u, defocus_v; // defocus vectors, u is horizontal, v is vertical
    bool initialized = false; // flag to check if the camera has been initialized
    int sqrt_spp;             // square root of samples_per_pixel
    const Scene scene;        // scene to render

    std::vector<std::vector<Colour>> pixels; // image pixel data

    void initialize() {
      if (initialized) return;

      // the image has a locked aspect ratio, but the height has to be at least 1
      image_height = std::max(static_cast<int>(image_width / aspect_ratio), 1);

      /* CAMERA */

      // the camera is at the origin of the scene, looking towards the negative z-axis.
      // we will use right-handed coordinates, so the x-axis points to the right,
      // the y-axis points up, and the z-axis points towards the viewer
      center = look_from;

      // the viewport is a virtual window that we use to render the image
      // it is a grid of pixels, with the same aspect ratio as the image
      double theta = glm::radians(vfov);
      double h = glm::tan(theta/2.0);
      double viewport_height = 2.0 * h * focus_dist;
      double viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

      // the camera coordinate system is defined by the look_from, look_at, and vup vectors
      w = glm::normalize(look_from - look_at); // camera forward direction
      u = glm::normalize(glm::cross(vup, w));  // camera right direction
      v = glm::cross(w, u);                    // camera up direction

      // the vectors vu and vv define the viewport in the scene coordinates
      // the viewport is centered at the camera, and the camera is looking towards the negative z-axis
      Vec viewport_u = u * viewport_width;
      Vec viewport_v = -v * viewport_height;

      // these are the deltas for the pixel coordinates in the viewport
      pixel_delta_u = viewport_u / static_cast<double>(image_width);
      pixel_delta_v = viewport_v / static_cast<double>(image_height);

      // in the image coordinates, the Y axis is flipped and the origin is at the top left corner
      // the top left corner of the image has coordinates (0, 0), and the bottom right
      // corner has coordinates (image_width, image_height).
      // we must calculate the location of the upper left pixel in the viewport coordinates
      viewport_origin = center - (focus_dist * w) - viewport_u/2.0 - viewport_v/2.0;

      // camera defocus disk factors
      double defocus_radius = focus_dist * glm::tan(glm::radians(defocus_angle)/2.0);
      defocus_u = u * defocus_radius;
      defocus_v = v * defocus_radius;

      // store square root of samples_per_pixel for stratified sampling
      sqrt_spp = static_cast<int>(std::sqrt(samples_per_pixel));

      // pre-allocate memory for the image
      pixels.resize(image_height);
      for (int j = 0; j < image_height; ++j)
        pixels[j].resize(image_width);
    }

    // Simple ray tracing algorithm with basic path tracing (recursive).
    Colour ray_trace(const Ray& r_in, int depth = 0) const {
      // if we've exceeded the ray bounce limit, the ray was absorbed
      if (depth >= max_depth)
        return Colour(0);

      // try to hit an object in the scene, starting at 0.0001 to avoid self-intersection
      // misses are considered as ambient_light colour
      HitRecord hit;
      if (!scene.hit(r_in, Interval(0.0001, infinity), hit))
        return scene.ambient_light;

      // HIT //

      EvalRecord eval = hit.object->material->evaluate(scene, r_in, hit);

      // ray was bounced and has a fixed direction (simple reflection)
      if (eval.ray != nullptr)
        return eval.colour * ray_trace(*eval.ray, depth+1);

      // ray was bounced and has a pdf (Diffuse)
      else if (eval.pdf != nullptr)
        return eval.colour * ray_trace(Ray(hit.p, eval.pdf->generate()), depth+1);

      // ray was absorbed (Light and Phong materials)
      else
        return eval.colour;
    }

    // Iterative path tracing algorithm.
    Colour path_trace(Ray& ray) const {
      auto L    = Colour(0); // accumulated radiance
      auto beta = Colour(1); // ponderation factor for the path

      for (int depth = 0; depth < max_depth; ++depth) {
        // russian roulette (https://www.pbr-book.org/3ed-2018/Monte_Carlo_Integration/Russian_Roulette_and_Splitting)
        if (russian_roulette && depth > min_depth) {
          // continuation probability (at least 10%)
          double p = utils::max({beta.r, beta.g, beta.b, 0.1});
          if (random::rand() > p) {
            // std::clog << "---" << std::endl;
            // std::clog << "Russian roulette end at {depth, p} = {" << depth << ", " << p << "}" << std::endl;
            // vec::print(beta);
            return L;
          }
          // compensate beta so that paths with higher prob of continuation are not overestimated
          beta /= p;
        }

        // try to hit an object in the scene, starting at 0.0001 to avoid self-intersection
        // misses are considered as ambient_light colour
        HitRecord hit;
        if (!scene.hit(ray, Interval(0.0001, infinity), hit)) {
          return L + beta * scene.ambient_light;
        }

        // HIT //

        // evaluate the material at the hit point
        auto mat = hit.object->material;
        EvalRecord eval = mat->evaluate(scene, ray, hit);

        // light source
        if (std::dynamic_pointer_cast<LightMat>(hit.object->material)) {
          // TODO: we should only count direct light sources at the first hit.
          // if we do so, the noise decreases faster, but we lose the reflection
          // of light on the scene objects, which looks much less realistic.
          // return (depth == 0) ? eval.colour : L;
          // return eval.colour * beta;
          return eval.colour; // more noise, but more realistic
        }

        // ray was absorbed (Phong materials)
        if (eval.pdf == nullptr && eval.ray == nullptr) {
          return L * eval.colour;
        }

        // path trace material

        // end path shooting a last ray to a light source
        Colour Le = scene.get_light_radiance(hit);
        L += Le * beta * eval.colour * mat->brdf_factor();

        // prepare next path segment
        ray = Ray(hit.p, eval.pdf->generate());
        double pdf = eval.pdf->value(ray.direction());
        double scatter_pdf = mat->scatter_pdf(hit.normal(), ray);
        beta *= eval.colour * scatter_pdf / pdf;
      }
      return L;
    }


    // get a stratified sampled camera ray for the pixel at location i,j
    // sample_idx is the index of the sample in the pixel, used to stratify the samples
    Ray ray_sample(int i, int j, int sample_idx) const {
      // pixel position
      Point pixel_upper_left = viewport_origin + ((double)i * pixel_delta_u) + ((double)j * pixel_delta_v);
      Point pixel_pos = random::sample_quad(pixel_upper_left, pixel_delta_u, pixel_delta_v);

      // TODO: stratified sampling not working properly
      // Point pixel_pos = random::sample_quad_stratified(pixel_upper_left, pixel_delta_u, pixel_delta_v, sample_idx, sqrt_spp);

      // ray center
      Point ray_origin = center;
      if (defocus_angle > 0) {
        // if defocus is enabled, the ray origin is a random point in the camera defocus disk
        Point p = random::sample_disk(1);
        ray_origin += (p.x * defocus_u) + (p.y * defocus_v);
      }

      // ray direction
      Vec ray_direction = pixel_pos - ray_origin;
      return Ray(ray_origin, ray_direction);
    }
};

} // namespace raytracer
