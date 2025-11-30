#ifndef RT_SETTINGS_H
#define RT_SEGGINGS_H

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <limits>
#include <memory>
#include <tuple>
#include <vector>
#include <utility>

using std::make_shared;
using std::shared_ptr;

/* Constants */
const float INF = std::numeric_limits<float>::infinity();
const float PI = 3.1415926535897932385f;

/* Utility Functions */
inline float degrees_to_radians(float degrees) { return degrees * PI / 180.0f; }
inline float random_float() { return std::rand() / (RAND_MAX + 1.0f); } // случайное число в [0,1)
inline float random_float(float min, float max) { return min + (max-min)*random_float(); } // случайное число в [min, max)
inline int random_int(int min, int max) { return int(random_float(min, max+1)); }

/* Common Headers */
#include "vec3.h"
#include "interval.h"
#include "ray.h"
#include "color.h"
#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "rt_stb_image.h"
#include "texture.h"
#include "material.h"
#include "camera.h"
#include "sphere.h"
#include "quad.h"
#include "tri.h"
#include "bvh.h"
#include "rt_obj_loader.h"

#endif