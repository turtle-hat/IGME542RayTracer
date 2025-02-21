#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

#include "Helpers.h"
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
		XMFLOAT3(0.0f, 0.0f, 0.0f),	// Position
		5.0f,						// Move speed
		0.002f,						// Look speed
		XM_PIDIV4,					// Field of view
		Window::AspectRatio(),		// Aspect ratio
		1.0f,						// Near clip
		100.0f,						// Far clip
		CameraProjectionType::Perspective,
		STATIC_TEXTURE_SCALE,
		MOVING_TEXTURE_SCALE
		);

	camera->SetSamplesPerPixel(100);
	camera->SetMaxDepth(10);

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
	auto matGround	= make_shared<Lambertian>(XMFLOAT3(0.8f, 0.8f, 0.0f));
	auto matCenter	= make_shared<Lambertian>(XMFLOAT3(0.1f, 0.2f, 0.5f));
	auto matLeft	= make_shared<Metal>(XMFLOAT3(0.8f, 0.8f, 0.8f));
	auto matRight	= make_shared<Metal>(XMFLOAT3(0.8f, 0.6f, 0.2f));

	world.Add(make_shared<Sphere>(XMFLOAT3( 0.0f,	-100.5f,	1.0f),	100.0f,	matGround));
	world.Add(make_shared<Sphere>(XMFLOAT3( 0.0f,	 0.0f,		1.2f),	0.5f,	matCenter));
	world.Add(make_shared<Sphere>(XMFLOAT3(-1.0f,	 0.0f,		1.0f),	0.5f,	matLeft));
	world.Add(make_shared<Sphere>(XMFLOAT3( 1.0f,	 0.0f,		1.0f),	0.5f,	matRight));
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



