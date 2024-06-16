// main.cpp

#include "utils/common.hpp"
#include "utils/utils.hpp"
#include "primitives/2d.hpp"
#include "primitives/box.hpp"
#include "primitives/mesh.hpp"
#include "primitives/sphere.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "scene.hpp"

using namespace raytracer;


// a scene with Phong and PhongMirror materials, composed by spheres and a point light
void phong() {
  Scene scene;
  scene.background = Colour(0.1, 0.1, 0.1);

  // light
  scene.ambient_light = Colour(0.05, 0.05, 0.05);
  auto material_light = make_shared<LightMat>(Colour(1, 1, 1), 1);
  scene.add(make_shared<Sphere>(Point(2.5, 0.7, -2.0), 0.1, material_light));

  // background
  auto material_background = make_shared<Phong>(Colour(0.1, 0.1, 0.1), 100);
  scene.add(make_shared<Quad>(Point(-400, -40, -40), Vec(800, 0, 0), Vec(0, 80, 0), material_background));

  // spheres
  auto material_ground = make_shared<Phong>(Colour(0.8, 0.0, 0.8), 100);
  auto material_right  = make_shared<Phong>(Colour(0.8, 0.8, 0.0), 100);
  auto material_center = make_shared<Phong>(Colour(0.0, 0.8, 0.8), 2);
  scene.add(make_shared<Sphere>(Point( 0.0, -100.5, -2.0), 100.0, material_ground));
  scene.add(make_shared<Sphere>(Point( 1.0,    0.0, -2.2), 0.5, material_right));
  scene.add(make_shared<Sphere>(Point( 0.0,    0.0, -2.0), 0.5, material_center));

  // mirror sphere
  auto material_mirror = make_shared<PhongMirror>(Colour(0.4, 0.4, 0.4), 1000, 0.02);
  scene.add(make_shared<Sphere>(Point(-2.0, 0.5, -3.0), 1, material_mirror));


  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 16.0/9.0;
  camera.image_width = 400;
  camera.samples_per_pixel = 16;
  camera.vfov = 90.0;
  camera.look_from = Point(0,0,0);
  camera.look_at = Point(0,0,-1);

  utils::clock([&camera]() { camera.render(); });
}


// a scene with Diffuse and Metal/Mirror materials, composed with quads, boxes, a sphere and a quad light
void cornell_box(bool use_phong) {
  Scene scene;
  scene.background = Colour(0.1, 0.1, 0.1);

  // light
  auto mat_light = make_shared<LightMat>(Colour(1, 1, 1), 5);
  scene.add(make_shared<Quad>(Point(343, 554, 332), Vec(-130,0,0), Vec(0,0,-105), mat_light));
  // scene.add(make_shared<Sphere>(Point(278, 554, 278), 50, mat_light));

  shared_ptr<Material> red, white, green, mirror;
  if (use_phong) {
    scene.ambient_light = Colour(0.03, 0.03, 0.03);
    red    = (shared_ptr<Material>) make_shared<Phong>(Colour(.65, .05, .05), 10);
    white  = (shared_ptr<Material>) make_shared<Phong>(Colour(.73, .73, .73), 100);
    green  = (shared_ptr<Material>) make_shared<Phong>(Colour(.12, .45, .15), 10);
    // green  = (shared_ptr<Material>) make_shared<PhongMirror>(Colour(.12, .45, .15), 10, 0.1);
    mirror = (shared_ptr<Material>) make_shared<PhongMirror>(Colour(0.8, 0.8, 0.8), 10, 0.1);
    // mirror = (shared_ptr<Material>) make_shared<Phong>(Colour(0.8, 0.8, 0.8), 10);
  } else {
    scene.ambient_light = Colour(0.15, 0.15, 0.15); // scene is too dark
    red    = (shared_ptr<Material>) make_shared<Diffuse>(Colour(.65, .05, .05));
    white  = (shared_ptr<Material>) make_shared<Diffuse>(Colour(.73, .73, .73));
    // green  = (shared_ptr<Material>) make_shared<Metal>(Colour(.12, .45, .15), 0.3);
    // mirror = (shared_ptr<Material>) make_shared<Metal>(Colour(0.8, 0.8, 0.8), 0.0);
    green  = (shared_ptr<Material>) make_shared<Diffuse>(Colour(.12, .45, .15));
  }

  // walls
  scene.add(make_shared<Quad>(Point(555,0,0), Vec(0,555,0), Vec(0,0,555), green)); // left
  scene.add(make_shared<Quad>(Point(0,0,0), Vec(0,555,0), Vec(0,0,555), red));     // right
  scene.add(make_shared<Quad>(Point(0,0,0), Vec(555,0,0), Vec(0,0,555), white));   // floor
  scene.add(make_shared<Quad>(Point(555,555,555), Vec(-555,0,0), Vec(0,0,-555), white)); // ceiling
  scene.add(make_shared<Quad>(Point(0,0,555), Vec(555,0,0), Vec(0,555,0), white)); // back

  // boxes
  scene.add(make_shared<Box>(Point(130, 0, 65), Point(295, 165, 230), white));
  scene.add(make_shared<Box>(Point(265, 0, 295), Point(430, 330, 460), white));

  // sphere
  // scene.add(make_shared<Sphere>(Point(400, 410, 350), 80, mirror));

  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 1;
  camera.image_width = 400;
  camera.samples_per_pixel = use_phong ? 4 : 9;
  camera.max_depth = 100;
  // camera.russian_roulette = false; // for tests
  camera.vfov = 40;
  camera.look_from = Point(278, 278, -800);
  camera.look_at = Point(278, 278, 0);

  utils::clock([&camera]() { camera.render(); });
}


