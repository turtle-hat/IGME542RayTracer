#pragma once
#include <DirectXMath.h>

#include "RayTracingStructs.h"

class Helpers
{
public:
	// Ray-Sphere Intersection
	static float HitSphere(Sphere _sphere, Ray _ray);
};

