// main.cpp

#include <chrono>

#include "hittable_list.hpp"
#include "camera.hpp"
#include "common.hpp"

using namespace raytracer;

int main() {
  /* WORLD */

  Hittable_list world;

  auto material_ground = make_shared<Lambertian>(Colour(0.8, 0.8, 0.0));
  auto material_center = make_shared<Lambertian>(Colour(0.1, 0.2, 0.5));
  auto material_left   = make_shared<Metal>(Colour(0.4, 0.4, 0.4), 0.0);
  auto material_right  = make_shared<Metal>(Colour(0.2, 0.8, 0.2), 0.2);
  auto material_small  = make_shared<Dielectric>(1.5);
  auto material_bubble = make_shared<Dielectric>(1.0/1.5);

  world.add(make_shared<Sphere>(Point( 0.0, -100.5, -2.0), 100.0, material_ground));
  world.add(make_shared<Sphere>(Point( 0.0,    0.0, -2.2), 0.5, material_center));
  world.add(make_shared<Sphere>(Point(-1.0,    0.0, -2.0), 0.5, material_left));
  world.add(make_shared<Sphere>(Point( 2.0,    0.0, -2.0), 0.5, material_right));
  world.add(make_shared<Sphere>(Point( 0.3,   -0.1, -0.7), 0.2, material_small));
  // world.add(make_shared<Sphere>(Point( 0.3,  -0.15, -0.7), 0.15, material_bubble));

  /* CAMERA */

  Camera camera(world);
  camera.aspect_ratio = 16.0 / 9.0;
  camera.image_width = 800;
  camera.samples_per_pixel = 15;
  camera.max_depth = 15;

  camera.vfov = 90.0;
  camera.look_from = Point(0,0,0);
  camera.look_at = Point(0,0,-1);
  camera.vup = Vec(0,1,0);

  camera.defocus_angle = 0;
  camera.focus_dist = 1;

  /* RENDER */

  auto start = std::chrono::high_resolution_clock::now();
  camera.render();
  auto end = std::chrono::high_resolution_clock::now();
  std::clog << "\nRender time: " << std::chrono::duration<double>(end-start).count() << " seconds" << std::endl;
}
