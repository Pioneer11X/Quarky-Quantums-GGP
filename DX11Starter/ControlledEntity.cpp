#include "ControlledEntity.h"



ControlledEntity::ControlledEntity(Mesh * Object, Material* materialInput, float _posX, float _posY, float _posZ, b2World* world, bool isDynamic, float _sizeX, float _sizeY) : Entity(Object, materialInput, _posX, _posY, _posZ, world, isDynamic, _sizeX, _sizeY)
{
	lightIsOn = true;
}


ControlledEntity::~ControlledEntity()
{
}

void ControlledEntity::HandleKeyboardInput(float moveSpeed)
{

	b2Vec2 tempImpulse = b2Vec2(0.0f, 0.0f);
	//b2Vec2 zeroImpulse = b2Vec2(0.0f, 0.0f);
	if (InputManager::Instance()->GetKeyHolding(KeyPressed::LEFT))
	{
		tempImpulse = b2Vec2(-1 * moveSpeed, 0);
	}

	if (InputManager::Instance()->GetKeyHolding(KeyPressed::RIGHT))
	{
		tempImpulse = b2Vec2(moveSpeed, 0);
	}

	if (InputManager::Instance()->GetKeyDown(KeyPressed::UP))
	{
		tempImpulse = b2Vec2(0, moveSpeed * 200);
	}
	// TODO: Need to add Raycast or something to check for the player if they are actually on the ground
	if ( !(tempImpulse.x == 0 && tempImpulse.y == 0) ) {
		this->GetPhysicsBody()->ApplyLinearImpulseToCenter(tempImpulse, true);
	}

	if (InputManager::Instance()->GetKeyDown(KeyPressed::FORWARD))
	{
		lightIsOn = !lightIsOn;
	}
}
