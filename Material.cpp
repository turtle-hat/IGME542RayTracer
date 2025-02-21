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
	XMStoreFloat3(&_scattered.Direction,
		XMVector3Reflect(XMLoadFloat3(&_rayIn.Direction), XMLoadFloat3(&_record.normal))
	);
	_attenuation = XMLoadFloat3(&albedo);
	return true;
}
