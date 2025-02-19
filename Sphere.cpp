#include "Sphere.h"

using namespace DirectX;

bool Sphere::Hit(const Ray& _ray, float _rayTMin, float _rayTMax, HitRecord& _record) const
{
	// Load relevant data
	XMVECTOR vecSphereOri = XMLoadFloat3(&origin);
	XMVECTOR vecRayOri = XMLoadFloat3(&_ray.Origin);
	XMVECTOR vecRayDir = XMLoadFloat3(&_ray.Direction);

	// Find vector from ray's origin to sphere's center
	XMVECTOR rayToSphere = vecSphereOri - vecRayOri;

	// Calculate ray-sphere intersection
	float a, h, c;

	XMStoreFloat(&a, XMVector3LengthSq(vecRayDir));
	XMStoreFloat(&h, XMVector3Dot(vecRayDir, rayToSphere));
	XMStoreFloat(&c, XMVector3LengthSq(rayToSphere));
	c -= radius * radius;

	float discriminant = (h * h) - (a * c);

	if (discriminant < 0) return false;

	float sqrtd = sqrt(discriminant);

	// Find nearest root of the equation in an acceptable range
	float root = (h - sqrtd) / a;
	if (root <= _rayTMin || _rayTMax <= root) {
		root = (h + sqrtd) / a;
		if (root <= _rayTMin || _rayTMax <= root) {
			return false;
		}
	}

	// Build hit record
	_record.t = root;
	XMVECTOR hitPoint = _ray.At(root);
	XMStoreFloat3(&_record.point, hitPoint);
	_record.SetFaceNormal(vecRayDir, XMVectorScale(hitPoint - vecSphereOri, 1.0f / radius));

	return true;
}
