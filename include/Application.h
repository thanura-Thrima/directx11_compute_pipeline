#pragma once

#include <windows.h>
#include <windowsx.h>

#include <iostream>
#include <array>
#include <vector>

#include <d3d11.h>

// Direct3D libraries
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


#include "resource.h"
#include "util/timer.h"
#include "geometry.h"

#include "Graphics/ComputeShader.h"
#include "Graphics/VertexShader.h"
#include "Graphics/PixelShader.h"

class Application
{
public:
	Application(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	virtual ~Application() = default;

	// Initialize the application
	bool Initialize(HWND hWnd);

	void InitRenderData();

	void Run();

	// Update the application state
	void Update(float deltaTime);

	// Render the application content
	void Render();

	void CleanupRenderData();

	// Handle input events
	void HandleInput();

	// Cleanup resources before exiting
	virtual void Cleanup();
private:
	const static int WIDTH = 1024;
	const static int HEIGHT = 768;
	const static size_t NUM_RENDERTARGETS = 3;

	// timer for retrieving delta time between frames
	Timer timer;

	// DirectX resources: swapchain, device, and device context
	IDXGISwapChain* swapchain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	// backbuffer obtained from the swapchain
	ID3D11RenderTargetView* backbuffer;

	// shaders
	//ShaderProgram modelShader;
	//ShaderProgram quadCompositeShader;
	//ComputeShader thresholdDownsampleShader;
	//ComputeShader blurShader;

	// render targets and depth-stencil target
	RenderTarget renderTargets[NUM_RENDERTARGETS];
	DepthStencilTarget depthStencilTarget;

	// depth-stencil states
	ID3D11DepthStencilState* depthStencilStateWithDepthTest;
	ID3D11DepthStencilState* depthStencilStateWithoutDepthTest;

	// default texture sampler
	ID3D11SamplerState* defaultSamplerState;

	// default rasterizer state
	ID3D11RasterizerState* defaultRasterizerState;

	// meshes
	Mesh objModelMesh;
	Mesh screenAlignedQuadMesh;

	// model, view, and projection transform
	Transformations transforms;
	ID3D11Buffer* transformConstantBuffer;

	// material and light source
	Material material;
	ID3D11Buffer* materialConstantBuffer;

	LightSource lightSource;
	ID3D11Buffer* lightSourceConstantBuffer;

	// parameters for compute shaders
	ID3D11Buffer* thresholdConstantBuffer;

	BlurParams blurParams;
	ID3D11Buffer* blurConstantBuffer;

	ID3D11Buffer* compositionConstantBuffer;

	ComputeShaderNew* blurShader_new = nullptr;
	ComputeShaderNew* thresholdDownsampleShader_new = nullptr;

	VertexShaderNew* vsPhongShader = nullptr;
	VertexShaderNew* vsQuadCompositeShader = nullptr;

	PixelShaderNew* psPhongShader = nullptr;
	PixelShaderNew* psQuadCompositeShader = nullptr;
};

