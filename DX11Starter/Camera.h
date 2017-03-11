#pragma once
#include <DirectXMath.h>
#include <math.h>
#include "DXCore.h"
#include "Vertex.h"
#include "InputManager.h"
#include "Game.h"
// For the DirectX Math library
using namespace DirectX;

class Camera
{
private:
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 up;
	float rotationX;
	float rotationY;

	void HandleKeyboardInput(float moveSpeed);

	// Bool to check if values have changed since last frame
	bool isDirty;

public:
	Camera();
	~Camera();
	XMFLOAT4X4& GetViewMatrix();
	XMFLOAT4X4& GetProjectionMatrix();
	void Update(float deltaTime, float totalTime);
	void SetRotationX(float rotVal);
	void SetRotationY(float rotVal);
	void SetProjectionMatrix();
	void MoveAlongDirection(float val);
	void MoveSideways(float val);
	void MoveVertical(float val);
	XMFLOAT3& GetPosition();
};

