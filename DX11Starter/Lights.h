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
	float AngleRads;
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
	SpotLightWrapper(SpotLight newLight, float newSpeed, Entity* spotLightEnt);
	~SpotLightWrapper();
	void HandleKeyboardInputs(float deltaTime);
	void UpdateLightPoistion(XMFLOAT3 newPos);
	inline SpotLight& GetSpotLight() { return myLight; }
	inline Entity* GetEntity() { return myEntity; }
private:
	SpotLight myLight;
	float speed;
	float yawAngle;
	void SetRotation(float angle);
	Entity* myEntity;
};