// a scene with quads, a mirror, a strong ambient light and a point light
void quads(bool use_phong) {
  Scene scene;
  scene.background = Colour(0.1, 0.1, 0.1);

  // light
  scene.ambient_light = Colour(0.1, 0.1, 0.1);
  auto material_light = make_shared<LightMat>(Colour(1.0, 1.0, 1.0), 1.5);
  scene.add(make_shared<Sphere>(Point(1, 2, 0), 0.2, material_light));

  shared_ptr<Material> left_red, back_green, upper_orange, lower_cyan, blue_metal;
  if (use_phong) {
    left_red     = (shared_ptr<Material>) make_shared<Phong>(Colour(1.0, 0.2, 0.2), 10);
    back_green   = (shared_ptr<Material>) make_shared<Phong>(Colour(0.2, 1.0, 0.2), 10);
    upper_orange = (shared_ptr<Material>) make_shared<Phong>(Colour(1.0, 0.5, 0.0), 100);
    lower_cyan   = (shared_ptr<Material>) make_shared<Phong>(Colour(0.2, 0.8, 0.8), 10);
    blue_metal   = (shared_ptr<Material>) make_shared<PhongMirror>(Colour(0.4, 0.4, 1.0), 10, 0.2);
  } else {
    left_red     = (shared_ptr<Material>) make_shared<Diffuse>(Colour(1.0, 0.2, 0.2));
    back_green   = (shared_ptr<Material>) make_shared<Diffuse>(Colour(0.2, 1.0, 0.2));
    upper_orange = (shared_ptr<Material>) make_shared<Diffuse>(Colour(1.0, 0.5, 0.0));
    lower_cyan   = (shared_ptr<Material>) make_shared<Diffuse>(Colour(0.2, 0.8, 0.8));
    blue_metal   = (shared_ptr<Material>) make_shared<Metal>(Colour(0.4, 0.4, 1.0), 0.03);
  }

  // quads
  scene.add(make_shared<Quad>(Point(-3,-2, 5), Vec(0, 0,-4), Vec(0, 4, 0), left_red));
  scene.add(make_shared<Quad>(Point(-2,-2, 0), Vec(4, 0, 0), Vec(0, 4, 0), back_green));
  scene.add(make_shared<Quad>(Point(-2, 3, 1), Vec(4, 0, 0), Vec(0, 0, 4), upper_orange));
  scene.add(make_shared<Quad>(Point(-2,-3, 5), Vec(4, 0, 0), Vec(0, 0,-4), lower_cyan));
  scene.add(make_shared<Quad>(Point( 3,-2, 1), Vec(0, 0, 4), Vec(0, 4, 0), blue_metal));

  /////////////////////

  Camera camera(scene);

  camera.image_width = 400;
  camera.samples_per_pixel = 9;
  camera.max_depth = 15;
  camera.vfov = 80;
  camera.look_from = Point(0,0,9);
  camera.look_at = Point(0,0,0);

  utils::clock([&camera]() { camera.render(); });
}


// a scene one spheres of each pathtracing material (diffuse, metal, dielectric) and a point light
void spheres(bool use_phong) {
  Scene scene;
  scene.background = Colour(0.1, 0.1, 0.5);

  // light
  scene.ambient_light = Colour(0.05, 0.05, 0.05);
  auto material_light = make_shared<LightMat>(Colour(1, 1, 0.2), 1);
  scene.add(make_shared<Sphere>(Point( 2.0,    0.0, -2.0), 0.5, material_light));

  shared_ptr<Material> ground, center;
  if (use_phong) {
    ground = (shared_ptr<Material>) make_shared<Phong>(Colour(0.5, 0.0, 0.0), 100);
    center = (shared_ptr<Material>) make_shared<Phong>(Colour(0.1, 0.2, 0.5), 100);
  } else {
    ground = (shared_ptr<Material>) make_shared<Diffuse>(Colour(0.5, 0.0, 0.0));
    center = (shared_ptr<Material>) make_shared<Diffuse>(Colour(0.1, 0.2, 0.5));
  }
  auto mirror = make_shared<Metal>(Colour(0.4, 0.4, 0.4), 0.0);
  auto metal  = make_shared<Metal>(Colour(0.2, 0.8, 0.2), 0.2);
  auto glass  = make_shared<Dielectric>(1.5);
  auto bubble = make_shared<Dielectric>(1.0/1.5);
  scene.add(make_shared<Sphere>(Point( 0.0, -100.5, -2.0), 100.0, ground));
  scene.add(make_shared<Sphere>(Point( 0.0,    0.0, -2.2), 0.5, center));
  scene.add(make_shared<Sphere>(Point(-1.0,    0.0, -2.0), 0.5, mirror));
  scene.add(make_shared<Sphere>(Point( 0.3,   -0.1, -0.5), 0.14, glass));

  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 16.0 / 9.0;
  camera.image_width = 400;
  camera.samples_per_pixel = 25;
  camera.max_depth = 20;
  camera.vfov = 90.0;
  camera.look_from = Point(0,0,0);
  camera.look_at = Point(0,0,-1);

  utils::clock([&camera]() { camera.render(); });
}


