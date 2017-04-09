#include "Lights.h"

SpotLightWrapper::SpotLightWrapper(SpotLight newLight, float newSpeed) 
{
	myLight = newLight;
	speed = newSpeed;
	yawAngle = 0.0f;
	//myEntity = newEntity;
}

SpotLightWrapper::~SpotLightWrapper()
{
	/*if (myEntity != NULL)
	{
		delete myEntity;
		myEntity = NULL;
	}*/
}

void SpotLightWrapper::HandleKeyboardInputs(float deltaTime)
{
	// Rotate Counter Clockwise
	if (InputManager::Instance()->GetKeyHolding(KeyPressed::LEFTARROW))
	{
		SetRotation(-speed * deltaTime);
	}

	// Rotate Clockwise
	if (InputManager::Instance()->GetKeyHolding(KeyPressed::RIGHTARROW))
	{
		SetRotation(speed * deltaTime);
	}
}

void SpotLightWrapper::SetRotation(float deltaAngle)
{
	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, deltaAngle);
	XMVECTOR rotatedVector = XMVector3Rotate(XMLoadFloat3(&myLight.Direction), rotationQuat);
	rotatedVector = XMQuaternionNormalize(rotatedVector);

	XMStoreFloat3(&myLight.Direction, rotatedVector);
}
/*{
	// Rotate the entity
	//myEntity->SetRotation(x, y, z, w);

	// Rotate the direction of the spotLight

}
void CalculateWorldMatrix();
XMFLOAT4X4& GetWorldMatrix();
Mesh* GetMesh();
void SetAlpha(float val);

// Get Alpha
float& GetAlpha();

// Set WVP
void PrepareMaterial(XMFLOAT4X4 camViewMatrix, XMFLOAT4X4 camProjectionMatrix);

// Get material
Material* GetMaterial();*/