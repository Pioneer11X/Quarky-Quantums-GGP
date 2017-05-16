#include "Renderer.h"
#include "Game.h"


Renderer::Renderer(ID3D11Device * deviceIn, ID3D11DeviceContext * contextIn)
{
	device = deviceIn;
	context = contextIn;
	
	shadowVS = new SimpleVertexShader(device, context);
	if (!shadowVS->LoadShaderFile(L"Assets/ShaderObjs/x86/ShadowVS.cso"))
		shadowVS->LoadShaderFile(L"Assets/ShaderObjs/x64/ShadowVS.cso");
	
	beamPS = new SimplePixelShader(device, context);
	if (!beamPS->LoadShaderFile(L"Assets/ShaderObjs/x86/BeamPS.cso"))
		beamPS->LoadShaderFile(L"Assets/ShaderObjs/x64/BeamPS.cso");

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

	D3D11_RASTERIZER_DESC rd3 = {};
	rd3.CullMode = D3D11_CULL_NONE;
	rd3.FillMode = D3D11_FILL_SOLID;
	rd3.DepthClipEnable = true;
	device->CreateRasterizerState(&rd2, &rsCullNone);

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

	// Create shadow requirements ------------------------------------------
	shadowMapSize = 2048;


	// Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapSize;
	shadowDesc.Height = shadowMapSize;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, &shadowTexture);

	// Create the depth/stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &shadowDSV);

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowSRV);

	// Release the texture reference since we don't need it
	shadowTexture->Release();

	// Create the special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	backbufferRTV = Game::Instance()->GetBackBufferRTV();
	depthStencilView = Game::Instance()->GetDSV();

	pr = new PrimitiveRenderer();
	pr->Init(device, context);

}


Renderer::~Renderer()
{
	if (rsCullFront) { rsCullFront->Release(); }
	if (rsCullBack) { rsCullBack->Release(); }
	if (rsCullNone) { rsCullNone->Release(); }
	if (depthStencilState) { depthStencilState->Release(); }
	if (bsAlphaBlend) { bsAlphaBlend->Release(); }
	if (shadowVS) { delete shadowVS; }

	// Clean up shadow map
	shadowDSV->Release();
	shadowSRV->Release();
	shadowRasterizer->Release();
	shadowSampler->Release();

	// Clean up Volumetric stuff
	delete beamPS;
	pr->CleanUp();
	delete pr;
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

void Renderer::DrawBeam(XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix)
{
	context->OMSetDepthStencilState(nullptr, NULL);
	context->RSSetState(rsCullNone);
	Entity* beam = Game::instance->spotLightEntity->GetEntity();

	beam->CalculateWorldMatrix();
	Game::instance->vertexShader->SetMatrix4x4("world", beam->GetWorldMatrix());
	Game::instance->vertexShader->SetMatrix4x4("view", viewMatrix);
	Game::instance->vertexShader->SetMatrix4x4("projection", projectionMatrix);
	Game::instance->vertexShader->SetMatrix4x4("shadowView", shadowViewMatrix);
	Game::instance->vertexShader->SetMatrix4x4("shadowProj", shadowProjectionMatrix);

	beamPS->SetShaderResourceView("ShadowMap", shadowSRV);
	beamPS->SetSamplerState("ShadowSampler", shadowSampler);
	beamPS->SetData("spotLight", &(Game::Instance()->playerChar->GetLight()), sizeof(SpotLight));
	beamPS->SetFloat3("cameraPosition", Game::Instance()->GetCameraPostion());
	beamPS->SetFloat("alpha", beam->GetAlpha());
	beamPS->SetFloat("scatterAmount", 500.0f);

	Game::instance->vertexShader->CopyAllBufferData();
	beamPS->CopyAllBufferData();
	Game::instance->vertexShader->SetShader();
	beamPS->SetShader();

	DrawEntity(beam);
	beamPS->SetShaderResourceView("ShadowMap", 0);
}

void Renderer::RenderShadowMap(std::vector<Entity*> entities)
{
	// Initial setup: No RTV (remember to clear shadow map)
	context->OMSetRenderTargets(0, 0, shadowDSV);
	context->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadowRasterizer);

	// Set up an appropriate shadow view port
	D3D11_VIEWPORT shadowVP = {};
	shadowVP.TopLeftX = 0;
	shadowVP.TopLeftY = 0;
	shadowVP.Width = (float)shadowMapSize;
	shadowVP.Height = (float)shadowMapSize;
	shadowVP.MinDepth = 0.0f;
	shadowVP.MaxDepth = 1.0f;
	context->RSSetViewports(1, &shadowVP);

	// Set up shaders for making the shadow map
	shadowVS->SetShader();
	shadowVS->SetMatrix4x4("view", shadowViewMatrix);
	shadowVS->SetMatrix4x4("projection", shadowProjectionMatrix);

	// Turn off pixel shader
	context->PSSetShader(0, 0, 0);

	// Actually draw the entities
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		// Grab the data from the first entity's mesh
		Entity* ge = entities[i];
		if (ge->GetAlpha() < 1.0f)
			continue;

		if (ge == Game::Instance()->playerChar)
			continue;
		ID3D11Buffer* vb = ge->GetMesh()->GetVertexBuffer();
		ID3D11Buffer* ib = ge->GetMesh()->GetIndexBuffer();

		// Set buffers in the input assembler
		context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		shadowVS->SetMatrix4x4("world", ge->GetWorldMatrix());
		shadowVS->CopyAllBufferData();

		// Finally do the actual drawing
		context->DrawIndexed(ge->GetMesh()->GetIndexCount(), 0, 0);
	}

	// Revert to original targets and states
	context->OMSetRenderTargets(1, &backbufferRTV, depthStencilView);
	shadowVP.Width = (float)Game::Instance()->GetScreenWidth();
	shadowVP.Height = (float)Game::Instance()->GetScreenHeight();
	context->RSSetViewports(1, &shadowVP);
	context->RSSetState(0);
}

