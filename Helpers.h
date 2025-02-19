#pragma once

#include <cmath>
#include <limits>
#include <memory>



// C++ Std Usings
using std::make_shared;
using std::shared_ptr;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385f;

// Utility Functions

inline float DegreesToRadians(float degrees) {
	return degrees * pi / 180.0f;
}

// Common Headers

#include <DirectXMath.h>
#include "RayTracingStructs.h"
#include "Interval.h"
