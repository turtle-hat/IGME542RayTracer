#pragma once
#include "Helpers.h"

class HitRecord {
public:
	DirectX::XMFLOAT3 point;
	DirectX::XMFLOAT3 normal;
	float t;
	bool isFrontFace;

	void SetFaceNormal(const DirectX::XMVECTOR& _rayDirection, const DirectX::XMVECTOR& _outwardNormal);
};

class Hittable
{
public:
	virtual ~Hittable() = default;
	virtual bool Hit(const Ray& _ray, float _rayTMin, float _rayTMax, HitRecord& _record) const = 0;
};

