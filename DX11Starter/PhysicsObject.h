#pragma once

#include "Box2D\Box2D.h"
#include <string>

class PhysicsObject
{

public:

	PhysicsObject(b2World* _world, bool _isDynamic, bool isTrigger, float _posX, float _posY, float _sizeX, float _sizeY, std::string _name = "");

	void InitPhysicsObject(std::string _name);
	b2Body* GetPhysicsBody();

	void DeactivatePhysicsObject();
	void ReactivatePhysicsObject();

	float GetPosX();
	float GetPosY();

	float GetSizeX();
	float GetSizeY();

	std::string _physicsName;

	~PhysicsObject();


private:

	b2Body * PhysicsBodyPtr;		// The Pointer that actually points to the Physics Body

	b2World * world;				// The pointer to the World object.
	bool isDynamic;				// To check whether the body is dynamic ( Like Players and objects that need Physics ) or Static ( Objects like Platforms that defy physics but need collisions)
	bool isTrigger;
	float posX, posY;				// The position of the Body ( Initially ) This would Update Periodically. ( For EveryPhysics Tick ).
	float sizeX, sizeY;				// The Size of the Collider. Need to check how this correlates to the unit in Graphics Engine.

	bool isActive;					// This would be the trigger to set it active or not. This bool can be used for both initialising and stopping the update ( Pausing the update loop ). We destroy the object completely if this is set to not active. We do not want this active as this collides with the other objects.

};

