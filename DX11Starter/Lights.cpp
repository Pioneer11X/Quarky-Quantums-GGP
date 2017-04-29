#include "Lights.h"
#include <vector>
#include "Game.h"

float CalcDistance(XMFLOAT3 l, XMFLOAT3 r);

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
	newPos.x += myLight.Direction.x * (myEntity->GetScale().y/2);
	newPos.y += (myLight.Direction.y * (myEntity->GetScale().y / 2));
	myEntity->SetTranslation(newPos.x, newPos.y, newPos.z);
}

void SpotLightWrapper::RayCastCheck()
{

	// ==== My Implementation ====

	// 1. Get all the Entities.

	std::vector<Entity *> entities = Game::Instance()->GetEntities();

	// 1.5 Check if the entities are triggerable.
	// For optimisation, we might want to store a seperate list of entities in the Game Class which are updated only when the entities are added. This saves us a loop.
	for (std::vector<Entity *>::iterator it = entities.begin(); it != entities.end();)
	{
			if (!(*it)->CanBeTrigerred())
			{
				it = entities.erase(it);
			}
			else
			{
				// 2. Check if the entities are within range of the Spotlight.
				// This needs to be in Update. Done in every Frame.
				if (CalcDistance((*it)->GetPosition(), myEntity->GetPosition()) > Range) {
					it = entities.erase(it);
				}
				else {
					++it;
				}
			}
	}

	// 3. Get the 4 Points ( Square ) on the same Z Plane as the spotlight.

	// ==== End of My Implementation ====

	// ==== Box2D Check ====



	// ==== End of Box2D Check ====

}

void SpotLightWrapper::HandleKeyboardInputs(float deltaTime)
{
	// Rotate Counter Clockwise
	if (InputManager::Instance()->GetKeyHolding(KeyPressed::LEFTARROW))
	{
		SetRotation(speed * deltaTime);
	}

	// Rotate Clockwise
	if (InputManager::Instance()->GetKeyHolding(KeyPressed::RIGHTARROW))
	{
		SetRotation(-speed * deltaTime);
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

float CalcDistance( XMFLOAT3 l, XMFLOAT3 r ) {

	XMVECTOR vector1 = XMLoadFloat3(&l);
	XMVECTOR vector2 = XMLoadFloat3(&r);
	XMVECTOR vectorSub = XMVectorSubtract(vector1, vector2);
	XMVECTOR length = XMVector3Length(vectorSub);

	float distance = 0.0f;
	XMStoreFloat(&distance, length);
	return distance;

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