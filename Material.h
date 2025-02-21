#pragma once
#include "Helpers.h"
#include "Hittable.h"

class Material
{
public:
	virtual ~Material() = default;

	virtual bool Scatter(
		const Ray& _rayIn, const HitRecord& _record, DirectX::XMVECTOR& _attenuation, Ray& _scattered
	) const {
		return false;
	}
};

class Lambertian : public Material {
public:
	Lambertian(const DirectX::XMFLOAT3& _albedo) : albedo(_albedo) {}

	bool Scatter(
		const Ray& _rayIn, const HitRecord& _record, DirectX::XMVECTOR& _attenuation, Ray& _scattered
	) const override;

private:
	DirectX::XMFLOAT3 albedo;
};

class Metal : public Material {
public:
	Metal(const DirectX::XMFLOAT3& _albedo) : albedo(_albedo) {}

	bool Scatter(
		const Ray& _rayIn, const HitRecord& _record, DirectX::XMVECTOR& _attenuation, Ray& _scattered
	) const override;

private:
	DirectX::XMFLOAT3 albedo;
};

