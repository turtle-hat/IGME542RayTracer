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

	std::shared_ptr<FPSCamera> camera;

	float textureScale;
	std::shared_ptr<CPUTexture> cpuTexture;
};

