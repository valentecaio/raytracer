#ifndef MESH_HPP
#define MESH_HPP

#include "../utils/common.hpp"
#include "../utils/interval.hpp"
#include "../utils/utils.hpp"
#include "../hittable/hit_record.hpp"
#include "../material.hpp"
#include "primitive.hpp"
#include "2d.hpp"
#include "box.hpp"

namespace raytracer {

// A mesh is list of 2D triangles with a bounding box to speed up the intersection tests.
// This is still a simple implementation, very inefficient and experimental.
class Mesh : public Primitive {
  public:
    Mesh() = default;
    ~Mesh() = default;

    // create mesh from list of triangles
    Mesh(const HittableList _triangles, const shared_ptr<Material> _material) {
      material = _material;
      triangles = _triangles;
      compute_bbox();
    }

    // create mesh from obj file
    Mesh(const std::string& filename, const shared_ptr<Material>& _material) {
      material = _material;
      load_obj(filename);
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& hitrec) const override {
      // the ray can only hit the mesh if it hits the bounding box first
      if (!bbox->hit(r, ray_t, hitrec))
        return false;

      // if the ray hits any triangle, the hit object is the mesh
      return triangles.hit(r, ray_t, hitrec) && (hitrec.object = shared_from_this());
    }

    // returns a random point in one of the triangles
    Point get_sample() const override {
      int idx = utils::random_int(0, triangles.objects.size() - 1);
      return triangles.objects[idx]->get_sample();
    }

    // load a mesh from an obj file (only simple triangulated meshes supported)
    void load_obj(const std::string& filename) {
      std::ifstream file(filename);
      if (!file.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return;
      }

      triangles = HittableList();
      std::vector<Point> vertices;
      std::string line;
      while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "v") {
          Point p;
          iss >> p.x >> p.y >> p.z;
          // std::clog << p.x << " " << p.y << " " << p.z << " " << vertices.size() << std::endl;
          vertices.push_back(p);
        } else if (token == "f") {
          int i, j, k;
          iss >> i >> j >> k;
          // std::clog << i << " " << j << " " << k << " " << vertices.size() << " ";
          auto t = make_shared<Triangle>(vertices[i-1], vertices[j-1], vertices[k-1], material);
          // std::clog << t->a.x << " " << t->a.y << " " << t->a.z << std::endl;
          triangles.add(t);
        }
      }
      file.close();
      compute_bbox();
    }


  private:
    HittableList triangles;
    shared_ptr<Box> bbox;   // TODO: replace by a proper BVH

    // compute the bounding box of the mesh
    void compute_bbox() {
      Point pmin = Point( infinity,  infinity,  infinity);
      Point pmax = Point(-infinity, -infinity, -infinity);
      for (const auto& triangle : triangles.objects) {
        auto t = *dynamic_cast<Triangle*>(triangle.get());
        pmin.x = utils::min({pmin.x, t.a.x, t.b.x, t.c.x});
        pmin.y = utils::min({pmin.y, t.a.y, t.b.y, t.c.y});
        pmin.z = utils::min({pmin.z, t.a.z, t.b.z, t.c.z});
        pmax.x = utils::max({pmax.x, t.a.x, t.b.x, t.c.x});
        pmax.y = utils::max({pmax.y, t.a.y, t.b.y, t.c.y});
        pmax.z = utils::max({pmax.z, t.a.z, t.b.z, t.c.z});
      }
      bbox = make_shared<Box>(pmin, pmax, material);
      // std::clog << "bbox: " << bbox->pmin.x << " " << bbox->pmin.y << " " << bbox->pmin.z << " " << bbox->pmax.x << " " << bbox->pmax.y << " " << bbox->pmax.z << std::endl;
    }
};

} // namespace raytracer

#endif // MESH_HPP
