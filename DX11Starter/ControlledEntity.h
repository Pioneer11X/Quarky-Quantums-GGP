#pragma once
#include "Entity.h"
#include "InputManager.h"

class ControlledEntity :
	public Entity
{
public:
	ControlledEntity(Mesh* Object, Material* materialInput);
	~ControlledEntity();
	void HandleKeyboardInput(float moveSpeed);
};

