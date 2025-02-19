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
	textureScale = 0.5f;
	cpuTexture = std::make_shared<CPUTexture>(
		(unsigned int)(Window::Width() * textureScale),
		(unsigned int)(Window::Height() * textureScale),
		Graphics::Device,
		Graphics::Context);
	viewportSize = XMFLOAT2(2.0f * Window::AspectRatio(), 2.0f);
	viewportPixelPercentage = XMFLOAT2(
		(1.0f / (Window::Width() * textureScale)),
		(1.0f / (Window::Height() * textureScale))
	);

	// Create the camera
	camera = std::make_shared<FPSCamera>(
		XMFLOAT3(0.0f, 0.0f, 0.0f),	// Position
		5.0f,						// Move speed
		0.002f,						// Look speed
		XM_PIDIV4,					// Field of view
		Window::AspectRatio(),		// Aspect ratio
		1.0f,						// Near clip
		100.0f,						// Far clip
		CameraProjectionType::Perspective);

	// Set initial graphics API state
	{
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	currentScanline = 0;



	// Initialize scene parameters
	InitializeParameters();
	UpdateViewportData();
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
		cpuTexture->Resize(
			(unsigned int)(Window::Width() * textureScale),
			(unsigned int)(Window::Height() * textureScale));
		viewportSize = XMFLOAT2(2.0f * Window::AspectRatio(), 2.0f);
		viewportPixelPercentage = XMFLOAT2(
			(1.0f / (Window::Width() * textureScale)),
			(1.0f / (Window::Height() * textureScale))
		);
	}

	if (camera)
	{
		camera->UpdateProjectionMatrix(Window::AspectRatio());
		UpdateViewportData();
	}
}

void Game::InitializeParameters()
{
	spheres.push_back(std::make_shared<Sphere>(XMFLOAT3(0.0f, 0.0f, 1.0f), 0.5f));
}

void Game::UpdateViewportData()
{
	// Get viewport's U and V vectors, scaling them to our viewport's size
	std::shared_ptr<Transform> cameraTransform = camera->GetTransform();
	XMFLOAT3 cameraPosition = cameraTransform->GetPosition();
	XMFLOAT3 cameraForward = cameraTransform->GetForward();
	XMFLOAT3 cameraRight = cameraTransform->GetRight();
	XMFLOAT3 cameraUp = cameraTransform->GetUp();
	
	XMStoreFloat3(&viewportU, XMVectorScale(
		XMLoadFloat3(&cameraRight),
		viewportSize.x)
	);
	XMStoreFloat3(&viewportV, XMVectorScale(
		XMVectorScale(XMLoadFloat3(&cameraUp), -1.0f), // Scale by -1 to invert V
		viewportSize.y)
	);

	// Find change in U and V between pixels
	XMStoreFloat3(&pixelDeltaU, XMVectorScale(
		XMLoadFloat3(&viewportU),
		viewportPixelPercentage.x)
	);
	XMStoreFloat3(&pixelDeltaV, XMVectorScale(
		XMLoadFloat3(&viewportV),
		viewportPixelPercentage.y)
	);

	// Find world position of the top-left of the viewport
	XMStoreFloat3(&upperLeftViewportLocation,
		XMLoadFloat3(&cameraPosition) +
		XMVectorScale(
			XMLoadFloat3(&cameraForward),
			camera->GetNearClip()
		) -
		XMVectorScale(XMLoadFloat3(&viewportU), 0.5f) -
		XMVectorScale(XMLoadFloat3(&viewportV), 0.5f)
	);

	// Find world position of the center of the top-left pixel
	XMStoreFloat3(&upperLeftPixelCenter,
		XMLoadFloat3(&upperLeftViewportLocation) +
		XMVectorScale(
			XMLoadFloat3(&pixelDeltaU) + XMLoadFloat3(&pixelDeltaV),
			0.5f
		)
	);
}

