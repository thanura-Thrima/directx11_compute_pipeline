#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <d3d11.h>

#include "ComputeShader.h"
#include "PixelShader.h"
#include "VertexShader.h"

class GraphicsManager {
public:
    ~GraphicsManager();

    ID3D11Device* GetDevice() const { return m_device; }
    ID3D11DeviceContext* GetDeviceContext() const { return m_deviceContext; }
    IDXGISwapChain* GetSwapChain() const { return m_swapChain; }
    ID3D11RenderTargetView* GetBackBuffer() const { return m_backbuffer; }

    bool Initialize(HWND hWnd, int width, int height);

    bool AddComputeShader(const std::string& name, const std::string& fileName);
    bool AddVertexShader(const std::string& name, const std::string& fileName);
    bool AddPixelShader(const std::string& name, const std::string& fileName);

    // Singleton instance retrieval
    static std::shared_ptr<GraphicsManager> GetInstance() {
        static std::shared_ptr<GraphicsManager> instance{ new GraphicsManager() };
        return instance;
    }

private:
    GraphicsManager();

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    ID3D11RenderTargetView* m_backbuffer = nullptr;
    ID3D11DepthStencilState* depthStencilStateWithDepthTest = nullptr;
    ID3D11DepthStencilState* depthStencilStateWithoutDepthTest = nullptr;
    ID3D11RasterizerState* defaultRasterizerState = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;
    std::map<std::string,ComputeShaderNew*> computeShaders;
    std::map<std::string,VertexShaderNew*> vertexShaders;
    std::map<std::string,PixelShaderNew*> pixelShaders;
};