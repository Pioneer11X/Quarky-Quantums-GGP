#include "ControlledEntity.h"



ControlledEntity::ControlledEntity(Mesh * Object, Material* materialInput, float _posX, float _posY, float _posZ, SpotLightWrapper* newSpotLight, b2World* world, bool isDynamic, float _sizeX, float _sizeY) : Entity(Object, materialInput, _posX, _posY, _posZ, world, isDynamic, _sizeX, _sizeY)
{
	lightIsOn = true;
	mySpotLight = newSpotLight;
	canJump = false;
	jumpHeight = 10.0;
	maxSpeed = 10;
}


ControlledEntity::~ControlledEntity()
{
}

void ControlledEntity::UpdateSpotLightPosition()
{
	XMFLOAT3 newLightPos = this->GetPosition();
	newLightPos.y += .02f;
	mySpotLight->UpdateLightPoistion(newLightPos);
}

void ControlledEntity::CheckForCollisions()
{
	//If the player is flagged as "cant jump", and they are currently falling
	if (!canJump && this->GetPhysicsObject()->GetPhysicsBody()->GetLinearVelocity().y < 0) {
		//Check all the things it is in contact with
		for (b2ContactEdge* edge = this->GetPhysicsObject()->GetPhysicsBody()->GetContactList(); edge; edge = edge->next) {
			if (edge->contact->IsTouching()) {
				//Get the collision fixtures of the two objects
				b2Fixture* a = edge->contact->GetFixtureA(); //Collision object
				b2Fixture* b = edge->contact->GetFixtureB(); //This object

				//Calculate their centerpoints (in the y), and their Y halfwidths
				float aY = a->GetBody()->GetPosition().y;
				float aYExtends = a->GetAABB(0).GetExtents().y;
				float bY = b->GetBody()->GetPosition().y;
				float bYExtends = b->GetAABB(0).GetExtents().y;

				//Calculate their summed centers and halfwidths
				float aCheck = abs(aY + aYExtends);
				float bCheck = abs((bY - bYExtends) - 0.1); //-0.1 to account for minute "bobbing" of an object sitting on another

				//If a is below b, then we can jump again
				if (aCheck <= bCheck) {
					canJump = true;
				}
			}
		}
	}
}



void ControlledEntity::HandleKeyboardInput(float deltaTime)
{
	b2Vec2 tempImpulse = b2Vec2(0.0f, 0.0f);

	if (InputManager::Instance()->GetKeyHolding(KeyPressed::LEFT))
	{
		this->GetPhysicsObject()->GetPhysicsBody()->SetLinearVelocity(
			this->GetPhysicsObject()->GetPhysicsBody()->GetLinearVelocity() + b2Vec2(-maxSpeed * deltaTime, 0));
	}

	if (InputManager::Instance()->GetKeyHolding(KeyPressed::RIGHT))
	{
		this->GetPhysicsObject()->GetPhysicsBody()->SetLinearVelocity(
			this->GetPhysicsObject()->GetPhysicsBody()->GetLinearVelocity() + b2Vec2(maxSpeed * deltaTime, 0));
	}

	if (InputManager::Instance()->GetKeyDown(KeyPressed::UP))
	{
		if (canJump) {
			tempImpulse = b2Vec2(0, jumpHeight);
			canJump = false;
		}
	}

	this->GetPhysicsObject()->GetPhysicsBody()->ApplyLinearImpulseToCenter(tempImpulse, true);

	// TODO: Need to add Raycast or something to check for the player if they are actually on the ground
	if (this->GetPhysicsObject()->GetPhysicsBody()->GetLinearVelocity().Length() > 0) {
		
		//Set max for velocity
		b2Vec2 velocity = this->GetPhysicsObject()->GetPhysicsBody()->GetLinearVelocity();
		if (velocity.Length() > maxSpeed) {
			velocity.Normalize();
			velocity *= maxSpeed;
			this->GetPhysicsObject()->GetPhysicsBody()->SetLinearVelocity(velocity);
		};

	}
	if (InputManager::Instance()->GetKeyDown(KeyPressed::FORWARD))
	{
		lightIsOn = !lightIsOn;
	}

	CheckForCollisions();
}
