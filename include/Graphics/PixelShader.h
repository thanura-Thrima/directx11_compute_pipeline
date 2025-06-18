#pragma once
#include "IShader.h"

#include <vector>
#include <memory>
#include <map>
#include <string>

#include "Resource.h"
#include <d3d11.h>
#include <d3dcompiler.h>

class PixelShaderNew : public IShader
{

public:
	PixelShaderNew(std::string name, std::string fileName, ID3D11Device* device, ID3D11DeviceContext* devicContext);
	~PixelShaderNew();

	void Execute(int x, int y, int z) override;
    ID3D11PixelShader* GetShader() const { return m_shader;}
    void SetShader(ID3D11PixelShader* shader) { m_shader = shader; }
private:
	std::vector<std::shared_ptr<RenderTarget>> m_resourceViews;
	std::vector<std::shared_ptr<RenderTarget>> m_unorderdAccessViews;
	ID3D11PixelShader* m_shader = nullptr;
};