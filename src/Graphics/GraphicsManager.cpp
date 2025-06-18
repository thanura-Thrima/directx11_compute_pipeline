#include "Graphics/GraphicsManager.h"
#include <iostream>

GraphicsManager::~GraphicsManager() {
    // Cleanup resources if necessary
}

GraphicsManager::GraphicsManager() {
    // Initialize graphics context, load default resources, etc.
        UINT createDeviceFlags = 0;
#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL featureLevelCreated;
    HRESULT hr = D3D11CreateDevice(
        nullptr, // Adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr, // Software
        createDeviceFlags, // Flags
        featureLevels, // Feature levels
        ARRAYSIZE(featureLevels), // Feature levels count
        D3D11_SDK_VERSION,
        &m_device,
        &featureLevelCreated, // Feature level
        &m_deviceContext
    );
}


bool GraphicsManager::Initialize(HWND hWnd, int width, int height)
{
    if (!m_device || !m_deviceContext) {
        std::cerr << "Failed to create Direct3D device and context." << std::endl;
        return false;
    }

    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = 0;
    // create a device, device context and swap chain

    IDXGIFactory* pFactory = nullptr;
    IDXGIDevice* pDXGIDevice = nullptr;
    IDXGIAdapter* pAdapter = nullptr;
// Get the DXGI factory from the device
    m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
    pDXGIDevice->GetAdapter(&pAdapter);
    pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

    auto hr = pFactory->CreateSwapChain(m_device, &scd, &m_swapChain);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create Direct3D device and swap chain: " << std::hex << hr << std::endl;
        return false; // Return false if initialization failed
    }

    if (pDXGIDevice) pDXGIDevice->Release();
    if (pAdapter) pAdapter->Release();
    if (pFactory) pFactory->Release();

    ID3D11Texture2D* pBackBuffer = nullptr;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (pBackBuffer == nullptr)
    {
        std::cerr << "Could not obtain backbuffer from swapchain";
        return false; // Return false if back buffer creation failed
    }
    m_device->CreateRenderTargetView(pBackBuffer, nullptr, &m_backbuffer);
    pBackBuffer->Release();

        // set the viewport (note: since this does not change, it is sufficient to do this once)
    D3D11_VIEWPORT viewport = { };
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;

    m_deviceContext->RSSetViewports(1, &viewport);

    return true; // Return true if initialization succeeded
}

bool GraphicsManager::AddComputeShader(const std::string& name, const std::string& fileName) {
    if (computeShaders.find(name) != computeShaders.end()) {
        std::cerr << "Compute shader with name " << name << " already exists." << std::endl;
        return false;
    }
    ComputeShaderNew* shader = new ComputeShaderNew(name, fileName, m_device, m_deviceContext);
    if (!shader->IsValid()) {
        delete shader;
        return false;
    }
    computeShaders[name] = shader;
    return true;
}

bool GraphicsManager::AddVertexShader(const std::string& name, const std::string& fileName) {
    if (vertexShaders.find(name) != vertexShaders.end()) {
        std::cerr << "Vertex shader with name " << name << " already exists." << std::endl;
        return false;
    }
    VertexShaderNew* shader = new VertexShaderNew(name, fileName, m_device, m_deviceContext);
    if (!shader->IsValid()) {
        delete shader;
        return false;
    }
    vertexShaders[name] = shader;
    return true;
}

bool GraphicsManager::AddPixelShader(const std::string& name, const std::string& fileName) {
    if (pixelShaders.find(name) != pixelShaders.end()) {
        std::cerr << "Pixel shader with name " << name << " already exists." << std::endl;
        return false;
    }
    PixelShaderNew* shader = new PixelShaderNew(name, fileName, m_device, m_deviceContext);
    if (!shader->IsValid()) {
        delete shader;
        return false;
    }
    pixelShaders[name] = shader;
    return true;
}