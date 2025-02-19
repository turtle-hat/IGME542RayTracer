#include "Camera.h"
#include "Window.h"
#include "Input.h"

using namespace DirectX;


Camera::Camera(
	DirectX::XMFLOAT3 position,
	float fieldOfView,
	float aspectRatio,
	float nearClip,
	float farClip,
	CameraProjectionType projType,
	float textureScaleStatic,
	float textureScaleMoving) :
	fieldOfView(fieldOfView), 
	aspectRatio(aspectRatio),
	nearClip(nearClip),
	farClip(farClip),
	projectionType(projType),
	orthographicWidth(10.0f),
	textureScaleStatic(textureScaleStatic),
	textureScaleMoving(textureScaleMoving)
{
	transform = std::make_shared<Transform>();
	transform->SetPosition(position);

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
	Initialize();
}

// Nothing to really do
Camera::~Camera()
{ }


// Camera's update, which simply updates the view matrix
void Camera::Update(float dt)
{
	// Update the view every frame - could be optimized
	UpdateViewMatrix();
}

// Creates a new view matrix based on current position and orientation
void Camera::UpdateViewMatrix()
{
	// Get the camera's forward vector and position
	XMFLOAT3 forward = transform->GetForward();
	XMFLOAT3 pos = transform->GetPosition();

	// Make the view matrix and save
	XMMATRIX view = XMMatrixLookToLH(
		XMLoadFloat3(&pos),
		XMLoadFloat3(&forward),
		XMVectorSet(0, 1, 0, 0)); // World up axis
	XMStoreFloat4x4(&viewMatrix, view);
}

// Updates the projection matrix
void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	this->aspectRatio = aspectRatio;

	XMMATRIX P;

	// Which type?
	if (projectionType == CameraProjectionType::Perspective)
	{
		P = XMMatrixPerspectiveFovLH(
			fieldOfView,		// Field of View Angle
			aspectRatio,		// Aspect ratio
			nearClip,			// Near clip plane distance
			farClip);			// Far clip plane distance
	}
	else // CameraProjectionType::ORTHOGRAPHIC
	{
		P = XMMatrixOrthographicLH(
			orthographicWidth,	// Projection width (in world units)
			orthographicWidth / aspectRatio,// Projection height (in world units)
			nearClip,			// Near clip plane distance 
			farClip);			// Far clip plane distance
	}

	XMStoreFloat4x4(&projMatrix, P);
	UpdateViewportData();
}

DirectX::XMFLOAT4X4 Camera::GetView() { return viewMatrix; }
DirectX::XMFLOAT4X4 Camera::GetProjection() { return projMatrix; }
std::shared_ptr<Transform> Camera::GetTransform() { return transform; }

float Camera::GetAspectRatio() { return aspectRatio; }

float Camera::GetFieldOfView() { return fieldOfView; }
void Camera::SetFieldOfView(float fov) 
{ 
	fieldOfView = fov; 
	UpdateProjectionMatrix(aspectRatio);
}

float Camera::GetNearClip() { return nearClip; }
void Camera::SetNearClip(float distance) 
{ 
	nearClip = distance;
	UpdateProjectionMatrix(aspectRatio);
}

float Camera::GetFarClip() { return farClip; }
void Camera::SetFarClip(float distance) 
{ 
	farClip = distance;
	UpdateProjectionMatrix(aspectRatio);
}

float Camera::GetOrthographicWidth() { return orthographicWidth; }
void Camera::SetOrthographicWidth(float width)
{
	orthographicWidth = width;
	UpdateProjectionMatrix(aspectRatio);
}

float Camera::GetTextureScale()
{
	return textureScale;
}

float Camera::GetStaticTextureScale()
{
	return textureScaleStatic;
}

void Camera::SetStaticTextureScale(float _scale)
{
	textureScaleStatic = _scale;
}

float Camera::GetMovingTextureScale()
{
	return textureScaleMoving;
}

void Camera::SetMovingTextureScale(float _scale)
{
	textureScaleMoving = _scale;
}

CameraProjectionType Camera::GetProjectionType() { return projectionType; }
void Camera::SetProjectionType(CameraProjectionType type) 
{
	projectionType = type;
	UpdateProjectionMatrix(aspectRatio);
}

void Camera::Initialize()
{
	viewportSize = XMFLOAT2(2.0f * Window::AspectRatio(), 2.0f);
	viewportPixelPercentage = XMFLOAT2(
		(1.0f / (Window::Width() * textureScale)),
		(1.0f / (Window::Height() * textureScale))
	);
	currentScanline = 0;
	UpdateViewportData();
}

