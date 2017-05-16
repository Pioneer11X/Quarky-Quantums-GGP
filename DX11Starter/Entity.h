#pragma once
#include <DirectXMath.h>
#include <math.h>
#include <string>
#include "Mesh.h"
#include "Material.h"
#include "PhysicsObject.h"
#include <DirectXCollision.h>

//DirectX::XMFLOAT3 operator - (DirectX::XMFLOAT3 left, DirectX::XMFLOAT3 right) {
//	return XMFLOAT3(left.x - right.x, left.y - right.y, left.z - right.z);
//}

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
	XMFLOAT3 rotation;

	// Mesh obj pointer
	Mesh* meshObj;

	// Material obj pointer
	Material* material;

	// Alpha value of the Entity
	float alpha;

	// Boolean to check if data has changed since previous frame
	bool isDirty;
	bool customPivot;
	PhysicsObject* pb = nullptr; // For some reason, it doesn't default to this?
	bool hasPhysics;
	bool hasTrigger = false; // Can this entity be trigerred by the spotlight.

	// DirectXBoundingBoxes.
	DirectX::BoundingBox bounds;
	
	// DirectX::BoundingBox& boundsRef;

public:

	DirectX::BoundingFrustum coneBounds;

	//Entity(Mesh* Object, Material* materialInput);
	Entity(Mesh* Object, Material* materialInput, float _posX, float _posY, float _posz, b2World *world = nullptr, std::string _nameForPhysicsBody = "", bool isDynamic = true, bool CanBeTriggered = false, float _sizeX = 1.0f, float _sizeY = 1.0f, float _scaleX = 1.0f, float _scaleY = 1.0f, float _scaleZ = 1.0f);
	~Entity();
	XMFLOAT3 GetPosition();
	void SetTranslation(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	inline XMFLOAT3 GetRotation() { return rotation; };
	void SetRotationAboutZ(float angle);
	void SetScale(float x, float y, float z);
	inline XMFLOAT3 GetScale() { return scale; }
	void Move(float x, float y, float z);
	void CalculateWorldMatrix();
	XMFLOAT4X4& GetWorldMatrix();

	// Get Mesh
	Mesh* GetMesh();
	void UpdatePhysicsTick();

	// Set Alpha
	void SetAlpha(float val);

	// Get Alpha
	float& GetAlpha();

	// Set WVP
	void PrepareMaterial(XMFLOAT4X4 camViewMatrix, XMFLOAT4X4 camProjectionMatrix);

	// Get material
	Material* GetMaterial();

	// Getter for Physics Body
	PhysicsObject* GetPhysicsObject() {
		return pb;
	}

	bool NeedsPhysicsUpdate();
	bool CanBeTrigerred();

	DirectX::BoundingBox& GetBounds() { return bounds; }
	void SetBounds(DirectX::BoundingBox newBounds) { bounds = newBounds; }

	void UpdateBounds();
};


