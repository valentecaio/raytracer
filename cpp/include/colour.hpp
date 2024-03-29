#ifndef COLOUR_H
#define COLOUR_H

#include <glm/glm.hpp>
#include <iostream>

namespace raytracer {

using colour3 = glm::vec3;

void write_colour(std::ostream &out, colour3 pixel_color) {
  out << static_cast<int>(pixel_color.r) << ' '
      << static_cast<int>(pixel_color.g) << ' '
      << static_cast<int>(pixel_color.b) << '\n';
}

} // namespace raytracer

#endif // COLOUR_H
