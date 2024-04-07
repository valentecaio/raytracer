// main.cpp

#include "common.hpp"
#include "hittable_list.hpp"
#include "camera.hpp"

using namespace raytracer;

int main() {
  /* WORLD */

  Hittable_list world;
  // a sphere in the center
  world.add(make_shared<Sphere>(Point(0, 0, -1), 0.5));
  // ground sphere
  world.add(make_shared<Sphere>(Point(0, -100.5, -1), 100));


  /* CAMERA */

  Camera camera = Camera();
  camera.aspect_ratio = 16.0 / 9.0;
  camera.image_width = 800;
  camera.samples_per_pixel = 10;
  camera.render(world);
}
