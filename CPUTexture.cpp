#include "CPUTexture.h"
#include <d3dcompiler.h>

using namespace DirectX;

// -----------------------------------
// Creates a new CPUTexture object
// of the given size
// 
// width - Width of pixel grid
// height - Height of pixel grid
// device - D3D11 Device object
// context - D3D11 context object
// -----------------------------------
CPUTexture::CPUTexture(
	unsigned int width,
	unsigned int height,
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	:
	width(width),
	height(height),
	device(device),
	context(context),
	pixelColors{}
{
	// Perform a resize to create the initial data
	Resize(width, height);
	
	// Compile necessary shaders into bytecode
	ID3DBlob* vsBlob;
	ID3DBlob* psBlob;
	D3DCompile(copyVSCode, strlen(copyVSCode), 0, 0, 0,	"main",	"vs_5_0", 0, 0, &vsBlob, 0);
	D3DCompile(copyPSCode, strlen(copyPSCode), 0, 0, 0, "main", "ps_5_0", 0, 0, &psBlob, 0);

	// Create D3D shaders from bytecode
	device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), 0, copyVS.GetAddressOf());
	device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), 0, copyPS.GetAddressOf());

	// Create a sampler state for copy
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());
}

// -----------------------------------
// Cleans up the pixel grid
// -----------------------------------
CPUTexture::~CPUTexture()
{
	delete[] pixelColors;
}

// -----------------------------------
// Resizes the pixel grid and associated
// GPU resource (texture)
// 
// width - New width
// height - New height
// -----------------------------------
void CPUTexture::Resize(unsigned int width, unsigned int height)
{
	// Grab new data
	this->width = width;
	this->height = height;

	// Reset resources
	copyTexture.Reset();
	copyTextureSRV.Reset();
	if (pixelColors) { delete[] pixelColors; }

	// Create the dynamic texture for uploading
	D3D11_TEXTURE2D_DESC copyDesc = {};
	copyDesc.ArraySize = 1;
	copyDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	copyDesc.CPUAccessFlags = 0;
	copyDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // Need to be able to copy to GPU quickly, so this must match our CPU data type
	copyDesc.Height = height;
	copyDesc.MipLevels = 1;
	copyDesc.MiscFlags = 0;
	copyDesc.SampleDesc.Count = 1;
	copyDesc.SampleDesc.Quality = 0;
	copyDesc.Usage = D3D11_USAGE_DEFAULT;
	copyDesc.Width = width;
	device->CreateTexture2D(&copyDesc, 0, copyTexture.GetAddressOf());

	// Make a default SRV
	device->CreateShaderResourceView(copyTexture.Get(), 0, copyTextureSRV.GetAddressOf());

	// Re-created pixel grid
	pixelColors = new XMFLOAT4[width * height];
	ClearFast();
}

// -----------------------------------
// Copies the pixel grid to the GPU
// and draws it to the screen
// -----------------------------------
void CPUTexture::Draw()
{
	// Copy the pixels to the actual texture
	D3D11_MAPPED_SUBRESOURCE map = {};
	/*context->Map(copyTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, pixelColors, sizeof(XMFLOAT4) * width * height);
	context->Unmap(copyTexture.Get(), 0);*/
	context->UpdateSubresource(
		copyTexture.Get(),
		0,
		0,
		pixelColors,
		width * sizeof(XMFLOAT4),
		1
	);

	// Perform a quick render to copy from our float texture to the back buffer
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(copyVS.Get(), 0, 0);
	context->PSSetShader(copyPS.Get(), 0, 0);
	context->PSSetShaderResources(0, 1, copyTextureSRV.GetAddressOf());
	context->PSSetSamplers(0, 1, sampler.GetAddressOf());
	context->Draw(3, 0);
	
}

// -----------------------------------
// Gets the width of the pixel grid
// -----------------------------------
unsigned int CPUTexture::GetWidth() { return width; }

// -----------------------------------
// Gets the height of the pixel grid
// -----------------------------------
unsigned int CPUTexture::GetHeight() { return height; }

// -----------------------------------
// Clears the pixel grid to a specified color
// 
// color - The color to replicate for all pixels
// -----------------------------------
void CPUTexture::Clear(DirectX::XMFLOAT4 color)
{
	unsigned int pixelCount = width * height;

	for (unsigned int i = 0; i < pixelCount; i++)
		memcpy(&pixelColors[i], &color, sizeof(XMFLOAT4));
}

// -----------------------------------
// Clears the pixel grid to black
// -----------------------------------
void CPUTexture::ClearFast()
{
	memset(pixelColors, 0, sizeof(XMFLOAT4) * width * height);
}

// -----------------------------------
// Sets the specified pixel to a color
// 
// x - Pixel grid x location
// y - Pixel grid y location
// color - Color to place in grid
// -----------------------------------
void CPUTexture::SetColor(unsigned int x, unsigned int y, DirectX::XMFLOAT4 color)
{
	memcpy(&pixelColors[PixelIndex(x, y)], &color, sizeof(XMFLOAT4));
}

// -----------------------------------
// Adds the specified color to the
// color already at the given position
// 
// x - Pixel grid x location
// y - Pixel grid y location
// color - Color to add
// -----------------------------------
void CPUTexture::AddColor(unsigned int x, unsigned int y, DirectX::XMFLOAT4 color)
{
	unsigned int index = PixelIndex(x, y);
	XMVECTOR p = XMLoadFloat4(&pixelColors[index]);
	XMVECTOR c = XMLoadFloat4(&color);
	XMStoreFloat4(&pixelColors[index], XMVectorAdd(p, c));
}

// -----------------------------------
// Calculates a 1D index from [x, y] notation
// 
// x - Pixel grid x location
// y - Pixel grid y location
// -----------------------------------
unsigned int CPUTexture::PixelIndex(unsigned int x, unsigned int y)
{
	return y * width + x;
}
