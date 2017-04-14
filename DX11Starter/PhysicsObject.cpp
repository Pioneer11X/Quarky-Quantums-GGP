#include "PhysicsObject.h"

PhysicsObject::PhysicsObject(b2World * _world, bool _isDynamic, float _posX, float _posY, float _sizeX, float _sizeY)
{
	world = _world;
	isDynamic = _isDynamic;
	posX = _posX;
	posY = _posY;
	sizeX = _sizeX;
	sizeY = _sizeY;

	InitPhysicsObject();

}

void PhysicsObject::InitPhysicsObject()
{
	b2BodyDef PhysicsBodyDef;
	if (isDynamic)
		PhysicsBodyDef.type = b2_dynamicBody;
	PhysicsBodyDef.position.Set(posX, posY);

	PhysicsBodyPtr = (*world).CreateBody(&PhysicsBodyDef);

	b2PolygonShape PhysicsBox;
	PhysicsBox.SetAsBox(sizeX, sizeY);

	b2FixtureDef FixDef;
	FixDef.shape = &PhysicsBox;
	FixDef.density = 1.0f;
	FixDef.friction = 0.3f;

	PhysicsBodyPtr->CreateFixture(&FixDef);
	isActive = true;
}

b2Body * PhysicsObject::GetPhysicsBody()
{
	return PhysicsBodyPtr;
}

void PhysicsObject::DeactivatePhysicsObject()
{
	//(*world).DestroyBody(PhysicsBodyPtr);
	PhysicsBodyPtr->SetActive(false);
	isActive = false;
}

void PhysicsObject::ReactivatePhysicsObject()
{
	//InitPhysicsObject();
	PhysicsBodyPtr->SetActive(true);
	isActive = true;
}

float PhysicsObject::GetPosX()
{
	return posX;
}

float PhysicsObject::GetPosY()
{
	return posY;
}


float PhysicsObject::GetSizeX()
{
	return sizeX;
}

float PhysicsObject::GetSizeY()
{
	return sizeY;
}

PhysicsObject::~PhysicsObject()
{
	if (PhysicsBodyPtr != nullptr) {
		(*world).DestroyBody(PhysicsBodyPtr);
	}
	//if (PhysicsBodyPtr) delete PhysicsBodyPtr; We cannot delete because Box2D Should be doing that.
}
