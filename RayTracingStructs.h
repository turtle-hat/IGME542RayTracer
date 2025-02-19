#pragma once
#include <DirectXMath.h>

struct Ray {
	DirectX::XMFLOAT3 Origin;
	DirectX::XMFLOAT3 Direction;

	// Scales direction by t and adds to origin
	DirectX::XMVECTOR At(float t) const {
		return DirectX::XMVectorAdd(
			DirectX::XMLoadFloat3(&Origin),
			DirectX::XMVectorScale(DirectX::XMLoadFloat3(&Direction), t)
		);
	}
};

struct Sphere {
	DirectX::XMFLOAT3 Origin;
	float Radius;
};
