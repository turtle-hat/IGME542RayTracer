#pragma once
#include "Helpers.h"

// Returns a random Vector3
inline DirectX::XMVECTOR RandomVector() {
	DirectX::XMFLOAT3 result(RandomFloat(), RandomFloat(), RandomFloat());
	return DirectX::XMLoadFloat3(&result);
}

// Returns a random Vector3 within the given bounds
inline DirectX::XMVECTOR RandomVector(float _min, float _max) {
	DirectX::XMFLOAT3 result(RandomFloat(_min, _max), RandomFloat(_min, _max), RandomFloat(_min, _max));
	return DirectX::XMLoadFloat3(&result);
}

inline DirectX::XMVECTOR RandomUnitVector() {
	while (true) {
		// Generate random vector
		DirectX::XMVECTOR p = RandomVector(-1.0f, 1.0f);

		// Find its length
		float lengthSquared;
		DirectX::XMStoreFloat(&lengthSquared, DirectX::XMVector3LengthSq(p));

		// If float length is smaller than minimum float value or larger than 1, reject
		if (std::numeric_limits<float>::denorm_min() < lengthSquared && lengthSquared <= 1.0f) {
			// If acceptable, normalize and return
			return DirectX::XMVector3Normalize(p);
		}
	}
}

inline DirectX::XMFLOAT3 RandomOnHemisphere(const DirectX::XMFLOAT3 _normal) {
	// Get random unit vector
	DirectX::XMVECTOR onUnitSphere = RandomUnitVector();

	// Find dot product between new vector and normal
	float dot;
	DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(onUnitSphere, DirectX::XMLoadFloat3(&_normal)));

	// If dot product shows not in same hemisphere, flip it
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, dot > 0.0f ? onUnitSphere : DirectX::XMVectorScale(onUnitSphere, -1.0f));
	return result;
}

inline DirectX::XMVECTOR LinearToGamma(DirectX::XMVECTOR _linearColor) {
	// Find square root to gamma correct
	return DirectX::XMVectorSqrt(
		// Clamp _linearColor above 0
		DirectX::XMVectorMax(
			_linearColor,
			DirectX::XMVectorZero()
		)
	);
}