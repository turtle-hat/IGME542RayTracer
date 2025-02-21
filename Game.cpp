#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

#include "Helpers.h"
#include "VectorHelpers.h"
#include "Vertex.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
#if defined(DEBUG) | defined(_DEBUG)
	printf("\n------------------------------------------\n");
	printf("\nRun in RELEASE mode for performance boost!\n");
	printf("\n------------------------------------------\n");
#endif

	// Create the CPU texture
	float textureScale = STATIC_TEXTURE_SCALE;
	cpuTexture = std::make_shared<CPUTexture>(
		(unsigned int)(Window::Width() * textureScale),
		(unsigned int)(Window::Height() * textureScale),
		Graphics::Device,
		Graphics::Context);

	// Create the camera
	camera = std::make_shared<FPSCamera>(
		XMFLOAT3(13.0f, 2.0f, -3.0f),	// Position
		1.0f,						// Move speed
		0.002f,						// Look speed
		20.0f,						// Field of view
		Window::AspectRatio(),		// Aspect ratio
		1.0f,						// Near clip
		100.0f,						// Far clip
		CameraProjectionType::Perspective,
		STATIC_TEXTURE_SCALE,
		MOVING_TEXTURE_SCALE
		);

	camera->SetSamplesPerPixel(100);
	camera->SetMaxDepth(10);

	camera->SetDefocusAngle(0.6f);
	camera->SetFocusDist(10.0f);

	shared_ptr<Transform> cameraTransform = camera->GetTransform();
	cameraTransform->SetRotation(0.15f, 74.02f, 0.0f);
	camera->UpdateProjectionMatrix(Window::AspectRatio());

	// Set initial graphics API state
	{
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}



	// Initialize scene parameters
	InitializeWorld();
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{

}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	currentScanline = 0;

	if (cpuTexture)
	{
		float textureScale = camera->GetTextureScale();

		cpuTexture->Resize(
			(unsigned int)(Window::Width() * textureScale),
			(unsigned int)(Window::Height() * textureScale));
	}

	if (camera)
	{
		camera->UpdateProjectionMatrix(Window::AspectRatio());
	}
}

void Game::InitializeWorld()
{
	auto matGround = make_shared<Lambertian>(XMFLOAT3(0.5f, 0.5f, 0.5f));
	world.Add(make_shared<Sphere>(XMFLOAT3(0.0f, -1000.0f, 0.0f), 1000.0f, matGround));

	XMFLOAT3 emptyPoint(4.0f, 0.2f, 0.0f);

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto chooseMat = RandomFloat();
			XMFLOAT3 center(a + 0.9f * RandomFloat(), 0.2f, b + 0.9f * RandomFloat());

			float sqLength;
			XMStoreFloat(&sqLength, XMVector3LengthSq(XMLoadFloat3(&center) - XMLoadFloat3(&emptyPoint)));
			if (sqLength > 0.81f) {
				shared_ptr<Material> sphereMaterial;

				if (chooseMat < 0.8f) {
					// Diffuse
					XMFLOAT3 albedo;
					XMStoreFloat3(&albedo, RandomVector() * RandomVector());
					sphereMaterial = make_shared<Lambertian>(albedo);
					world.Add(make_shared<Sphere>(center, 0.2f, sphereMaterial));
				}
				else if (chooseMat < 0.95f) {
					// Metal
					XMFLOAT3 albedo;
					XMStoreFloat3(&albedo, RandomVector(0.5, 1.0f));
					float fuzz = RandomFloat(0.0f, 0.5f);
					sphereMaterial = make_shared<Metal>(albedo, fuzz);
					world.Add(make_shared<Sphere>(center, 0.2f, sphereMaterial));
				}
				else {
					// Glass
					sphereMaterial = make_shared<Dielectric>(1.5f);
					world.Add(make_shared<Sphere>(center, 0.2f, sphereMaterial));
				}
			}
		}
	}

	auto material1 = make_shared<Dielectric>(1.5f);
	world.Add(make_shared<Sphere>(XMFLOAT3(0.0f, 1.0f, 0.0f), 1.0f, material1));

	auto material2 = make_shared<Lambertian>(XMFLOAT3(0.4f, 0.2f, 0.1f));
	world.Add(make_shared<Sphere>(XMFLOAT3(-4.0f, 1.0f, 0.0f), 1.0f, material2));

	auto material3 = make_shared<Metal>(XMFLOAT3(0.7f, 0.6f, 0.5f), 0.0f);
	world.Add(make_shared<Sphere>(XMFLOAT3(4.0f, 1.0f, 0.0f), 1.0f, material3));
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	camera->Render(world, cpuTexture, deltaTime, totalTime);
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// Draw the texture
	cpuTexture->Draw();

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}