// a scene with a Mesh bunny, strong ambient light and a point Light
void bunny() {
  Scene scene;
  scene.background = Colour(0.1, 0.1, 0.1);

  // light
  scene.ambient_light = Colour(0.2, 0.2, 0.2);
  auto material_light = make_shared<LightMat>(Colour(1, 1, 0), 2);
  scene.add(make_shared<Sphere>(Point(-0.05, 0.15, 0.2), 0.01, material_light));
  // scene.add(make_shared<Sphere>(Point(-0.05, 0.15, -0.2), 0.01, material_light));

  // bunny
  auto bunny_material = make_shared<Phong>(Colour(0.5, 0.5, 0.5), 500);
  auto bunny = make_shared<raytracer::Mesh>("assets/bunny.obj", bunny_material);
  scene.add(bunny);

  // ground
  auto ground = make_shared<Phong>(Colour(0.2, 0.7, 0.0), 10);
  scene.add(make_shared<Sphere>(Point(0, -98, -20), 100, ground));

  // sphere
  auto mirror = make_shared<PhongMirror>(Colour(0.8, 0.8, 0.8), 1000, 0.02);
  scene.add(make_shared<Sphere>(Point(-0.8, 0.5, -0.5), 0.4, mirror));

  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 16.0/9.0;
  camera.image_width = 300;
  camera.samples_per_pixel = 4;
  camera.vfov = 50.0;
  camera.look_from = Point(0, 0, 0.5);
  camera.look_at = Point(-0.5, 0.6, -1);

  utils::clock([&camera]() { camera.render(); });
}


// a scene with Phong spheres, a Metal mirror and a point light
void spheres_and_mirror() {
  Scene scene;
  scene.background = Colour(0.1, 0.1, 0.1);

  // light
  scene.ambient_light = Colour(0.05, 0.05, 0.05);
  auto material_light = make_shared<LightMat>(Colour(1, 1, 1), 3);
  scene.add(make_shared<Sphere>(Point(2.5, 0.7, -2.0), 0.1, material_light));

  // spheres
  auto material_ground = make_shared<Phong>(Colour(0.8, 0.0, 0.8), 100);
  auto material_right  = make_shared<Phong>(Colour(0.8, 0.8, 0.0), 100);
  auto material_center = make_shared<Phong>(Colour(0.0, 0.8, 0.8), 2);
  scene.add(make_shared<Sphere>(Point( 0.0, -100.5, -2.0), 100.0, material_ground));
  scene.add(make_shared<Sphere>(Point( 1.0,    0.0, -2.2), 0.5, material_right));
  scene.add(make_shared<Sphere>(Point( 0.0,    0.0, -2.0), 0.5, material_center));

  // mirror quad
  auto material_metal = make_shared<Metal>(Colour(0.4, 0.4, 0.4), 0.0);
  scene.add(make_shared<Quad>(Point(-4, -1, -3), Vec(2, 0, -2), Vec(0, 2, 0), material_metal));

  /////////////////////

  Camera camera(scene);

  camera.aspect_ratio = 16.0 / 9.0;
  camera.image_width = 800;
  camera.samples_per_pixel = 20;
  camera.max_depth = 5;
  camera.vfov = 90.0;
  camera.look_from = Point(0,0,0);
  camera.look_at = Point(0,0,-1);
  camera.defocus_angle = 0;
  camera.focus_dist = 1;

  utils::clock([&camera]() { camera.render(); });
}


int main() {
  switch (11) {
    // phong materials
    case 0: phong(); break;
    case 1: cornell_box(true); break;
    case 2: quads(true); break;
    case 3: bunny(); break;

    // pathtracing materials (experimental)
    case 10: spheres(false); break;
    case 11: cornell_box(false); break;
    case 12: quads(false); break;

    // mixed phong and pathtracing materials (experimental)
    case 20: spheres_and_mirror(); break;
    case 21: spheres(true); break;
  }
}
