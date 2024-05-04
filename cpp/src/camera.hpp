#ifndef CAMERA_H
#define CAMERA_H

#include "utils/common.hpp"
#include "utils/utils.hpp"
#include "hittable/scene.hpp"
#include "material.hpp"

namespace raytracer {

// A camera that can render a scene.
class Camera {
  public:
    double aspect_ratio = 1.0;      // ratio width/height
    int image_width = 100;          // image width in pixel count
    int samples_per_pixel = 10;     // random samples for each pixel
    int max_depth = 10;             // maximum number of ray bounces into scene

    double vfov = 90.0;             // vertical field of view in degrees
    Point look_from = Point(0,0,0); // camera location
    Point look_at = Point(0,1,0);   // camera target
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

      // experimental parallelization: uncomment one of the pragma directives to enable

      // OpenACC for GPU parallelization
      // auto pixels = std::vector<std::vector<Colour>>(image_height, std::vector<Colour>(image_width));
      // auto image_width = this->image_width;
      // auto image_height = this->image_height;
      // auto samples_per_pixel = this->samples_per_pixel;
      // auto max_depth = this->max_depth;
      // #pragma acc data copy(pixels, image_height, image_width, samples_per_pixel, max_depth)
      // #pragma acc parallel loop

      // OpenMP for CPU parallelization
      // #pragma omp parallel for schedule(static)

      for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
          auto pixel_colour = Colour(0, 0, 0);
          for (int sample = 0; sample < samples_per_pixel; ++sample) {
            Ray r = ray_sample(i, j);
            pixel_colour += trace_ray(r, max_depth);
          }
          pixels[j][i] = pixel_colour/static_cast<double>(samples_per_pixel);
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
    Scene scene;              // scene to render

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

      // pre-allocate memory for the image
      pixels.resize(image_height);
      for (int j = 0; j < image_height; ++j)
        pixels[j].resize(image_width);
    }

    // returns the colour of the ray after it hits the scene
    Colour trace_ray(const Ray& r_in, int depth) const {
      // if we've exceeded the ray bounce limit, the ray was absorbed
      if (depth <= 0)
        return scene.ambient_light;

      // try to hit an object in the scene, starting at 0.0001 to avoid self-intersection
      // misses are considered as background colour
      HitRecord hitrec;
      if (!scene.hit(r_in, Interval(0.0001, infinity), hitrec))
        return scene.ambient_light;

      // HIT !
      // if the material fully absorbed the ray, return the material evaluated colour
      // otherwise, the material evaluation will return a new ray to trace
      Colour c;
      Ray r_out;
      if (hitrec.object->material->evaluate(r_in, hitrec, scene, c, r_out))
        return c * trace_ray(r_out, depth-1);
      return c;
    }

    // get a randomly sampled camera ray for the pixel at location i,j
    Ray ray_sample(int i, int j) const {
      // pixel position
      Point pixel_upper_left = viewport_origin + ((double)i * pixel_delta_u) + ((double)j * pixel_delta_v);
      Point pixel_pos = utils::sample_quad(pixel_upper_left, pixel_delta_u, pixel_delta_v);

      // ray center
      Point ray_origin = center;
      if (defocus_angle > 0) {
        // if defocus is enabled, the ray origin is a random point in the camera defocus disk
        Point p = utils::sample_disk(1);
        ray_origin += (p.x * defocus_u) + (p.y * defocus_v);
      }

      // ray direction
      Vec ray_direction = pixel_pos - ray_origin;
      return Ray(ray_origin, ray_direction);
    }
};

} // namespace raytracer

#endif // CAMERA_H
