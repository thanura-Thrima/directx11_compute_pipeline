#include "Graphics/PixelShader.h"
#include <d3dcompiler.h>

PixelShaderNew::PixelShaderNew(std::string name, std::string fileName, ID3D11Device* device, ID3D11DeviceContext* devicContext)
	: IShader(name, fileName, device, devicContext)
{

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG;
#endif
    // Prefer higher PS shader profile when possible as PS 5.0 provides better performance on 11-class hardware.
    LPCSTR profile = ( device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ) ? "ps_5_0" : "ps_4_0";
    const D3D_SHADER_MACRO defines[] = 
    {
        "EXAMPLE_DEFINE", "1",
        NULL, NULL
    };
    ID3DBlob* errorBlob = nullptr;
	std::wstring fileNameW = std::wstring(fileName.begin(), fileName.end());
    HRESULT hr = D3DCompileFromFile( fileNameW.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                     "PSMain", profile,
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
        std::cerr << "Failed to compile pixel shader from file: " << fileName << std::endl;
    }
}

PixelShaderNew::~PixelShaderNew() {
}


void PixelShaderNew::Execute(int x, int y, int z)
{

}