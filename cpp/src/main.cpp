// main.cpp

#include "hittable_list.hpp"
#include "camera.hpp"
#include "common.hpp"

using namespace raytracer;

int main() {
  /* WORLD */

  Hittable_list world;

  auto material_ground = make_shared<Lambertian>(Colour(0.8, 0.8, 0.0));
  auto material_center = make_shared<Lambertian>(Colour(0.1, 0.2, 0.5));
  auto material_left =  make_shared<Metal>(Colour(1.0, 0.2, 0.2), 0.0);
  auto material_right = make_shared<Metal>(Colour(0.2, 0.8, 0.2), 0.2);

  world.add(make_shared<Sphere>(Point( 0.0, -100.5, -2.0), 100.0, material_ground));
  world.add(make_shared<Sphere>(Point( 0.0,    0.0, -2.2), 0.5, material_center));
  world.add(make_shared<Sphere>(Point(-1.0,    0.0, -2.0), 0.5, material_left));
  world.add(make_shared<Sphere>(Point( 2.0,    0.0, -2.0), 0.5, material_right));
  world.add(make_shared<Sphere>(Point( 0.3,  -0.15, -0.7), 0.1, material_left));

  /* CAMERA */

  Camera camera;
  camera.aspect_ratio = 16.0 / 9.0;
  camera.image_width = 800;
  camera.samples_per_pixel = 5;
  camera.max_depth = 5;
  camera.render(world);
}
