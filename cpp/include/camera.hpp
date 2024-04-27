#ifndef CAMERA_H
#define CAMERA_H

#include "common.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "image.hpp"
#include "utils.hpp"
#include "material.hpp"

namespace raytracer {

class Camera {
  public:
    double aspect_ratio = 1.0;      // ratio width/height
    int image_width = 100;          // image width in pixel count
    int samples_per_pixel = 10;     // random samples for each pixel
    int max_depth = 10;             // maximum number of ray bounces into scene

    double vfov = 90.0;             // vertical field of view in degrees
    Point look_from = Point(0,0,0); // camera location
    Point look_at = Point(0,1,0);   // camera target
    Vec vup;                        // camera up vector (view up)

    double defocus_angle = 0.0;     // angle of the cone with apex at the viewpoint and base at the camera center (0 = no defocus)
    double focus_dist = 10;         // distance from camera to focus plane

    Colour background_colour;       // scene background colour


    // constructors and destructors
    Camera() = default;
    Camera(Hittable_list world) : world(world) {}

    // render the image row by row, from top to bottom
    void render() {
      initialize();

      // necessary for data sharing in OpenACC
      auto pixels = std::vector<std::vector<Colour>>(image_height, std::vector<Colour>(image_width));
      auto image_width = this->image_width;
      auto image_height = this->image_height;
      auto samples_per_pixel = this->samples_per_pixel;
      auto max_depth = this->max_depth;

      // #pragma acc data copy(pixels, image_height, image_width, samples_per_pixel, max_depth)
      // #pragma acc parallel loop
      // #pragma omp parallel for schedule(static)
      for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
          auto pixel_colour = Colour(0, 0, 0);
          for (int sample = 0; sample < samples_per_pixel; ++sample) {
            Ray r = get_ray(i, j);
            pixel_colour += ray_colour(r, max_depth);
          }
          pixels[j][i] = pixel_colour/static_cast<double>(samples_per_pixel);
        }
      }
      write_image(image_width, image_height, pixels);
    }


  private:
    int image_height;         // image height in pixel count
    Point center;             // camera center
    Point pixel00_loc;        // location of pixel {0, 0}
    Vec pixel_delta_u;        // offset to pixel to the right
    Vec pixel_delta_v;        // offset to pixel below
    Vec u, v, w;              // camera coordinate system
    Vec defocus_u, defocus_v; // defocus vectors, u is horizontal, v is vertical
    bool initialized = false; // flag to check if the camera has been initialized
    Hittable_list world;      // world to render

    std::vector<std::vector<Colour>> pixels; // image pixel data

    void initialize() {
      if (initialized) return;

      // the image has a locked aspect ratio, but the height has to be at least 1
      image_height = max(static_cast<int>(image_width / aspect_ratio), 1);

      /* CAMERA */

      // the camera is at the origin of the world, looking towards the negative z-axis.
      // we will use right-handed coordinates, so the x-axis points to the right,
      // the y-axis points up, and the z-axis points towards the viewer
      center = look_from;

      // the viewport is a virtual window that we use to render the image
      // it is a grid of pixels, with the same aspect ratio as the image
      auto theta = glm::radians(vfov);
      auto h = glm::tan(theta/2.0);
      auto viewport_height = 2.0 * h * focus_dist;
      auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

      // the camera coordinate system is defined by the look_from, look_at, and vup vectors
      w = glm::normalize(look_from - look_at); // camera forward direction
      u = glm::normalize(glm::cross(vup, w));  // camera right direction
      v = glm::cross(w, u);                    // camera up direction

      // the vectors vu and vv define the viewport in the world coordinates
      // the viewport is centered at the camera, and the camera is looking towards the negative z-axis
      auto viewport_u = u * viewport_width;
      auto viewport_v = -v * viewport_height;

      // these are the deltas for the pixel coordinates in the viewport
      pixel_delta_u = viewport_u / static_cast<double>(image_width);
      pixel_delta_v = viewport_v / static_cast<double>(image_height);

      // in the image coordinates, the Y axis is flipped and the origin is at the top left corner
      // the top left corner of the image has coordinates (0, 0), and the bottom right
      // corner has coordinates (image_width, image_height).
      // we must calculate the location of the upper left pixel in the viewport coordinates
      auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2.0 - viewport_v/2.0;
      pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

      // camera defocus disk factors
      auto defocus_radius = focus_dist * glm::tan(glm::radians(defocus_angle)/2.0);
      defocus_u = u * defocus_radius;
      defocus_v = v * defocus_radius;

      // pre-allocate memory for the image
      pixels.resize(image_height);
      for (int j = 0; j < image_height; ++j)
        pixels[j].resize(image_width);
    }

    // returns the colour of the ray after it hits the world
    Colour ray_colour(const Ray& r, int depth) const {
      // if we've exceeded the ray bounce limit, no more light is gathered
      if (depth <= 0)
        return background_colour;

      Hit_record rec;
      // starts interval at 0.0001 to avoid self-intersection
      if (world.hit(r, Interval(0.0001, infinity), rec)) {
        Ray scattered;
        Colour attenuation;
        if (rec.material->scatter(r, rec, attenuation, scattered)) {
          return attenuation * ray_colour(scattered, depth-1);
        } else {
          return rec.material->emit();
        }
      } else {
        return background_colour;
      }
    }

    // get a randomly sampled camera ray for the pixel at location i,j
    Ray get_ray(int i, int j) const {
      auto pixel_center = pixel00_loc
                        + (static_cast<double>(i) * pixel_delta_u)
                        + (static_cast<double>(j) * pixel_delta_v);
      auto pixel_sample = pixel_center + pixel_sample_square();

      auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
      auto ray_direction = pixel_sample - ray_origin;

      return Ray(ray_origin, ray_direction);
    }

    // returns a random point in the square surrounding a pixel at the origin
    Vec pixel_sample_square() const {
      auto px = -0.5 + random_double();
      auto py = -0.5 + random_double();
      return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    // returns a random point in the camera defocus disk.
    Point defocus_disk_sample() const {
      auto p = vec::random_in_unit_disk();
      return center + (p.x * defocus_u) + (p.y * defocus_v);
    }

};

} // namespace raytracer

#endif // CAMERA_H
