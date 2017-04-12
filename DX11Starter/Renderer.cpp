#include "Renderer.h"
#include "Game.h"


Renderer::Renderer(ID3D11Device * deviceIn, ID3D11DeviceContext * contextIn)
{
	device = deviceIn;
	context = contextIn;

	D3D11_DEPTH_STENCIL_DESC lessEqualsDesc = {};
	lessEqualsDesc.DepthEnable = true;
	lessEqualsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	lessEqualsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&lessEqualsDesc, &depthStencilState);

	// Set up a rasterizer state with no culling
	D3D11_RASTERIZER_DESC rd = {};
	rd.CullMode = D3D11_CULL_BACK;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.DepthClipEnable = true;
	device->CreateRasterizerState(&rd, &rsCullBack);

	// Set up a rasterizer state with front culling
	D3D11_RASTERIZER_DESC rd2 = {};
	rd2.CullMode = D3D11_CULL_FRONT;
	rd2.FillMode = D3D11_FILL_SOLID;
	rd2.DepthClipEnable = true;
	device->CreateRasterizerState(&rd2, &rsCullFront);

	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&bd, &bsAlphaBlend);
}


Renderer::~Renderer()
{
	if (rsCullFront) { rsCullFront->Release(); }
	if (rsCullBack) { rsCullBack->Release(); }
	if (depthStencilState) { depthStencilState->Release(); }
	if (bsAlphaBlend) { bsAlphaBlend->Release(); }
}

void Renderer::DrawEntity(Entity* entity)
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

void Renderer::DrawSkyBox(Entity * skyBox, XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix)
{
	// Draw SkyBox after drawing all Opaque objects
	context->RSSetState(rsCullFront);
	context->OMSetDepthStencilState(depthStencilState, 1);

	skyBox->PrepareMaterial(viewMatrix, projectionMatrix);
	skyBox->GetMaterial()->GetVertexShader()->CopyAllBufferData();
	skyBox->GetMaterial()->GetPixelShader()->CopyAllBufferData();
	skyBox->GetMaterial()->GetVertexShader()->SetShader();
	skyBox->GetMaterial()->GetPixelShader()->SetShader();

	DrawEntity(skyBox);
}

void Renderer::Draw(std::vector<Entity*> entities, Entity* skyBox, XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix, DirectionalLight* dirLights, PointLight* pointLights, SpotLight* spotLights)
{
	context->RSSetState(nullptr);

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

		entities[0]->GetMaterial()->GetPixelShader()->SetData(
			"spotLight",  // The name of the (eventual) variable in the shader
			&spotLights[0],   // The address of the data to copy
			sizeof(SpotLight)); // The size of the data to copy
	};

	SetGlobalData();
#pragma endregion

	std::vector<Entity*> blendEntities;

#pragma region Draw Opaque Objects
	for (Entity* entity : entities)
	{
		if (entity->GetAlpha() < 1.0f)
		{
			blendEntities.push_back(entity);
			continue;
		}

		entity->PrepareMaterial(viewMatrix, projectionMatrix);
		entity->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", Game::Instance()->GetCameraPostion());

		entity->GetMaterial()->GetVertexShader()->CopyAllBufferData();
		entity->GetMaterial()->GetPixelShader()->CopyAllBufferData();
		entity->GetMaterial()->GetVertexShader()->SetShader();
		entity->GetMaterial()->GetPixelShader()->SetShader();

		DrawEntity(entity);
	}

#pragma endregion

#pragma region Draw SkyBox
	DrawSkyBox(skyBox, viewMatrix, projectionMatrix);
#pragma endregion

#pragma region Draw Blended Objects
	// Turn on our custom blend state to enable alpha blending
	context->RSSetState(rsCullBack);
	context->OMSetBlendState(bsAlphaBlend, 0, 0xFFFFFFFF);

	auto sortBlendObjsByZVal = [&]() {
		bool sorted = false;
		size_t i = 0;
		int j = 1;

		while (!sorted)
		{
			sorted = true;
			for (i = 0; i < blendEntities.size() - j; i++)
			{
				if (blendEntities[i]->GetPosition().z < blendEntities[i + 1]->GetPosition().z)
				{
					Entity* temp;
					temp = blendEntities[i];
					blendEntities[i] = blendEntities[i + 1];
					blendEntities[i + 1] = temp;

					sorted = false;
				}
			}
			j++;
		}
	};

	sortBlendObjsByZVal();

	for (Entity* entity : blendEntities)
	{
		entity->PrepareMaterial(viewMatrix, projectionMatrix);
		entity->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", Game::Instance()->GetCameraPostion());

		entity->GetMaterial()->GetVertexShader()->CopyAllBufferData();
		entity->GetMaterial()->GetPixelShader()->CopyAllBufferData();
		entity->GetMaterial()->GetVertexShader()->SetShader();
		entity->GetMaterial()->GetPixelShader()->SetShader();

		DrawEntity(entity);
	}
#pragma endregion

}

