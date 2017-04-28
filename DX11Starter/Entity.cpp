#include "Entity.h"



Entity::Entity(Mesh * Object, Material* materialInput, float _posX, float _posY, float _posZ, b2World* world, bool isDynamic, float _sizeX, float _sizeY, float _scaleX, float _scaleY, float _scaleZ)
{
	pb = nullptr;		// Need this I guess.
	isDirty = true;
	meshObj = Object;
	material = materialInput;
	SetScale(_scaleX, _scaleY, _scaleZ);
	alpha = 1.0f;
	hasPhysics = isDynamic;

	rotation.x = 0.0f;
	rotation.y = 0.0f;
	rotation.z = 0.0f;

	XMStoreFloat4x4(&rotationMatrix, XMMatrixRotationQuaternion(XMQuaternionIdentity()));

	SetTranslation(_posX, _posY, _posZ);

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	if (world != nullptr)
		pb = new PhysicsObject(world, isDynamic, _posX, _posY, _sizeX, _sizeY);
	
}

Entity::~Entity()
{
	if (pb != nullptr) delete pb;
}

XMFLOAT3 Entity::GetPosition()
{
	return position;
}

void Entity::SetTranslation(float x, float y, float z)
{
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;

	XMStoreFloat4x4(&translationMatrix, XMMatrixTranslation(x, y, z));

	isDirty = true;
}

void Entity::SetRotation(float x, float y, float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMQuaternionNormalize(quaternion);

	XMStoreFloat4x4(&rotationMatrix, XMMatrixRotationQuaternion(quaternion));

	isDirty = true;
}

void Entity::SetRotationAboutZ(float angle)
{
	XMStoreFloat4x4(&rotationMatrix, XMMatrixRotationZ(angle));

	isDirty = true;
}

void Entity::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;

	XMStoreFloat4x4(&scaleMatrix, XMMatrixScaling(x, y, z));

	isDirty = true;
}

void Entity::Move(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;

	XMStoreFloat4x4(&translationMatrix, XMMatrixTranslation(position.x, position.y, position.z));

	isDirty = true;
}

void Entity::CalculateWorldMatrix()
{
	if (isDirty)
	{
		XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&scaleMatrix) * XMLoadFloat4x4(&rotationMatrix) * XMLoadFloat4x4(&translationMatrix)));

		isDirty = false;
	}
}

Mesh * Entity::GetMesh()
{
	return meshObj;
}

XMFLOAT4X4& Entity::GetWorldMatrix()
{
	return worldMatrix;
}

void Entity::UpdatePhysicsTick()
{

	if (pb != nullptr) {
		SetTranslation(pb->GetPhysicsBody()->GetPosition().x, pb->GetPhysicsBody()->GetPosition().y, GetPosition().z);
	}

}

void Entity::SetAlpha(float val)
{
	alpha = val;
}

float & Entity::GetAlpha()
{
	// TODO: insert return statement here
	return alpha;
}



void Entity::PrepareMaterial(XMFLOAT4X4 camViewMatrix, XMFLOAT4X4 camProjectionMatrix)
{
	CalculateWorldMatrix();
	material->GetVertexShader()->SetMatrix4x4("world", worldMatrix);
	material->GetVertexShader()->SetMatrix4x4("view", camViewMatrix);
	material->GetVertexShader()->SetMatrix4x4("projection", camProjectionMatrix);
	material->GetPixelShader()->SetShaderResourceView("Texture", material->GetSRV());
	material->GetPixelShader()->SetSamplerState("Sampler", material->GetSamplerState());
	material->GetPixelShader()->SetFloat("alpha", alpha);
}

Material * Entity::GetMaterial()
{
	return material;
}

bool Entity::NeedsPhysicsUpdate()
{
	return hasPhysics;
}
