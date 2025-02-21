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
	Metal(const DirectX::XMFLOAT3& _albedo, float _fuzz) : albedo(_albedo), fuzz(_fuzz < 1.0f ? _fuzz : 1.0f) {}

	bool Scatter(
		const Ray& _rayIn, const HitRecord& _record, DirectX::XMVECTOR& _attenuation, Ray& _scattered
	) const override;

private:
	DirectX::XMFLOAT3 albedo;
	float fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(float _refractionIndex) : refractionIndex(_refractionIndex) {}

	bool Scatter(
		const Ray& _rayIn, const HitRecord& _record, DirectX::XMVECTOR& _attenuation, Ray& _scattered
	) const override;

private:
	// Refractive index in air or a vacuum; ratio of material's refractive index
	// over the enclosing media's refractive index
	float refractionIndex;

	static float Reflectance(float _cosine, float _refractionIndex);
};


