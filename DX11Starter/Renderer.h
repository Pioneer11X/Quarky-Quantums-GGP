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
	ID3D11RasterizerState* rsCullNone;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11BlendState* bsAlphaBlend;

	// Shadow stuff
	int shadowMapSize;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11SamplerState* shadowSampler;
	ID3D11RasterizerState* shadowRasterizer;
	SimpleVertexShader* shadowVS;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;

	// Needed for shadow stuff
	ID3D11RenderTargetView* backbufferRTV;
	ID3D11DepthStencilView* depthStencilView;

	// Volumteric Stuff
	SimplePixelShader* beamPS;

	// Pointer for device and context
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	void DrawEntity(Entity* entity);
	void DrawSkyBox(Entity * skyBox, XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix);
	void DrawBeam(XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix);
	void RenderShadowMap(std::vector<Entity*> entities);

public:
	Renderer(ID3D11Device * deviceIn, ID3D11DeviceContext * contextIn);
	~Renderer();
	void Draw(std::vector<Entity*> entities, Entity* skyBox, XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix, DirectionalLight* dirLights, PointLight* pointLights, SpotLight* spotLights);
	void UpdateBuffers(ID3D11RenderTargetView* backBuf, ID3D11DepthStencilView* dsv) { backbufferRTV = backBuf; depthStencilView = dsv; }

};

