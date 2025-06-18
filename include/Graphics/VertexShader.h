#pragma once
#include "IShader.h"

#include <vector>
#include <memory>
#include <map>
#include <string>

#include "Resource.h"
#include <d3d11.h>
#include <d3dcompiler.h>

class VertexShaderNew : public IShader
{

public:
	VertexShaderNew(std::string name, std::string fileName, ID3D11Device* device, ID3D11DeviceContext* devicContext);
	~VertexShaderNew();

	void Execute(int x, int y, int z) override;
    ID3D11VertexShader* GetShader() const { return m_shader; }
    void SetShader(ID3D11VertexShader* shader) { m_shader = shader; }
private:
	std::vector<std::shared_ptr<RenderTarget>> m_resourceViews;
	std::vector<std::shared_ptr<RenderTarget>> m_unorderdAccessViews;
	ID3D11VertexShader* m_shader = nullptr;
};