void Camera::UpdateViewportData()
{
	viewportSize = XMFLOAT2(2.0f * Window::AspectRatio(), 2.0f);
	viewportPixelPercentage = XMFLOAT2(
		(1.0f / (Window::Width() * textureScale)),
		(1.0f / (Window::Height() * textureScale))
	);

	// Get viewport's U and V vectors, scaling them to our viewport's size
	XMFLOAT3 cameraPosition = transform->GetPosition();
	XMFLOAT3 cameraForward = transform->GetForward();
	XMFLOAT3 cameraRight = transform->GetRight();
	XMFLOAT3 cameraUp = transform->GetUp();

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
			nearClip
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

DirectX::XMFLOAT4 Camera::RayColor(const Ray& _ray, const Hittable& _world)
{
	XMFLOAT4 outColor;

	// Test for world collision
	HitRecord record;

	if (_world.Hit(_ray, Interval(0, infinity), record)) {
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




// ---------------------------------------------
//  FPS CAMERA
// ---------------------------------------------

FPSCamera::FPSCamera(
	DirectX::XMFLOAT3 position,
	float moveSpeed,
	float mouseLookSpeed,
	float fieldOfView,
	float aspectRatio,
	float nearClip,
	float farClip,
	CameraProjectionType projType,
	float textureScaleStatic,
	float textureScaleMoving) :
	Camera(position, fieldOfView, aspectRatio, nearClip, farClip, projType, textureScaleStatic, textureScaleMoving),
	movementSpeed(moveSpeed),
	mouseLookSpeed(mouseLookSpeed)
{

}

float FPSCamera::GetMovementSpeed() { return movementSpeed; }
void FPSCamera::SetMovementSpeed(float speed) { movementSpeed = speed; }

float FPSCamera::GetMouseLookSpeed() { return mouseLookSpeed; }
void FPSCamera::SetMouseLookSpeed(float speed) { mouseLookSpeed = speed; }

bool FPSCamera::Update(float dt)
{
	// Flag, set to true if any input detected
	bool isInputDetected = false;
	// Current speed
	float speed = dt * movementSpeed;

	// Speed up or down as necessary
	if (Input::KeyDown(VK_SHIFT)) { speed *= 5; }
	if (Input::KeyDown(VK_CONTROL)) { speed *= 0.1f; }

	// Movement
	if (Input::KeyDown('W')) {
		transform->MoveRelative(0, 0, speed);
		isInputDetected = true;
	}
	if (Input::KeyDown('S')) { 
		transform->MoveRelative(0, 0, -speed);
		isInputDetected = true;
	}
	if (Input::KeyDown('A')) { 
		transform->MoveRelative(-speed, 0, 0);
		isInputDetected = true;
	}
	if (Input::KeyDown('D')) { 
		transform->MoveRelative(speed, 0, 0);
		isInputDetected = true;
	}
	if (Input::KeyDown('X')) { 
		transform->MoveAbsolute(0, -speed, 0);
		isInputDetected = true;
	}
	if (Input::KeyDown(' ')) { 
		transform->MoveAbsolute(0, speed, 0);
		isInputDetected = true;
	}

	// Handle mouse movement only when button is down
	if (Input::MouseLeftDown())
	{
		// Calculate cursor change
		float xDiff = mouseLookSpeed * Input::GetMouseXDelta();
		float yDiff = mouseLookSpeed * Input::GetMouseYDelta();
		transform->Rotate(yDiff, xDiff, 0);

		// Clamp the X rotation
		XMFLOAT3 rot = transform->GetPitchYawRoll();
		if (rot.x > XM_PIDIV2) rot.x = XM_PIDIV2;
		if (rot.x < -XM_PIDIV2) rot.x = -XM_PIDIV2;
		transform->SetRotation(rot);

		isInputDetected = true;
	}

	// Use base class's update (handles view matrix)
	Camera::Update(dt);

	return isInputDetected;
}

void FPSCamera::Render(const Hittable& _world, std::shared_ptr<CPUTexture> _cpuTexture, float _deltaTime, float _totalTime)
{
	// Check for input and move Camera if needed
	bool isInputDetected = Update(_deltaTime);

	// Resize the texture if it would've been a different size last frame
	if (isInputDetected != wasInputDetectedLastFrame) {
		// Pick correct texture scale
		textureScale = isInputDetected ? textureScaleMoving : textureScaleStatic;
		_cpuTexture->Resize(
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
	unsigned int w = _cpuTexture->GetWidth();
	unsigned int h = _cpuTexture->GetHeight();

	float fWidth = (float)w;
	float fHeight = (float)h;
	float fSinTime = (float)sin(_totalTime) * 0.5f + 0.5f;

	unsigned int y = currentScanline;
	// If camera has detected input, render full screen;
	// If not, set the max row to be rendered this frame so
	// only one scanline is rendered
	unsigned int frameRenderHeight = isInputDetected ? h : y + 1;

	// Get relevant information
	XMVECTOR vecPixelDeltaU = XMLoadFloat3(&pixelDeltaU);
	XMVECTOR vecPixelDeltaV = XMLoadFloat3(&pixelDeltaV);
	XMFLOAT3 cameraPosition = transform->GetPosition();
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
			XMFLOAT4 color = RayColor(ray, _world);

			_cpuTexture->SetColor(x, y, color);
		}
		y++;
	}

	if (!isInputDetected) {
		currentScanline++;
		currentScanline %= h;
	}

	wasInputDetectedLastFrame = isInputDetected;
}
