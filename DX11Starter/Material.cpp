#include "Material.h"



Material::Material(SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11ShaderResourceView* srvIn, ID3D11SamplerState* samplerIn)
{
	vertexShader = vShader;
	pixelShader = pShader;
	srv = srvIn;
	sampler = samplerIn;
}


Material::~Material()
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	srv = nullptr;
	sampler = nullptr;
}

SimpleVertexShader * Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Material::GetSRV()
{
	return srv;
}

ID3D11SamplerState * Material::GetSamplerState()
{
	return sampler;
}
