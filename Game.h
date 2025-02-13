#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

#include "CPUTexture.h"
#include "Camera.h"

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
	void OnResize();

private:
	// --- CONSTANTS ---

	const float STATIONARY_TEXTURE_SCALE = 0.5f;
	const float MOVING_TEXTURE_SCALE = 0.1f;



	// --- VARIABLES ---

	std::shared_ptr<FPSCamera> camera;

	float textureScale;
	std::shared_ptr<CPUTexture> cpuTexture;

	// Rendering Variables

	// Current row of pixels being rendered
	unsigned int currentScanline;
	// Whether the camera moved last frame
	bool wasInputDetectedLastFrame = false;





	// FUNCTIONS

	// Initialization helper functions
	void InitializeParameters();
};

