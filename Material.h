#pragma once
#include "Helpers.h"
#include "Hittable.h"

class Material
{
public:
	virtual ~Material() = default;

	virtual bool Scatter(
		const Ray& _rayIn, const HitRecord& _record, DirectX::XMFLOAT4 _attenuation, Ray& _scattered
	) const {
		return false;
	}
};

class Lambertian : public Material {
public:
	Lambertian(const DirectX::XMFLOAT4& albedo) : albedo(albedo) {}

	bool Scatter(
		const Ray& _rayIn, const HitRecord& _record, DirectX::XMFLOAT4 _attenuation, Ray& _scattered
	) const override;

private:
	DirectX::XMFLOAT4 albedo;
};

