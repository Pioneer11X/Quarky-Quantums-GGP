#include "Lights.h"

SpotLightWrapper::SpotLightWrapper(SpotLight newLight, float newSpeed, Entity* spotLightEnt) 
{
	myLight = newLight;
	speed = newSpeed;
	yawAngle = 0.0f;
	myEntity = spotLightEnt;
	myEntity->SetRotation(0.0f, 0.0f, XM_PIDIV2);
}

SpotLightWrapper::~SpotLightWrapper()
{
	/*if (myEntity != NULL)
	{
		delete myEntity;
		myEntity = NULL;
	}*/
}

void SpotLightWrapper::UpdateLightPoistion(XMFLOAT3 newPos)
{
	myLight.Position = newPos;
	myEntity->SetTranslation(newPos.x, newPos.y, newPos.z);
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
	XMFLOAT3 currentRotation = myEntity->GetRotation();
	myEntity->SetRotation(currentRotation.x, currentRotation.y, currentRotation.z + deltaAngle);
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