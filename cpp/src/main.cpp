// main.cpp

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "defines.hpp"
#include "sphere.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "vec.hpp"

using namespace raytracer;

colour3 ray_color(const Ray& r) {
  auto sphere = Sphere(point3(0,0,-1), 0.5);
  Hit_record rec;
  if (sphere.hit(r, 0.0, 5, rec)) {
    // normal is in the range [-1, 1] so we need to map it to [0, 1]
    return 0.5*colour3(rec.normal.x+1, rec.normal.y+1, rec.normal.z+1);
  }

  vec3 unit_direction = glm::normalize(r.direction());
  auto a = 0.5*(unit_direction.y + 1.0);
  return (1.0 - a)*colour3(1, 1, 1) + a*colour3(0.5, 0.7, 1); // linear interpolation
}

int main() {
  // the image has a locked aspect ratio of 16:9, the height is calculated based on the width
  // it has to be at least 1 because we can't have an image with 0 height
  int image_width = 800;
  auto aspect_ratio = 16.0 / 9.0;
  int image_height = MAX(static_cast<int>(image_width / aspect_ratio), 1);

  // the viewport is a virtual window that we use to render the image
  // it is a grid of pixels, with the same aspect ratio as the image
  // we need to calculate the actual aspect ratio in double precision here
  auto viewport_height = 2.0;
  auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

  // the focal length is the distance between the camera and the viewport.
  // we will start with a focal length of 1.0
  auto focal_length = 1.0;

  // the camera is at the origin of the world, looking towards the negative z-axis.
  // we will use right-handed coordinates, so the x-axis points to the right,
  // the y-axis points up, and the z-axis points towards the viewer
  auto camera_center = point3(0, 0, 0);

  // the vectors vu and vv define the viewport in the world coordinates
  // the viewport is centered at the camera, and the camera is looking towards the negative z-axis
  auto viewport_u = vec3(viewport_width, 0, 0);
  auto viewport_v = vec3(0, -viewport_height, 0);

  // these are the deltas for the pixel coordinates in the viewport
  auto pixel_delta_u = viewport_u / static_cast<double>(image_width);
  auto pixel_delta_v = viewport_v / static_cast<double>(image_height);

  // in the image coordinates, the Y axis is flipped and the origin is at the top left corner
  // the top left corner of the image has coordinates (0, 0), and the bottom right
  // corner has coordinates (image_width, image_height).
  // we must calculate the location of the upper left pixel in the viewport coordinates
  auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2.0 - viewport_v/2.0;
  auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
 
  // render the image row by row, from top to bottom
  std::vector<std::vector<colour3>> pixels;
  for (int j = 0; j < image_height; ++j) {
    pixels.push_back(std::vector<colour3>());
    for (int i = 0; i < image_width; ++i) {
      auto pixel_center = pixel00_loc + (static_cast<double>(i) * pixel_delta_u) + (static_cast<double>(j) * pixel_delta_v);
      auto ray_direction = pixel_center - camera_center;
      Ray r(camera_center, ray_direction);
      pixels[j].push_back(ray_color(r));
    }
  }
  write_image(image_width, image_height, pixels);

  return 0;
}
