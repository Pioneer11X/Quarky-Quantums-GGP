#pragma once
#include <DirectXMath.h>
#include <math.h>
#include "Mesh.h"
#include "Material.h"
#include "Box2D\Box2D.h"

// For the DirectX Math library
using namespace DirectX;

class Entity
{
private:
	// Entity values
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 scaleMatrix;
	XMFLOAT4X4 rotationMatrix;
	XMFLOAT4X4 translationMatrix;
	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT4 rotation;

	// Mesh obj pointer
	Mesh* meshObj;

	// Material obj pointer
	Material* material;

	// RigidBody
	

	// Boolean to check if data has changed since previous frame
	bool isDirty;
	b2Body* PhysicsBody;

public:
	//Entity(Mesh* Object, Material* materialInput);
	Entity(Mesh* Object, Material* materialInput, b2World *world = nullptr);
	~Entity();
	XMFLOAT3 GetPosition();
	void SetTranslation(float x, float y, float z);
	void SetRotation(float x, float y, float z, float w);
	void SetRotationAboutZ(float angle);
	void SetScale(float x, float y, float z);
	void Move(float x, float y, float z);
	void CalculateWorldMatrix();
	XMFLOAT4X4& GetWorldMatrix();

	// Get Mesh
	Mesh* GetMesh();
	void AddPhysicsBody(b2World* world);
	void UpdatePhysicsTick();

	// Set WVP
	void PrepareMaterial(XMFLOAT4X4 camViewMatrix, XMFLOAT4X4 camProjectionMatrix);

	// Get material
	Material* GetMaterial();
};


