#include "Hittable.h"

using namespace DirectX;

void HitRecord::SetFaceNormal(const DirectX::XMVECTOR& _rayDirection, const DirectX::XMVECTOR& _outwardNormal)
{
	// Sets the hit record's normal vector.
	// NOTE: The parameter `_outwardNormal` is assumed to have unit length.

	float dotRayNormal;
	XMStoreFloat(&dotRayNormal, XMVector3Dot(_rayDirection, _outwardNormal));
	isFrontFace = dotRayNormal < 0;
	XMStoreFloat3(&normal, isFrontFace ? _outwardNormal : -_outwardNormal);
}
