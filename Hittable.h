#pragma once
#include "Helpers.h"

class Material;

class HitRecord {
public:
	DirectX::XMFLOAT3 point;
	DirectX::XMFLOAT3 normal;
	shared_ptr<Material> material;
	float t;
	bool isFrontFace;

	void SetFaceNormal(const DirectX::XMVECTOR& _rayDirection, const DirectX::XMVECTOR& _outwardNormal);
};

class Hittable
{
public:
	virtual ~Hittable() = default;
	virtual bool Hit(const Ray& _ray, Interval _rayT, HitRecord& _record) const = 0;
};

