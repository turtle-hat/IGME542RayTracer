#pragma once
#include <DirectXMath.h>

#include "RayTracingStructs.h"

class Helpers
{
public:
	// Ray-Sphere Intersection
	static float HitSphere(const Sphere& _sphere, const Ray& _ray);
};

