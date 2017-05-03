#include "BoxListener.h"


BoxListener::BoxListener(b2Body * SpotLightBody)
{

	_spotLightSensor = SpotLightBody->GetFixtureList();

}

BoxListener::~BoxListener()
{
}

void BoxListener::BeginContact(b2Contact * contact)
{

	// Only Check for Sensors here.

}

void BoxListener::EndContact(b2Contact * contact)
{
}
