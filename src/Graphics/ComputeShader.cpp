#include "Graphics/ComputeShader.h"
#include <d3dcompiler.h>

ComputeShaderNew::ComputeShaderNew(std::string name, std::string fileName, ID3D11Device* device, ID3D11DeviceContext* devicContext)
	: IShader(name, fileName, device, devicContext)
{

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG;
#endif
    // Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
    LPCSTR profile = ( device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ) ? "cs_5_0" : "cs_4_0";
    const D3D_SHADER_MACRO defines[] = 
    {
        "EXAMPLE_DEFINE", "1",
        NULL, NULL
    };
    ID3DBlob* errorBlob = nullptr;
	std::wstring fileNameW = std::wstring(fileName.begin(), fileName.end());
    HRESULT hr = D3DCompileFromFile( fileNameW.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                     "CSMain", profile,
                                     flags, 0, &m_shaderBlob, &errorBlob );
	if ( FAILED(hr) )
    {
        if ( errorBlob )
        {
            OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
            errorBlob->Release();
        }

        if ( m_shaderBlob )
           m_shaderBlob->Release();
        m_shaderBlob = nullptr;
        std::cerr << "Failed to compile compute shader from file: " << fileName << std::endl;
    }    
}

ComputeShaderNew::~ComputeShaderNew() {
}


void ComputeShaderNew::Execute(int x, int y, int z)
{
    if (!IsValid() || !m_shader) {
        std::cerr << "Compute shader is not valid or not set." << std::endl;
        return;
    }
    int i = 0;
    for(const auto& constantBuffer : m_constantBuffers)
    {
        m_deviceContext->CSSetConstantBuffers(i, 1, &constantBuffer);
        i++;
    }
    
    i = 0;
    for(const auto& resourceView : m_resourceViews)
    {
        m_deviceContext->CSSetShaderResources(i, 1, &resourceView->shaderResourceView);
        i++;
    }
    i = 0;
    for(const auto& unorderedAccessView : m_unorderdAccessViews)
    {
        m_deviceContext->CSSetUnorderedAccessViews(i, 1, &unorderedAccessView->unorderedAccessView, &NO_OFFSET);
        i++;
    }
    m_deviceContext->CSSetShader(m_shader, nullptr, 0);
    m_deviceContext->Dispatch((x+15) / 16, (y+15) / 16, (z+15) / 16);

    i = 0;
    for(const auto& resourceView : m_resourceViews)
    {
        m_deviceContext->CSSetShaderResources(i, 1, &NULL_SRV);
        i++;
    }
    i = 0;
    for(const auto& unorderedAccessView : m_unorderdAccessViews)
    {
        m_deviceContext->CSSetUnorderedAccessViews(i, 1, &NULL_UAV, &NO_OFFSET);
        i++;
    }

    m_constantBuffers.clear();
}