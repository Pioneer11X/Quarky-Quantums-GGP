#pragma once

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
	XMFLOAT4 Color;
	XMFLOAT3 Direction;
	float DotDist;
	XMFLOAT3 Position;
	int isOn;
};