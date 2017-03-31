#pragma once
#include "SimpleShader.h"
#include "Entity.h"
#include <vector>
#include "Lights.h"

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Draw(std::vector<Entity*> entities, ID3D11DeviceContext* context, XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix, DirectionalLight* dirLights, PointLight* pointLights, SpotLight* spotLights);
	void DrawEntity(Entity* entity, ID3D11DeviceContext* context);
};

