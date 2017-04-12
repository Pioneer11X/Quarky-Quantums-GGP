#pragma once

#include "Entity.h"
#include "InputManager.h"
#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight
{
	XMFLOAT4 AmbientColor;
	XMFLOAT4 DiffuseColor;
	XMFLOAT3 Direction;
	int isOn;
};

struct PointLight
{
	XMFLOAT4 Color;
	XMFLOAT3 Position;
	int isOn;
};

// The order for these members matters
// If two float3 values are by eachother then the data structure is unaligned.
struct SpotLight
{
	XMFLOAT4 AmbientColor;
	XMFLOAT4 DiffuseColor;
	XMFLOAT3 Direction;
	float DotDist;
	XMFLOAT3 Position;
	int isOn;
	float SpotIntensity;
	float ConstAtten;
	float LinearAtten;
	float ExpoAtten;
};

class SpotLightWrapper
{
public:
	SpotLightWrapper(SpotLight newLight,float newSpeed);
	~SpotLightWrapper();
	void HandleKeyboardInputs(float deltaTime);
	void UpdateLightPoistion(XMFLOAT3 newPos);
	inline SpotLight& GetSpotLight() { return myLight; }
	/*void CalculateWorldMatrix();
	XMFLOAT4X4& GetWorldMatrix();
	Mesh* GetMesh();
	void SetAlpha(float val);

	// Get Alpha
	float& GetAlpha();

	// Set WVP
	void PrepareMaterial(XMFLOAT4X4 camViewMatrix, XMFLOAT4X4 camProjectionMatrix);

	// Get material
	Material* GetMaterial();*/
private:
	SpotLight myLight;
	float speed;
	float yawAngle;
	void SetRotation(float angle);
	//Entity* myEntity;
};
