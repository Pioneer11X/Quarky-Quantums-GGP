#pragma once
#include "SimpleShader.h"
#include "Entity.h"
#include <vector>
#include "Lights.h"

class Renderer
{
	//DirectX related Buffers (for SkyBox)
	ID3D11RasterizerState* rsCullFront;
	ID3D11RasterizerState* rsCullBack;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11BlendState* bsAlphaBlend;

	// Pointer for device and context
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	void DrawEntity(Entity* entity);
	void DrawSkyBox(Entity * skyBox, XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix);

public:
	Renderer(ID3D11Device * deviceIn, ID3D11DeviceContext * contextIn);
	~Renderer();
	void Draw(std::vector<Entity*> entities, Entity* skyBox, XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix, DirectionalLight* dirLights, PointLight* pointLights, SpotLight* spotLights);

};

