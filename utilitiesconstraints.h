#ifndef UTILITIESNCONSTRAINTS_H
#define UTILITIESNCONSTRAINTS_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

//Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//Constraints

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

//Utility Functions
inline float degrees_to_radians(float degrees)
{
    return degrees * pi / 180.0;
}

inline float random_float() 
{
    return rand() / (RAND_MAX + 1.0);
}

inline float random_float(float min, float max)
{
    return min + (max-min) * random_float();
}

inline float random_int(int min, int max)
{
    return static_cast<int>(random_float(min, max+1));
}

inline float clamp(float x, float min, float max)
{
    if(x < min) return min;
    if(x > max) return max;
    return x;
}

//Common Headers
#include "ray.h"
#include "vec3.h"

#endif