DirectX::XMFLOAT4 Game::RayColor(const Ray& _ray, const Hittable& _world)
{
	XMFLOAT4 outColor;

	// Test for world collision
	HitRecord record;

	if (_world.Hit(_ray, 0, infinity, record)) {
		// Calculate color
		XMFLOAT3 color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		XMStoreFloat3(&color,
			XMVectorScale(XMLoadFloat3(&record.normal) + XMLoadFloat3(&color), 0.5f)
		);

		return XMFLOAT4(color.x, color.y, color.z, 1.0f);
	}

	// Sky color

	// Unpack XMFLOAT3s
	XMVECTOR vecRayOrigin = XMLoadFloat3(&_ray.Origin);
	XMVECTOR vecRayDirection = XMLoadFloat3(&_ray.Direction);

	// Normalize ray's direction and store
	XMVECTOR vecUnitDirection = XMVector3Normalize(vecRayDirection);

	XMFLOAT3 unitDirection;
	XMStoreFloat3(&unitDirection, vecUnitDirection);

	// Find y component of ray
	float a = 0.5f * (unitDirection.y + 1.0f);

	XMFLOAT4 color1(0.5f, 0.7f, 1.0f, 1.0f);
	XMFLOAT4 color2(1.0f, 1.0f, 1.0f, 1.0f);

	// Calculate interpolated color
	XMStoreFloat4(&outColor, XMVectorLerp(XMLoadFloat4(&color1), XMLoadFloat4(&color2), a));

	return outColor;
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	// Check for input and move Camera if needed
	bool isInputDetected = camera->Update(deltaTime);

	// Resize the texture if it would've been a different size last frame
	if (isInputDetected != wasInputDetectedLastFrame) {
		// Pick correct texture scale
		textureScale = isInputDetected ? MOVING_TEXTURE_SCALE : STATIONARY_TEXTURE_SCALE;
		cpuTexture->Resize(
			(unsigned int)(Window::Width() * textureScale),
			(unsigned int)(Window::Height() * textureScale));
		viewportPixelPercentage = XMFLOAT2(
			(1.0f / (Window::Width() * textureScale)),
			(1.0f / (Window::Height() * textureScale))
		);
		// Reset scanline
		currentScanline = 0;
		UpdateViewportData();
	}
	// If it hasn't already been, update viewport data with movement
	else if (isInputDetected) {
		UpdateViewportData();
	}

	

	// Change the color of the texture
	unsigned int w = cpuTexture->GetWidth();
	unsigned int h = cpuTexture->GetHeight();

	float fWidth = (float)w;
	float fHeight = (float)h;
	float fSinTime = (float)sin(totalTime) * 0.5f + 0.5f;
	
	unsigned int y = currentScanline;
	// If camera has detected input, render full screen;
	// If not, set the max row to be rendered this frame so
	// only one scanline is rendered
	unsigned int frameRenderHeight = isInputDetected ? h : y + 1;

	// Get relevant information
	XMVECTOR vecPixelDeltaU = XMLoadFloat3(&pixelDeltaU);
	XMVECTOR vecPixelDeltaV = XMLoadFloat3(&pixelDeltaV);
	std::shared_ptr<Transform> cameraTransform = camera->GetTransform();
	XMFLOAT3 cameraPosition = cameraTransform->GetPosition();
	XMVECTOR vecCameraPosition = XMLoadFloat3(&cameraPosition);

	while (y < frameRenderHeight)
	{
		for (unsigned int x = 0; x < w; x++)
		{
			// Create ray
			Ray ray = {};
			ray.Origin = cameraPosition;

			// Find center of this pixel
			XMVECTOR pixelCenter = XMLoadFloat3(&upperLeftPixelCenter) +
				XMVectorScale(vecPixelDeltaU, (float)x) +
				XMVectorScale(vecPixelDeltaV, (float)y);

			// Get the direction of the ray through the center of this pixel
			XMVECTOR vecRayDirection = pixelCenter - vecCameraPosition;
			XMStoreFloat3(&ray.Direction, vecRayDirection);

			//XMFLOAT4 color = XMFLOAT4(x / fWidth, y / fHeight, fSinTime, 1);
			XMFLOAT4 color = RayColor(ray);

			cpuTexture->SetColor(x, y, color);
		}
		y++;
	}

	if (!isInputDetected) {
		currentScanline++;
		currentScanline %= h;
	}

	wasInputDetectedLastFrame = isInputDetected;
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



