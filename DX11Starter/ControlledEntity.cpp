#include "ControlledEntity.h"



ControlledEntity::ControlledEntity(Mesh* Object, Material* materialInput) : Entity (Object, materialInput)
{
	lightIsOn = true;
}


ControlledEntity::~ControlledEntity()
{
}

void ControlledEntity::HandleKeyboardInput(float moveSpeed)
{
	if (InputManager::Instance()->GetKeyHolding(KeyPressed::LEFT))
	{
		Move(-moveSpeed, 0, 0);
	}

	if (InputManager::Instance()->GetKeyHolding(KeyPressed::RIGHT))
	{
		Move(moveSpeed, 0, 0);
	}

	if (InputManager::Instance()->GetKeyDown(KeyPressed::UP))
	{

		Move(0, moveSpeed * 100, 0);

	}

	if (InputManager::Instance()->GetKeyDown(KeyPressed::FORWARD))
	{
		lightIsOn = !lightIsOn;
	}
}
