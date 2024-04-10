#ifndef CAMERA_H
#define CAMERA_H

#include "common.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "image.hpp"
#include "utils.hpp"

namespace raytracer {

class Camera {
  public:
    double aspect_ratio = 1.0;  // ratio width/height
    int image_width = 100;      // image width in pixel count
    int samples_per_pixel = 10; // random samples for each pixel
    int max_depth = 10;         // maximum number of ray bounces into scene

    // render the image row by row, from top to bottom
    void render(const Hittable& world) {
      initialize();

      for (int j = 0; j < image_height; ++j) {
        pixels.push_back(std::vector<Colour>());
        for (int i = 0; i < image_width; ++i) {
          auto pixel_color = Colour(0, 0, 0);
          for (int sample = 0; sample < samples_per_pixel; ++sample) {
            Ray r = get_ray(i, j);
            pixel_color += ray_colour(r, world, max_depth);
          }
          pixels[j].push_back(pixel_color/static_cast<double>(samples_per_pixel));
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
    bool initialized = false; // flag to check if the camera has been initialized
    std::vector<std::vector<Colour>> pixels; // image pixel data

    void initialize() {
      if (initialized) return;

      /* IMAGE */

      // the image has a locked aspect ratio, but the height has to be at least 1
      image_height = max(static_cast<int>(image_width / aspect_ratio), 1);

      /* WORLD */

      Hittable_list world;
      world.add(make_shared<Sphere>(Point(0,0,-1), 0.5));      // a sphere in the center
      world.add(make_shared<Sphere>(Point(0,-100.5,-1), 100)); // ground sphere

      /* CAMERA */

      // the camera is at the origin of the world, looking towards the negative z-axis.
      // we will use right-handed coordinates, so the x-axis points to the right,
      // the y-axis points up, and the z-axis points towards the viewer
      center = Point(0, 0, 0);

      // the focal length is the distance between the camera and the viewport.
      // we will start with a focal length of 1.0
      auto focal_length = 1.0;

      // the viewport is a virtual window that we use to render the image
      // it is a grid of pixels, with the same aspect ratio as the image
      // we need to calculate the actual aspect ratio in double precision here
      auto viewport_height = 2.0;
      auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

      // the vectors vu and vv define the viewport in the world coordinates
      // the viewport is centered at the camera, and the camera is looking towards the negative z-axis
      auto viewport_u = Vec(viewport_width, 0, 0);
      auto viewport_v = Vec(0, -viewport_height, 0);

      // these are the deltas for the pixel coordinates in the viewport
      pixel_delta_u = viewport_u / static_cast<double>(image_width);
      pixel_delta_v = viewport_v / static_cast<double>(image_height);

      // in the image coordinates, the Y axis is flipped and the origin is at the top left corner
      // the top left corner of the image has coordinates (0, 0), and the bottom right
      // corner has coordinates (image_width, image_height).
      // we must calculate the location of the upper left pixel in the viewport coordinates
      auto viewport_upper_left = center - Vec(0, 0, focal_length) - viewport_u/2.0 - viewport_v/2.0;
      pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    // returns the colour of the ray after it hits the world
    Colour ray_colour(const Ray& r, const Hittable& world, int depth) const {
      // if we've exceeded the ray bounce limit, no more light is gathered
      if (depth <= 0)
        return Colour(1, 0, 0);

      Hit_record rec;
      // simple diffuse material:
      // if a ray hits an object, it bounces off in a random direction
      // starts interval at 0.0001 to avoid self-intersection
      if (world.hit(r, Interval(0.0001, infinity), rec)) {
        Vec rand_direction = random_vec_on_hemisphere(rec.normal);
        return 0.5 * ray_colour(Ray(rec.p, rand_direction), world, depth-1);
      }

      Vec unit_direction = glm::normalize(r.direction());
      // normal is in the range [-1, 1] so we need to map it to [0, 1]
      auto a = 0.5*(unit_direction.y + 1.0);
      return (1-a)*Colour(1, 1, 1) + a*Colour(0.5, 0.7, 1); // linear interpolation
    }

    // get a randomly sampled camera ray for the pixel at location i,j
    Ray get_ray(int i, int j) const {
      auto pixel_center = pixel00_loc
                        + (static_cast<double>(i) * pixel_delta_u)
                        + (static_cast<double>(j) * pixel_delta_v);
      auto pixel_sample = pixel_center + pixel_sample_square();

      auto ray_origin = center;
      auto ray_direction = pixel_sample - ray_origin;

      return Ray(ray_origin, ray_direction);
    }

    // returns a random point in the square surrounding a pixel at the origin
    Vec pixel_sample_square() const {
      auto px = -0.5 + random_double();
      auto py = -0.5 + random_double();
      return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

};

} // namespace raytracer

#endif // CAMERA_H
