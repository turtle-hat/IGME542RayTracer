#pragma once
#include <DirectXMath.h>

struct Ray {
	DirectX::XMFLOAT3 Origin;
	DirectX::XMFLOAT3 Direction;

	// Scales direction by t and adds to origin
	DirectX::XMFLOAT3 At(float t) const {
		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, DirectX::XMVectorAdd(
			DirectX::XMLoadFloat3(&Origin),
			DirectX::XMVectorScale(DirectX::XMLoadFloat3(&Direction), t)
		));
		return result;
	}
};
