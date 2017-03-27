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
	if (InputManager::Instance()->isLeftPressed())
	{
		Move(-moveSpeed, 0, 0);
	}

	if (InputManager::Instance()->isRightPressed())
	{
		Move(moveSpeed, 0, 0);
	}

	if (InputManager::Instance()->isUpPressed())
	{
		Move(0, moveSpeed, 0);
	}

	if (InputManager::Instance()->GetKeyDown(VK_RETURN))
	{
		lightIsOn = !lightIsOn;
	}
}
