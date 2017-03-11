#include "Renderer.h"
#include "Game.h"


Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::DrawEntity(Entity* entity, ID3D11DeviceContext*	context)
{
	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	ID3D11Buffer* vPointer = nullptr;

	vPointer = entity->GetMesh()->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vPointer, &stride, &offset);
	context->IASetIndexBuffer(entity->GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(
		entity->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}

void Renderer::Draw(std::vector<Entity*> entities, ID3D11DeviceContext * context, XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix, DirectionalLight* dirLights, PointLight* pointLights)
{
#pragma region Setting the common data

	auto SetGlobalData = [&]() {

		entities[0]->GetMaterial()->GetPixelShader()->SetData(
			"light",  // The name of the (eventual) variable in the shader
			&dirLights[0],   // The address of the data to copy
			sizeof(DirectionalLight)); // The size of the data to copy

		entities[0]->GetMaterial()->GetPixelShader()->SetData(
			"light2",  // The name of the (eventual) variable in the shader
			&dirLights[1],   // The address of the data to copy
			sizeof(DirectionalLight)); // The size of the data to copy

		entities[0]->GetMaterial()->GetPixelShader()->SetData(
			"pointLight",  // The name of the (eventual) variable in the shader
			&pointLights[0],   // The address of the data to copy
			sizeof(PointLight)); // The size of the data to copy
	};

	SetGlobalData();
#pragma endregion


	for (Entity* entity : entities)
	{
		entity->PrepareMaterial(viewMatrix, projectionMatrix);

		// Once you've set all of the data you care to change for
		// the next draw call, you need to actually send it to the GPU
		//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
		entity->GetMaterial()->GetVertexShader()->CopyAllBufferData();

		entity->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", Game::Instance()->GetCameraPostion());

		entity->GetMaterial()->GetPixelShader()->CopyAllBufferData();

		// Set the vertex and pixel shaders to use for the next Draw() command
		//  - These don't technically need to be set every frame...YET
		//  - Once you start applying different shaders to different objects,
		//    you'll need to swap the current shaders before each draw
		entity->GetMaterial()->GetVertexShader()->SetShader();
		entity->GetMaterial()->GetPixelShader()->SetShader();

		DrawEntity(entity, context);
	}

}
