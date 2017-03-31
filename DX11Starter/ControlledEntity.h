#pragma once
#include "Entity.h"
#include "InputManager.h"

class ControlledEntity :
	public Entity
{
public:
	bool lightIsOn;
	ControlledEntity(Mesh * Object, Material* materialInput, float _posX, float _posY, float _posZ, b2World* world = nullptr, bool isDynamic = true, float _sizeX = 1, float _sizeY = 1);
	~ControlledEntity();
	void HandleKeyboardInput(float moveSpeed);
};
 
