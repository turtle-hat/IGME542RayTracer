#include "Helpers.h"

using namespace DirectX;

float Helpers::HitSphere(Sphere _sphere, Ray _ray)
{
	// Load relevant data
	XMVECTOR vecSphereOri = XMLoadFloat3(&_sphere.Origin);
	XMVECTOR vecRayOri = XMLoadFloat3(&_ray.Origin);
	XMVECTOR vecRayDir = XMLoadFloat3(&_ray.Direction);

	// Find vector from ray's origin to sphere's center
	XMVECTOR rayToSphere = vecSphereOri - vecRayOri;

	// Calculate ray-sphere intersection
	float a, h, c;

	XMStoreFloat(&a, XMVector3LengthSq(vecRayDir));
	XMStoreFloat(&h, XMVector3Dot(vecRayDir, rayToSphere));
	XMStoreFloat(&c, XMVector3LengthSq(rayToSphere));
	c -= _sphere.Radius * _sphere.Radius;

	float discriminant = (h * h) - (a * c);

	return discriminant < 0 ? 
		-1.0f :
		(h - sqrt(discriminant)) / a;
}
