#pragma once

#include <cmath>
#include <cstdlib>
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

inline float RandomFloat() {
	// Returns a random real in [0,1).
	return std::rand() / (RAND_MAX + 1.0f);
}

inline float RandomFloat(float _min, float _max) {
	// Returns a random real in [_min,_max).
	return _min + (_max - _min) * RandomFloat();
}

// Common Headers

#include <DirectXMath.h>
#include "RayTracingStructs.h"
#include "Interval.h"
