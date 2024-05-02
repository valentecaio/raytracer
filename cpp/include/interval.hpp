#ifndef INTERVAL_H
#define INTERVAL_H

#include "common.hpp"

namespace raytracer {

// The Interval class represents a closed interval [min, max].
class Interval {
  public:
    double min, max;

    Interval() : min(+INFINITY), max(-INFINITY) {}
    Interval(double _min, double _max) : min(_min), max(_max) {}

    bool contains(double x) const  { return min <= x && x <= max; }
    bool surrounds(double x) const { return min < x && x < max; }
    double clamp(double x) const { return std::min(std::max(x, min), max); }

    static const Interval empty, universe;
};

const static Interval empty   (+INFINITY, -INFINITY);
const static Interval universe(-INFINITY, +INFINITY);

} // namespace raytracer

#endif // INTERVAL_H
