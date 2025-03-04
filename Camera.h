#pragma once
#include <memory>
#include "Hittable.h"
#include "Transform.h"
#include "CPUTexture.h"

enum class CameraProjectionType
{
	Perspective,
	Orthographic
};

class Camera
{
public:
	// --- FUNCTIONS ---

	Camera(
		DirectX::XMFLOAT3 position,
		float fieldOfView, 
		float aspectRatio, 
		float nearClip = 0.01f, 
		float farClip = 100.0f, 
		CameraProjectionType projType = CameraProjectionType::Perspective,
		float textureScaleStatic = 1.0f,
		float textureScaleMoving = 1.0f
		);

	~Camera();

	// Updating methods
	void Update(float dt);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

	// Getters & Setters
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
	std::shared_ptr<Transform> GetTransform();
	float GetAspectRatio();

	float GetFieldOfView();
	void SetFieldOfView(float fov);
	
	float GetNearClip();
	void SetNearClip(float distance);

	float GetFarClip();
	void SetFarClip(float distance);

	float GetOrthographicWidth();
	void SetOrthographicWidth(float width);

	CameraProjectionType GetProjectionType();
	void SetProjectionType(CameraProjectionType type);

	float GetTextureScale();

	float GetStaticTextureScale();
	void SetStaticTextureScale(float _scale);

	float GetMovingTextureScale();
	void SetMovingTextureScale(float _scale);

	int GetSamplesPerPixel();
	void SetSamplesPerPixel(int _samples);

	int GetMaxDepth();
	void SetMaxDepth(int _depth);

	float GetDefocusAngle();
	void  SetDefocusAngle(float _angle);

	float GetFocusDist();
	void  SetFocusDist(float _dist);



	

protected:
	// --- VARIABLES ---

	// Camera matrices
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	std::shared_ptr<Transform> transform;

	float fieldOfView;
	float aspectRatio;
	float nearClip;
	float farClip;
	float orthographicWidth;

	float defocusAngle;	// Variation angle of rays through each pixel
	float focusDist;	// Distance from camera position to plane of perfect focus

	CameraProjectionType projectionType;




	// Rendering Process Variables
	
	// Current row of pixels being rendered
	unsigned int currentScanline;
	// Whether the camera moved last frame
	bool wasInputDetectedLastFrame = false;



	// Image Variables

	// How much to scale the texture by based on if the camera is moving or not
	float textureScale;
	float textureScaleStatic;
	float textureScaleMoving;
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

	// Defocus disk horizontal and vertical radius
	DirectX::XMFLOAT3 defocusDiskU;
	DirectX::XMFLOAT3 defocusDiskV;

	int samplesPerPixel;
	float pixelSamplesScale;
	int maxDepth;



	// --- FUNCTIONS ---

	// Image Rendering Functions
	void Initialize();
	// Resizes render texture and updates associated variables
	void UpdateViewportData();

	// Drawing helper functions

	// Construct camera ray originating from origin and directed at randomly
	// sampled point around pixel location _i, _j.
	Ray GetRay(unsigned int _i, unsigned int _j, DirectX::XMVECTOR _pixelDeltaU, DirectX::XMVECTOR _pixelDeltaV, DirectX::XMVECTOR _cameraPosition) const;
	// Returns a 2D vector to a random point in X: [-0.5, +0.5], Y: [-0.5, +0.5] unit square
	DirectX::XMFLOAT2 SampleSquare() const;
	// Find the color returned by a given ray
	DirectX::XMVECTOR RayColor(const Ray& _ray, int _depth, const Hittable& _world);
	DirectX::XMFLOAT3 DefocusDiskSample(DirectX::XMVECTOR _center) const;
};


class FPSCamera : public Camera
{
public:

	FPSCamera(
		DirectX::XMFLOAT3 position,
		float moveSpeed,
		float mouseLookSpeed,
		float fieldOfView,
		float aspectRatio,
		float nearClip = 0.01f,
		float farClip = 100.0f,
		CameraProjectionType projType = CameraProjectionType::Perspective,
		float textureScaleStatic = 1.0f,
		float textureScaleMoving = 1.0f);

	float GetMovementSpeed();
	void SetMovementSpeed(float speed);

	float GetMouseLookSpeed();
	void SetMouseLookSpeed(float speed);

	bool Update(float dt);

	// Image Rendering Functions
	void Render(const Hittable& _world, std::shared_ptr<CPUTexture> _cpuTexture, float _deltaTime, float _totalTime);
private:
	float movementSpeed;
	float mouseLookSpeed;
};