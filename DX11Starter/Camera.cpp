#include "Camera.h"



void Camera::Update(float deltaTime, float totalTime)
{
	//HandleKeyboardInput(deltaTime);

	if (isDirty)
	{
		XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(rotationX, rotationY, 0.0f);
		XMVECTOR rotatedVector = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationQuat);
		rotatedVector = XMQuaternionNormalize(rotatedVector);

		XMStoreFloat3(&direction, rotatedVector);

		XMStoreFloat3(&up, XMQuaternionNormalize(XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationQuat)));

		XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMMatrixLookToLH(XMLoadFloat3(&position), rotatedVector, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))));

		isDirty = false;
	}
}

void Camera::SetRotationX(float rotVal)
{
	rotationX += rotVal/1024;

	isDirty = true;
}

void Camera::SetRotationY(float rotVal)
{
	rotationY += rotVal / 1024;

	isDirty = true;
}

void Camera::SetProjectionMatrix()
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//   the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)Game::Instance()->GetScreenWidth() / Game::Instance()->GetScreenHeight(),		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

void Camera::MoveAlongDirection(float val)
{
	position.x += val * direction.x;
	position.y += val * direction.y;
	position.z += val * direction.z;

	isDirty = true;
}

void Camera::MoveSideways(float val)
{
	XMFLOAT3 leftSide;
	XMStoreFloat3(&leftSide, XMVector3Cross(XMLoadFloat3(&direction), XMLoadFloat3(&up)));

	position.x += val * leftSide.x;
	position.y += val * leftSide.y;
	position.z += val * leftSide.z;

	isDirty = true;
}

void Camera::MoveVertical(float val)
{
	position.x += val * up.x;
	position.y += val * up.y;
	position.z += val * up.z;

	isDirty = true;
}

XMFLOAT3 & Camera::GetPosition()
{
	// TODO: insert return statement here
	return position;
}

void Camera::LerpToPosition(XMFLOAT3& lookAt, float deltaTime)
{
	XMVECTOR here = XMLoadFloat3(&position);
	XMVECTOR target = XMLoadFloat3(&lookAt);
	XMVECTOR result = XMVectorLerp(here, target, 2 * deltaTime);
	XMStoreFloat3(&position, result);
	position.z = lookAt.z - 20;

	isDirty = true;
}

Camera::Camera()
{
	isDirty = true;

	rotationX = 0.0f;
	rotationY = 0.0f;
	position = { 0.0f, 0.0f, -5.0f };
	direction = { 0.0f, 0.0f, -1.0f };
	up = { 0.0f, 1.0f, 0.0f };

	SetProjectionMatrix();
}


Camera::~Camera()
{
}

XMFLOAT4X4 & Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 & Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}
