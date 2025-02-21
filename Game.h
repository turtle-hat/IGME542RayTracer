#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <vector>

#include "CPUTexture.h"
#include "Camera.h"
#include "RayTracingStructs.h"
#include "Sphere.h"
#include "HittableList.h"

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

	// How much to scale the render texture if the camera is moving or static
	const float STATIC_TEXTURE_SCALE = 0.25f;
	const float MOVING_TEXTURE_SCALE = 0.05f;

	// --- VARIABLES ---

	std::shared_ptr<FPSCamera> camera;
	std::shared_ptr<CPUTexture> cpuTexture;

	// Scene Variables
	HittableList world;





	// Rendering Process Variables

	// Current row of pixels being rendered
	unsigned int currentScanline;
	// Whether the camera moved last frame
	bool wasInputDetectedLastFrame = false;





	// FUNCTIONS

	// Initialization helper functions

	void InitializeWorld();
};

