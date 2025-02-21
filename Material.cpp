#include "Material.h"
#include "VectorHelpers.h"

using namespace DirectX;

bool Lambertian::Scatter(const Ray& _rayIn, const HitRecord& _record, DirectX::XMVECTOR& _attenuation, Ray& _scattered) const
{
	XMVECTOR vecNormal = XMLoadFloat3(&_record.normal);
	XMVECTOR scatterDirection = vecNormal + RandomUnitVector();

	// Catch degenerate scatter condition
	float scatterDirectionLength;
	XMStoreFloat(&scatterDirectionLength, XMVector3LengthSq(scatterDirection));
	if (scatterDirectionLength < std::numeric_limits<float>::epsilon()) {
		scatterDirection = vecNormal;
	}

	_scattered.Origin = _record.point;
	XMStoreFloat3(&_scattered.Direction, scatterDirection);
	_attenuation = XMLoadFloat3(&albedo);
	return true;
}

bool Metal::Scatter(const Ray& _rayIn, const HitRecord& _record, DirectX::XMVECTOR& _attenuation, Ray& _scattered) const
{
	_scattered.Origin = _record.point;
	XMVECTOR vecNormal = XMLoadFloat3(&_record.normal);
	XMVECTOR scatterDirection =
		// Normalize reflected ray
		XMVector3Normalize(
			// Calculate reflected ray
			XMVector3Reflect(XMLoadFloat3(&_rayIn.Direction), vecNormal)
		) +
		// Add fuzz
		XMVectorScale(RandomUnitVector(), fuzz);

	XMStoreFloat3(&_scattered.Direction, scatterDirection);

	_attenuation = XMLoadFloat3(&albedo);
	float dotScatteredNormal;
	XMStoreFloat(&dotScatteredNormal, XMVector3Dot(scatterDirection, vecNormal));
	
	// Only return if fuzz doesn't make scattered vector go into the surface
	return dotScatteredNormal > 0.0f;
}

bool Dielectric::Scatter(const Ray& _rayIn, const HitRecord& _record, DirectX::XMVECTOR& _attenuation, Ray& _scattered) const
{
	XMVECTOR vecUnitDirection = XMVector3Normalize(XMLoadFloat3(&_rayIn.Direction));
	XMVECTOR vecNormal = XMLoadFloat3(&_record.normal);

	float ri = _record.isFrontFace ? (1.0f / refractionIndex) : refractionIndex;

	float cosTheta;
	XMStoreFloat(&cosTheta, XMVector3Dot(XMVectorScale(vecUnitDirection, -1.0f), vecNormal));
	cosTheta = std::fmin(cosTheta, 1.0f);
	float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

	bool cannotRefract = ri * sinTheta > 1.0f;

	_scattered.Origin = _record.point;
	XMStoreFloat3(&_scattered.Direction, 
		cannotRefract || Reflectance(cosTheta, ri) > RandomFloat() ?
		XMVector3Reflect(vecUnitDirection, vecNormal) :
		XMVector3Refract(vecUnitDirection, vecNormal, ri)
	);
	_attenuation = XMLoadFloat3(&UNIT_VECTOR3);

	return true;
}

float Dielectric::Reflectance(float _cosine, float _refractionIndex)
{
	// Schlick's approximation for reflectance
	float r0 = (1.0f - _refractionIndex) / (1.0f + _refractionIndex);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * std::powf((1.0f - _cosine), 5.0f);
}
