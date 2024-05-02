// main.cpp

#include "utils/common.hpp"
#include "hittable/scene.hpp"
#include "primitives/2d.hpp"
#include "primitives/sphere.hpp"
#include "primitives/box.hpp"
#include "camera.hpp"
#include "material.hpp"

using namespace raytracer;


void cornell_box(bool use_phong) {
  Scene scene;

  // light
  scene.ambient_light = Colour(0.03, 0.03, 0.03);
  auto mat_light = make_shared<Light>(Colour(1, 1, 1), 0.2);
  scene.add(make_shared<Quad>(Point(343, 554, 332), Vec(-130,0,0), Vec(0,0,-105), mat_light));

  // walls
  shared_ptr<Material> red, white, green;
  if (use_phong) {
    red   = (shared_ptr<Material>) make_shared<Phong>(Colour(.65, .05, .05), 100);
    white = (shared_ptr<Material>) make_shared<Phong>(Colour(.73, .73, .73), 100);
    green = (shared_ptr<Material>) make_shared<Phong>(Colour(.12, .45, .15), 100);
  } else {
    red   = (shared_ptr<Material>) make_shared<Lambertian>(Colour(.65, .05, .05));
    white = (shared_ptr<Material>) make_shared<Lambertian>(Colour(.73, .73, .73));
    green = (shared_ptr<Material>) make_shared<Lambertian>(Colour(.12, .45, .15));
  }
  scene.add(make_shared<Quad>(Point(555,0,0), Vec(0,555,0), Vec(0,0,555), green)); // left
  scene.add(make_shared<Quad>(Point(0,0,0), Vec(0,555,0), Vec(0,0,555), red));     // right
  scene.add(make_shared<Quad>(Point(0,0,0), Vec(555,0,0), Vec(0,0,555), white));   // floor
  scene.add(make_shared<Quad>(Point(555,555,555), Vec(-555,0,0), Vec(0,0,-555), white)); // ceiling
  scene.add(make_shared<Quad>(Point(0,0,555), Vec(555,0,0), Vec(0,555,0), white)); // back

  // boxes
  scene.add(make_shared<Box>(Point(130, 0, 65), Point(295, 165, 230), white));
  scene.add(make_shared<Box>(Point(265, 0, 295), Point(430, 330, 460), white));

  // mirror sphere above big box
  auto mirror = make_shared<Metal>(Colour(0.8, 0.8, 0.8), 0.0);
  scene.add(make_shared<Sphere>(Point(400, 410, 350), 80, mirror));

  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 1.0;
  camera.image_width = 600;
  camera.samples_per_pixel = 10;
  camera.max_depth = 15;
  camera.vfov = 40;
  camera.look_from = Point(278, 278, -800);
  camera.look_at = Point(278, 278, 0);

  utils::clock([&camera]() { camera.render(); });
}


// a scene with Quads and a strong ambient light
void quads(bool use_phong) {
  Scene scene;

  scene.ambient_light = Colour(0.1, 0.1, 0.1);
  auto material_light = make_shared<Light>(Colour(1.0, 1.0, 1.0), 1);
  scene.add(make_shared<Sphere>(Point(1, 2, 0), 0.2, material_light));

  shared_ptr<Material> left_red, back_green, upper_orange, lower_cyan;
  if (use_phong) {
    left_red     = (shared_ptr<Material>) make_shared<Phong>(Colour(1.0, 0.2, 0.2), 10);
    back_green   = (shared_ptr<Material>) make_shared<Phong>(Colour(0.2, 1.0, 0.2), 10);
    upper_orange = (shared_ptr<Material>) make_shared<Phong>(Colour(1.0, 0.5, 0.0), 10);
    lower_cyan   = (shared_ptr<Material>) make_shared<Phong>(Colour(0.2, 0.8, 0.8), 10);
  } else {
    left_red     = (shared_ptr<Material>) make_shared<Lambertian>(Colour(1.0, 0.2, 0.2));
    back_green   = (shared_ptr<Material>) make_shared<Lambertian>(Colour(0.2, 1.0, 0.2));
    upper_orange = (shared_ptr<Material>) make_shared<Lambertian>(Colour(1.0, 0.5, 0.0));
    lower_cyan   = (shared_ptr<Material>) make_shared<Lambertian>(Colour(0.2, 0.8, 0.8));
  }
  auto blue_metal = make_shared<Metal>(Colour(0.4, 0.4, 1.0), 0.03);

  scene.add(make_shared<Quad>(Point(-3,-2, 5), Vec(0, 0,-4), Vec(0, 4, 0), left_red));
  scene.add(make_shared<Quad>(Point(-2,-2, 0), Vec(4, 0, 0), Vec(0, 4, 0), back_green));
  scene.add(make_shared<Quad>(Point(-2, 3, 1), Vec(4, 0, 0), Vec(0, 0, 4), upper_orange));
  scene.add(make_shared<Quad>(Point(-2,-3, 5), Vec(4, 0, 0), Vec(0, 0,-4), lower_cyan));
  scene.add(make_shared<Quad>(Point( 3,-2, 1), Vec(0, 0, 4), Vec(0, 4, 0), blue_metal));

  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 1.0;
  camera.image_width = 600;
  camera.samples_per_pixel = 30;
  camera.max_depth = 15;
  camera.vfov = 80;
  camera.look_from = Point(0,0,9);
  camera.look_at = Point(0,0,0);

  utils::clock([&camera]() { camera.render(); });
}


