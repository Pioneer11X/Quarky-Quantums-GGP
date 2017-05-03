#pragma once

#include "Box2D\Box2D.h"

class BoxListener : public b2ContactListener
{

private:
	b2Fixture * _spotLightSensor;

public:
	BoxListener(b2Body * SpotLightBody);
	~BoxListener();

	/// Called when two fixtures begin to touch.
	void BeginContact(b2Contact* contact);

	/// Called when two fixtures cease to touch.
	void EndContact(b2Contact* contact);
};