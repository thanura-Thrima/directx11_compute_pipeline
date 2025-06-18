#pragma once

#include <concepts>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>

#include <d3d11.h>
#include <d3d11shader.h>
#include <dxcapi.h>

constexpr ID3D11RenderTargetView* NULL_RT = nullptr;
constexpr ID3D11ShaderResourceView* NULL_SRV = nullptr;
constexpr ID3D11UnorderedAccessView* NULL_UAV = nullptr;
constexpr UINT NO_OFFSET = -1;


template <typename T>
concept ShaderObject = std::is_base_of<ID3D11DeviceChild, T>::value;

class IShader
{
public:
	IShader(std::string name, std::string fileName, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~IShader() = 0;
	virtual void Execute(int x, int y, int z) = 0;
	void generateConstantBuffer(ID3DBlob* blob);
	ID3DBlob* GetShaderBlob() const { return m_shaderBlob; }
protected:
	std::string m_name;
	std::string m_fileName;
	ID3DBlob* m_shaderBlob{ nullptr };
	std::map<std::string, std::pair<int, int>> m_constantBufferOffsets;
	ID3D11Device* m_device{ nullptr };
	ID3D11DeviceContext* m_deviceContext{ nullptr };

};