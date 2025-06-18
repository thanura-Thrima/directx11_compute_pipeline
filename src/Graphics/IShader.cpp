#include "Graphics/IShader.h"
#include <dxcapi.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>

IShader::IShader(std::string name, std::string fileName, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: m_name(name), m_fileName(fileName), m_device(device), m_deviceContext(deviceContext), m_shaderBlob(nullptr)
{
}

IShader::~IShader() {
	if (m_shaderBlob) {
		m_shaderBlob->Release();
		m_shaderBlob = nullptr;
	}
}

void IShader::generateConstantBuffer(ID3DBlob* blob) {
    /*ID3D11ShaderReflection* reflection = nullptr;

    HRESULT hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection);
    if (FAILED(hr))
    {
        std::cerr << "Failed to reflect shader." << std::endl;
        return;
    }
    D3D11_SHADER_DESC shaderDesc;
    reflection->GetDesc(&shaderDesc);

    for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
    {
        ID3D11ShaderReflectionConstantBuffer* constantBuffer = reflection->GetConstantBufferByIndex(i);
        D3D11_SHADER_BUFFER_DESC bufferDesc;
        constantBuffer->GetDesc(&bufferDesc);

        std::cout << "Constant Buffer: " << bufferDesc.Name << std::endl;

        for (UINT j = 0; j < bufferDesc.Variables; ++j)
        {
            ID3D11ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
            D3D11_SHADER_VARIABLE_DESC variableDesc;
            variable->GetDesc(&variableDesc);

            std::cout << "  Variable: " << variableDesc.Name
                << " Size: " << variableDesc.Size
                << " Offset: " << variableDesc.StartOffset << std::endl;
            m_constantBufferOffsets[variableDesc.Name] = { variableDesc.StartOffset,variableDesc.Size };
        }
    }

    reflection->Release();
    */
}