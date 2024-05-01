// main.cpp

#include <chrono>

#include "common.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "sphere.hpp"
#include "quad.hpp"

using namespace raytracer;

void cornell_box() {
  Scene scene;

  scene.ambient_light_colour = Colour(0.05, 0.05, 0.05);
  auto light = make_shared<Light>(Colour(1, 1, 1), 10);
  scene.add_light(make_shared<Quad>(Point(343, 554, 332), Vec(-130,0,0), Vec(0,0,-105), light));

  auto red   = make_shared<Lambertian>(Colour(.65, .05, .05));
  auto white = make_shared<Lambertian>(Colour(.73, .73, .73));
  auto green = make_shared<Lambertian>(Colour(.12, .45, .15));
  scene.add_object(make_shared<Quad>(Point(555,0,0), Vec(0,555,0), Vec(0,0,555), green));
  scene.add_object(make_shared<Quad>(Point(0,0,0), Vec(0,555,0), Vec(0,0,555), red));
  scene.add_object(make_shared<Quad>(Point(0,0,0), Vec(555,0,0), Vec(0,0,555), white));
  scene.add_object(make_shared<Quad>(Point(555,555,555), Vec(-555,0,0), Vec(0,0,-555), white));
  scene.add_object(make_shared<Quad>(Point(0,0,555), Vec(555,0,0), Vec(0,555,0), white));


  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 1.0;
  camera.image_width = 600;
  camera.samples_per_pixel = 20;
  camera.max_depth = 15;
  camera.vfov = 40;
  camera.look_from = Point(278, 278, -800);
  camera.look_at = Point(278, 278, 0);
  auto start = std::chrono::high_resolution_clock::now();
  camera.render();
  auto end = std::chrono::high_resolution_clock::now();
  std::clog << "\nRender time: " << std::chrono::duration<double>(end-start).count() << " seconds" << std::endl;
}


void quads() {
  Scene scene;

  scene.ambient_light_colour = Colour(1, 1, 1);

  auto left_red     = make_shared<Lambertian>(Colour(1.0, 0.2, 0.2));
  auto back_green   = make_shared<Lambertian>(Colour(0.2, 1.0, 0.2));
  auto right_blue   = make_shared<Metal>(Colour(0.4, 0.4, 1.0), 0.03);
  auto upper_orange = make_shared<Lambertian>(Colour(1.0, 0.5, 0.0));
  auto lower_teal   = make_shared<Lambertian>(Colour(0.2, 0.8, 0.8));
  scene.add_object(make_shared<Quad>(Point(-3,-2, 5), Vec(0, 0,-4), Vec(0, 4, 0), left_red));
  scene.add_object(make_shared<Quad>(Point(-2,-2, 0), Vec(4, 0, 0), Vec(0, 4, 0), back_green));
  scene.add_object(make_shared<Quad>(Point( 3,-2, 1), Vec(0, 0, 4), Vec(0, 4, 0), right_blue));
  scene.add_object(make_shared<Quad>(Point(-2, 3, 1), Vec(4, 0, 0), Vec(0, 0, 4), upper_orange));
  scene.add_object(make_shared<Quad>(Point(-2,-3, 5), Vec(4, 0, 0), Vec(0, 0,-4), lower_teal));

  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 1.0;
  camera.image_width = 600;
  camera.samples_per_pixel = 30;
  camera.max_depth = 15;
  camera.vfov = 80;
  camera.look_from = Point(0,0,9);
  camera.look_at = Point(0,0,0);
  auto start = std::chrono::high_resolution_clock::now();
  camera.render();
  auto end = std::chrono::high_resolution_clock::now();
  std::clog << "\nRender time: " << std::chrono::duration<double>(end-start).count() << " seconds" << std::endl;

}


void spheres() {
  Scene scene;

  scene.ambient_light_colour = Colour(0.05, 0.05, 0.05);
  auto material_light  = make_shared<Light>(Colour(1, 1, 0), 10);
  scene.add_light(make_shared<Sphere>(Point( 2.0,    0.0, -2.0), 0.5, material_light));

  auto material_ground = make_shared<Lambertian>(Colour(0.8, 0.8, 0.0));
  auto material_center = make_shared<Lambertian>(Colour(0.1, 0.2, 0.5));
  auto material_metal1 = make_shared<Metal>(Colour(0.4, 0.4, 0.4), 0.0);
  auto material_metal2 = make_shared<Metal>(Colour(0.2, 0.8, 0.2), 0.2);
  auto material_glass  = make_shared<Dielectric>(1.5);
  auto material_bubble = make_shared<Dielectric>(1.0/1.5);
  scene.add_object(make_shared<Sphere>(Point( 0.0, -100.5, -2.0), 100.0, material_ground));
  scene.add_object(make_shared<Sphere>(Point( 0.0,    0.0, -2.2), 0.5, material_center));
  scene.add_object(make_shared<Sphere>(Point(-1.0,    0.0, -2.0), 0.5, material_metal1));
  scene.add_object(make_shared<Sphere>(Point( 0.3,   -0.1, -0.7), 0.2, material_glass));
  // scene.add(make_shared<Sphere>(Point( 0.3,  -0.15, -0.7), 0.15, material_bubble));


  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 16.0 / 9.0;
  camera.image_width = 800;
  camera.samples_per_pixel = 20;
  camera.max_depth = 15;
  camera.vfov = 90.0;
  camera.look_from = Point(0,0,0);
  camera.look_at = Point(0,0,-1);
  camera.defocus_angle = 0;
  camera.focus_dist = 1;

  auto start = std::chrono::high_resolution_clock::now();
  camera.render();
  auto end = std::chrono::high_resolution_clock::now();
  std::clog << "\nRender time: " << std::chrono::duration<double>(end-start).count() << " seconds" << std::endl;
}


void phong() {
  Scene scene;

  scene.ambient_light_colour = Colour(0.03, 0.03, 0.03);
  auto material_light = make_shared<Light>(Colour(1, 1, 1), 0.4);
  scene.add_light (make_shared<Sphere>(Point(2.0, 1.0, -2.0), 0.1, material_light));

  auto material_ground = make_shared<Phong>(Colour(0.8, 0.0, 0.8), 100);
  auto material_center = make_shared<Phong>(Colour(0.8, 0.8, 0.0), 3);
  auto material_metal1 = make_shared<Phong>(Colour(0.0, 0.8, 0.8), 10);
  scene.add_object(make_shared<Sphere>(Point( 0.0, -100.5, -2.0), 100.0, material_ground));
  scene.add_object(make_shared<Sphere>(Point( 0.0,    0.0, -2.2), 0.5, material_center));
  scene.add_object(make_shared<Sphere>(Point(-1.0,    0.0, -2.0), 0.5, material_metal1));


  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 16.0 / 9.0;
  camera.image_width = 800;
  camera.samples_per_pixel = 20;
  camera.max_depth = 15;
  camera.vfov = 90.0;
  camera.look_from = Point(0,0,0);
  camera.look_at = Point(0,0,-1);
  camera.defocus_angle = 0;
  camera.focus_dist = 1;

  auto start = std::chrono::high_resolution_clock::now();
  camera.render();
  auto end = std::chrono::high_resolution_clock::now();
  std::clog << "\nRender time: " << std::chrono::duration<double>(end-start).count() << " seconds" << std::endl;
}

int main() {
  switch (3) {
    case 0: cornell_box(); break;
    case 1: spheres(); break;
    case 2: quads(); break;
    case 3: phong(); break;
  }
}
