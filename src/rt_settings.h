#ifndef RT_SETTINGS_H
#define RT_SEGGINGS_H

#include <iostream>
#include <cmath>
#include <memory>
#include <limits>
#include <cstdlib>

using std::make_shared;
using std::shared_ptr;

/* Constants */
const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

/* Utility Functions */
inline double degrees_to_radians(double degrees) { return degrees * PI / 180.0; }

inline double random_double() { return std::rand() / (RAND_MAX + 1.0); } // случайное число в [0,1)
inline double random_double(double min, double max) { return min + (max-min)*random_double(); } // случайное число в [min, max)

/* Common Headers */
#include "color.h"
#include "ray.h"
#include "interval.h"
#include "vec3.h"

#endif