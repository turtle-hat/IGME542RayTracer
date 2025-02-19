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
	float a, b, c;

	XMStoreFloat(&a, XMVector3Dot(vecRayDir, vecRayDir));
	XMStoreFloat(&b, XMVector3Dot(vecRayDir, rayToSphere));
	b *= -2.0f;
	XMStoreFloat(&c, XMVector3Dot(rayToSphere, rayToSphere));
	c -= _sphere.Radius * _sphere.Radius;

	float discriminant = (b * b) - (4 * a * c);

	return discriminant < 0 ? 
		-1.0f :
		(-b - sqrt(discriminant)) / (2.0f * a);
}
