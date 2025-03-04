#pragma once
#include "Hittable.h"
#include "Material.h"

class Sphere :
	public Hittable
{
public:
	Sphere(DirectX::XMFLOAT3 _origin, float _radius, std::shared_ptr<Material> _material) :
		origin(_origin),
		radius(fmax(0.0f, _radius)),
		material(_material)
	{}
	bool Hit(const Ray& _ray, Interval _rayT, HitRecord& _record) const override;
private:
	DirectX::XMFLOAT3 origin;
	float radius;
	shared_ptr<Material> material;
};