void Renderer::Draw(std::vector<Entity*> entities, Entity* skyBox, XMFLOAT4X4& viewMatrix, XMFLOAT4X4& projectionMatrix, DirectionalLight* dirLights, PointLight* pointLights, SpotLight* spotLights)
{

	SpotLight lt = Game::Instance()->playerChar->GetLight();
	// Shadow view matrix (where the light is looking from)
	XMMATRIX shView = XMMatrixLookToLH(
		XMVectorSet(lt.Position.x, lt.Position.y, lt.Position.z, 0), // Eye position
		XMVectorSet(lt.Direction.x, lt.Direction.y, lt.Direction.z, 0),		// Look at pos
		XMVectorSet(0, 1, 0, 0));		// Up
	DirectX::XMStoreFloat4x4(&shadowViewMatrix, XMMatrixTranspose(shView));

	XMMATRIX shProj = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		1,//(float)Game::Instance()->GetScreenWidth() / Game::Instance()->GetScreenHeight(),		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	DirectX::XMStoreFloat4x4(&shadowProjectionMatrix, XMMatrixTranspose(shProj));

	RenderShadowMap(entities);

	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backbufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

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
		if ("SpotLight" == entity->GetPhysicsObject()->_physicsName) {
			//continue;
		}

		if (entity->GetAlpha() < 1.0f)
		{
			blendEntities.push_back(entity);
			continue;
		}

		entity->PrepareMaterial(viewMatrix, projectionMatrix);


		entity->GetMaterial()->GetVertexShader()->SetMatrix4x4("shadowView", shadowViewMatrix);
		entity->GetMaterial()->GetVertexShader()->SetMatrix4x4("shadowProj", shadowProjectionMatrix);

		entity->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", Game::Instance()->GetCameraPostion());
		entity->GetMaterial()->GetPixelShader()->SetShaderResourceView("ShadowMap", shadowSRV);
		entity->GetMaterial()->GetPixelShader()->SetSamplerState("ShadowSampler", shadowSampler);

		entity->GetMaterial()->GetVertexShader()->CopyAllBufferData();
		entity->GetMaterial()->GetPixelShader()->CopyAllBufferData();
		entity->GetMaterial()->GetVertexShader()->SetShader();
		entity->GetMaterial()->GetPixelShader()->SetShader();

		DrawEntity(entity);
		entity->GetMaterial()->GetPixelShader()->SetShaderResourceView("ShadowMap", 0);
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
			if (blendEntities.size() > 0) {
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
		}
	};

	sortBlendObjsByZVal();

	for (Entity* entity : blendEntities)
	{
		entity->PrepareMaterial(viewMatrix, projectionMatrix);
		entity->GetMaterial()->GetVertexShader()->SetMatrix4x4("shadowView", shadowViewMatrix);
		entity->GetMaterial()->GetVertexShader()->SetMatrix4x4("shadowProj", shadowProjectionMatrix);
		entity->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", Game::Instance()->GetCameraPostion());
		entity->GetMaterial()->GetPixelShader()->SetShaderResourceView("ShadowMap", shadowSRV);
		entity->GetMaterial()->GetPixelShader()->SetSamplerState("ShadowSampler", shadowSampler);

		entity->GetMaterial()->GetVertexShader()->CopyAllBufferData();
		entity->GetMaterial()->GetPixelShader()->CopyAllBufferData();
		entity->GetMaterial()->GetVertexShader()->SetShader();
		entity->GetMaterial()->GetPixelShader()->SetShader();

		DrawEntity(entity);
		entity->GetMaterial()->GetPixelShader()->SetShaderResourceView("ShadowMap", 0);
	}
#pragma endregion

	DrawBeam(viewMatrix, projectionMatrix);

#pragma region BoundingBoxDisplay
	//for (Entity* entity : entities) {
	//	if ("SpotLight" == entity->GetPhysicsObject()->_physicsName)
	//		std::cout << "lkasdj" << std::endl;

	//	auto& bounds = entity->GetBounds();
	//	pr->Draw(bounds, PrimitiveRenderer::blue);
	//	// pr->Reset();
	//}
	//pr->Draw(Game::Instance()->spotLightEntity->GetEntity()->coneBounds, PrimitiveRenderer::red);

	//pr->Render(viewMatrix, projectionMatrix);
#pragma endregion

}