// a scene with Spheres and a Point Light
void spheres(bool use_phong) {
  Scene scene;

  scene.ambient_light = Colour(0.05, 0.05, 0.05);
  auto material_light  = make_shared<Light>(Colour(1, 1, 0), 1);
  scene.add(make_shared<Sphere>(Point( 2.0,    0.0, -2.0), 0.5, material_light));

  shared_ptr<Material> ground, center;
  if (use_phong) {
    ground = (shared_ptr<Material>) make_shared<Phong>(Colour(0.8, 0.8, 0.0), 100);
    center = (shared_ptr<Material>) make_shared<Phong>(Colour(0.1, 0.2, 0.5), 100);
  } else {
    ground = (shared_ptr<Material>) make_shared<Lambertian>(Colour(0.8, 0.8, 0.0));
    center = (shared_ptr<Material>) make_shared<Lambertian>(Colour(0.1, 0.2, 0.5));
  }
  auto metal1 = make_shared<Metal>(Colour(0.4, 0.4, 0.4), 0.0);
  auto metal2 = make_shared<Metal>(Colour(0.2, 0.8, 0.2), 0.2);
  auto glass  = make_shared<Dielectric>(1.5);
  auto bubble = make_shared<Dielectric>(1.0/1.5);
  scene.add(make_shared<Sphere>(Point( 0.0, -100.5, -2.0), 100.0, ground));
  scene.add(make_shared<Sphere>(Point( 0.0,    0.0, -2.2), 0.5, center));
  scene.add(make_shared<Sphere>(Point(-1.0,    0.0, -2.0), 0.5, metal1));
  scene.add(make_shared<Sphere>(Point( 0.3,   -0.1, -0.7), 0.2, glass));
  // scene.add(make_shared<Sphere>(Point( 0.3,  -0.15, -0.7), 0.15, bubble));


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

  utils::clock([&camera]() { camera.render(); });
}


// a scene with Phong materials, Spheres and a Point Light
void phong() {
  Scene scene;

  scene.ambient_light = Colour(0.03, 0.03, 0.03);
  auto material_light = make_shared<Light>(Colour(1, 1, 1), 0.4);
  scene.add(make_shared<Sphere>(Point(2.0, 1.0, -2.0), 0.1, material_light));

  auto material_ground = make_shared<Phong>(Colour(0.8, 0.0, 0.8), 100);
  auto material_center = make_shared<Phong>(Colour(0.8, 0.8, 0.0), 3);
  // auto material_center = make_shared<Metal>(Colour(0.8, 0.8, 0.8), 0.0);
  auto material_left   = make_shared<Phong>(Colour(0.0, 0.8, 0.8), 10);
  scene.add(make_shared<Sphere>(Point( 0.0, -100.5, -2.0), 100.0, material_ground));
  scene.add(make_shared<Sphere>(Point( 0.0,    0.0, -2.2), 0.5, material_center));
  scene.add(make_shared<Sphere>(Point(-1.0,    0.0, -2.0), 0.5, material_left));


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

  utils::clock([&camera]() { camera.render(); });
}

int main() {
  // false to use Lambertian, true to use Phong
  switch (4) {
    case 0: phong(); break;
    case 1: cornell_box(false); break;
    case 2: cornell_box(true); break;
    case 3: quads(false); break;
    case 4: quads(true); break;
    case 5: spheres(false); break;
    case 6: spheres(true); break;
  }
}
