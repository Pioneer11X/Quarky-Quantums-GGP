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

struct SpotLight
{
	XMFLOAT4 Color;
	XMFLOAT3 Position;
	XMFLOAT3 Direction;
	float DotDist;
	int isOn;
};