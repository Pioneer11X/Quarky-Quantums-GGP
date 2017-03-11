#include "Entity.h"



Entity::Entity(Mesh * Object, Material* materialInput)
{
	isDirty = true;
	meshObj = Object;
	material = materialInput;
	SetScale(1.0f, 1.0f, 1.0f);

	XMStoreFloat4(&rotation, XMQuaternionIdentity());
	XMStoreFloat4x4(&rotationMatrix, XMMatrixRotationQuaternion(XMQuaternionIdentity()));

	SetTranslation(0.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMMatrixIdentity()));
}


Entity::~Entity()
{
}

void Entity::SetTranslation(float x, float y, float z)
{
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;

	XMStoreFloat4x4(&translationMatrix, XMMatrixTranslation(x, y, z));
	
	isDirty = true;
}

void Entity::SetRotation(float x, float y, float z, float w)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
	rotation.w = w;
	
	XMVECTOR quaternion = XMVectorSet(x, y, z, w);
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

void Entity::MoveForward(float x)
{
	XMFLOAT3 rotatedVector;
	XMStoreFloat3(&rotatedVector, XMVector3Rotate(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMLoadFloat4(&rotation)));
	Move(x * rotatedVector.x, x * rotatedVector.y, 0.0f);

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

void Entity::PrepareMaterial(XMFLOAT4X4 camViewMatrix, XMFLOAT4X4 camProjectionMatrix)
{
	CalculateWorldMatrix();
	material->GetVertexShader()->SetMatrix4x4("world", worldMatrix);
	material->GetVertexShader()->SetMatrix4x4("view", camViewMatrix);
	material->GetVertexShader()->SetMatrix4x4("projection", camProjectionMatrix);
	material->GetPixelShader()->SetShaderResourceView("diffuseTexture", material->GetSRV());
	material->GetPixelShader()->SetSamplerState("basicSampler", material->GetSamplerState());

}

Material * Entity::GetMaterial()
{
	return material;
}
