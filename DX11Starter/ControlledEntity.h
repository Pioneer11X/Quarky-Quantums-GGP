#pragma once
#include "Entity.h"
#include "InputManager.h"

class ControlledEntity :
	public Entity
{
public:
	bool lightIsOn;
	ControlledEntity(Mesh* Object, Material* materialInput, b2World* world = nullptr);
	~ControlledEntity();
	void HandleKeyboardInput(float moveSpeed);
};

