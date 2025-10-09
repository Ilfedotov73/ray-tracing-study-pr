#ifndef RT_SETTINGS_H
#define RT_SEGGINGS_H

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

/* Constants */
const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

/* Utility Functions */
inline double degrees_to_radians(double degrees) { return degrees * PI / 180.0; }

inline double random_double() { return std::rand() / (RAND_MAX + 1.0); } // случайное число в [0,1)
inline double random_double(double min, double max) { return min + (max-min)*random_double(); } // случайное число в [min, max)
inline int random_int(int min, int max) { return int(random_double(min, max+1)); }

/* Common Headers */
#include "vec3.h"
#include "interval.h"
#include "ray.h"
#include "color.h"
#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "camera.h"
#include "sphere.h"
#include "bvh.h"

#endif