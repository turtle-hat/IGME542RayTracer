#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

#include "CPUTexture.h"
#include "Camera.h"
#include "Ray.h"

class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	// Run when window is resized
	void OnResize();

private:
	// --- CONSTANTS ---

	const float STATIONARY_TEXTURE_SCALE = 0.5f;
	const float MOVING_TEXTURE_SCALE = 0.1f;



	// --- VARIABLES ---

	std::shared_ptr<FPSCamera> camera;

	float textureScale;
	std::shared_ptr<CPUTexture> cpuTexture;

	// Image Variables

	// The amount one pixel takes up of the screen, if both dimensions are 1.0f
	DirectX::XMFLOAT2 viewportPixelPercentage;
	// Viewport dimensions
	DirectX::XMFLOAT2 viewportSize;

	// World positions of the vectors across the viewport
	// U and V are Camera's Right and -Up vectors, respectively
	DirectX::XMFLOAT3 viewportU;
	DirectX::XMFLOAT3 viewportV;
	// World delta vectors along each pixel
	DirectX::XMFLOAT3 pixelDeltaU;
	DirectX::XMFLOAT3 pixelDeltaV;
	// World position of the upper-leftmost position of the viewport
	DirectX::XMFLOAT3 upperLeftViewportLocation;
	// World position of the center of upper-leftmost pixel of the viewport
	DirectX::XMFLOAT3 upperLeftPixelCenter;





	// Rendering Process Variables

	// Current row of pixels being rendered
	unsigned int currentScanline;
	// Whether the camera moved last frame
	bool wasInputDetectedLastFrame = false;





	// FUNCTIONS

	// Initialization helper functions

	void InitializeParameters();
	// Resizes render texture and updates associated variables
	void UpdateViewportData();



	// Drawing helper functions

	// Find the color returned by a given ray
	DirectX::XMFLOAT4 RayColor(Ray _ray);
};

