#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>

#include "image.hpp"
#include "point.hpp"

namespace raytracer {

class Image
{
  int image_width;
  int image_height;

public:
  Image(int width, int height);
  void generateImage(std::vector<std::vector<Point>>& points);
};

} // namespace raytracer

#endif // IMAGE_HPP
