#include "Emitter.h"

using namespace DirectX;

Emitter::Emitter(
	unsigned int maxParticles,
	float emissionRate,
	float lifetime,
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	SimpleComputeShader* deadListInitCS,
	SimpleComputeShader* emitCS,
	SimpleComputeShader* updateCS,
	SimpleComputeShader* copyDrawCountCS,
	SimpleVertexShader* particleVS,
	SimplePixelShader* particlePS)
{
	this->maxParticles = maxParticles;
	this->lifetime = lifetime;
	this->emissionRate = emissionRate;

	this->context = context;
	this->emitCS = emitCS;
	this->updateCS = updateCS;
	this->copyDrawCountCS = copyDrawCountCS;
	this->particleVS = particleVS;
	this->particlePS = particlePS;

	// Calculated members
	emitTimeCounter = 0.0f;
	timeBetweenEmit = 1.0f / emissionRate;


	// Particle index buffer =================
	{
		// Buffer
		D3D11_BUFFER_DESC ibDesc = {};
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.ByteWidth = sizeof(unsigned long) * maxParticles * 6;
		ibDesc.CPUAccessFlags = 0;
		ibDesc.MiscFlags = 0;
		ibDesc.StructureByteStride = 0;
		ibDesc.Usage = D3D11_USAGE_DEFAULT;

		// Fill it with data
		unsigned long* indices = new unsigned long[maxParticles * 6];
		for (unsigned long i = 0; i < maxParticles; i++)
		{
			unsigned long indexCounter = i * 6;
			indices[indexCounter + 0] = 0 + i * 4;
			indices[indexCounter + 1] = 1 + i * 4;
			indices[indexCounter + 2] = 2 + i * 4;
			indices[indexCounter + 3] = 1 + i * 4;
			indices[indexCounter + 4] = 3 + i * 4;
			indices[indexCounter + 5] = 2 + i * 4;
		}

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = indices;
		device->CreateBuffer(&ibDesc, &data, &indexBuffer);
		
		delete[] indices;
	}

	// PARTICLE POOL ============
	{
		// Buffer
		ID3D11Buffer* particlePoolBuffer;
		D3D11_BUFFER_DESC poolDesc = {};
		poolDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		poolDesc.ByteWidth = sizeof(Particle) * maxParticles;
		poolDesc.CPUAccessFlags = 0;
		poolDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		poolDesc.StructureByteStride = sizeof(Particle);
		poolDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateBuffer(&poolDesc, 0, &particlePoolBuffer);

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC poolUAVDesc = {};
		poolUAVDesc.Format = DXGI_FORMAT_UNKNOWN; // Needed for RW structured buffers
		poolUAVDesc.Buffer.FirstElement = 0;
		poolUAVDesc.Buffer.Flags = 0;
		poolUAVDesc.Buffer.NumElements = maxParticles;
		poolUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		device->CreateUnorderedAccessView(particlePoolBuffer, &poolUAVDesc, &particlePoolUAV);

		// SRV (for indexing in VS)
		D3D11_SHADER_RESOURCE_VIEW_DESC poolSRVDesc = {};
		poolSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		poolSRVDesc.Buffer.FirstElement = 0;
		poolSRVDesc.Buffer.NumElements = maxParticles;
		// Don't actually set these!  They're union'd with above data, so 
		// it will just overwrite correct values with incorrect values
		//poolSRVDesc.Buffer.ElementOffset = 0;
		//poolSRVDesc.Buffer.ElementWidth = sizeof(Particle);
		poolSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		device->CreateShaderResourceView(particlePoolBuffer, &poolSRVDesc, &particlePoolSRV);

		// Done
		particlePoolBuffer->Release();
	}

	// DEAD LIST ===================
	{
		// Buffer
		ID3D11Buffer* deadListBuffer;
		D3D11_BUFFER_DESC deadDesc = {};
		deadDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		deadDesc.ByteWidth = sizeof(unsigned int) * maxParticles;
		deadDesc.CPUAccessFlags = 0;
		deadDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		deadDesc.StructureByteStride = sizeof(unsigned int);
		deadDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateBuffer(&deadDesc, 0, &deadListBuffer);

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC deadUAVDesc = {};
		deadUAVDesc.Format = DXGI_FORMAT_UNKNOWN; // Needed for RW structured buffers
		deadUAVDesc.Buffer.FirstElement = 0;
		deadUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND; // Append/Consume
		deadUAVDesc.Buffer.NumElements = maxParticles;
		deadUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		device->CreateUnorderedAccessView(deadListBuffer, &deadUAVDesc, &particleDeadUAV);

		// Done
		deadListBuffer->Release();
	}

	// Draw List
	{
		// Buffer
		ID3D11Buffer* drawListBuffer;
		D3D11_BUFFER_DESC drawDesc = {};
		drawDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		drawDesc.ByteWidth = sizeof(ParticleSort) * maxParticles;
		drawDesc.CPUAccessFlags = 0;
		drawDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		drawDesc.StructureByteStride = sizeof(ParticleSort);
		drawDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateBuffer(&drawDesc, 0, &drawListBuffer);

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC drawUAVDesc = {};
		drawUAVDesc.Format = DXGI_FORMAT_UNKNOWN; // Needed for RW structured buffers
		drawUAVDesc.Buffer.FirstElement = 0;
		drawUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER; // IncrementCounter() in HLSL
		drawUAVDesc.Buffer.NumElements = maxParticles;
		drawUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		device->CreateUnorderedAccessView(drawListBuffer, &drawUAVDesc, &particleDrawUAV);
		
		// SRV (for indexing in VS)
		D3D11_SHADER_RESOURCE_VIEW_DESC drawSRVDesc = {};
		drawSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		drawSRVDesc.Buffer.FirstElement = 0;
		drawSRVDesc.Buffer.NumElements = maxParticles;
		// Don't actually set these!  They're union'd with above data, so 
		// it will just overwrite correct values with incorrect values
		//drawSRVDesc.Buffer.ElementOffset = 0;
		//drawSRVDesc.Buffer.ElementWidth = sizeof(ParticleSort);
		drawSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		device->CreateShaderResourceView(drawListBuffer, &drawSRVDesc, &particleDrawSRV);

		// Done
		drawListBuffer->Release();
	}

	// DRAW ARGS ================
	{
		// Buffer
		D3D11_BUFFER_DESC argsDesc = {};
		argsDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		argsDesc.ByteWidth = sizeof(unsigned int) * 5; // Need 5 if using an index buffer!
		argsDesc.CPUAccessFlags = 0;
		argsDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		argsDesc.StructureByteStride = 0;
		argsDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateBuffer(&argsDesc, 0, &drawArgsBuffer);

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC argsUAVDesc = {};
		argsUAVDesc.Format = DXGI_FORMAT_R32_UINT; // Actually UINT's in here!
		argsUAVDesc.Buffer.FirstElement = 0;
		argsUAVDesc.Buffer.Flags = 0;  // Nothing special
		argsUAVDesc.Buffer.NumElements = 5; // Need 5 if using an index buffer
		argsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		device->CreateUnorderedAccessView(drawArgsBuffer, &argsUAVDesc, &drawArgsUAV);

		// Must keep buffer ref for indirect draw!
	}

	// Launch the dead list init shader
	deadListInitCS->SetInt("MaxParticles", maxParticles);
	deadListInitCS->SetUnorderedAccessView("DeadList", particleDeadUAV);
	deadListInitCS->SetShader();
	deadListInitCS->CopyAllBufferData();
	deadListInitCS->DispatchByThreads(maxParticles, 1, 1);

	// Additive blend state
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &additiveBlend);

	D3D11_DEPTH_STENCIL_DESC depth = {};
	depth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth.DepthFunc = D3D11_COMPARISON_LESS;
	depth.DepthEnable = true;
	device->CreateDepthStencilState(&depth, &depthWriteOff); 
}


Emitter::~Emitter()
{
	particlePoolUAV->Release();
	particleDrawUAV->Release();
	particleDeadUAV->Release();

	particlePoolSRV->Release();
	particleDrawSRV->Release();

	drawArgsBuffer->Release();
	drawArgsUAV->Release();

	indexBuffer->Release();
	additiveBlend->Release();
	depthWriteOff->Release();
}


void Emitter::Update(float dt, float tt)
{
	// Reset UAVs (potential issue with setting the following ones)
	ID3D11UnorderedAccessView* none[8] = {};
	context->CSSetUnorderedAccessViews(0, 8, none, 0);

	// Track time
	emitTimeCounter += dt;
	while(emitTimeCounter >= timeBetweenEmit)
	{
		// How many to emit?
		int emitCount = (int)(emitTimeCounter / timeBetweenEmit);
		
		// Max to emit in a single batch is 65,535
		emitCount = min(emitCount, 65535);
		
		// Adjust time counter
		emitTimeCounter = fmod(emitTimeCounter, timeBetweenEmit);

		XMFLOAT4 pos = XMFLOAT4(6, -22.5, 0, 1);

		// Emit an appropriate amount of particles
		emitCS->SetShader();
		emitCS->SetFloat4("Position", pos);
		emitCS->SetFloat("TotalTime", tt);
		emitCS->SetInt("EmitCount", emitCount);
		emitCS->SetInt("MaxParticles", (int)maxParticles);
		emitCS->SetInt("GridSize", 100);
		emitCS->SetUnorderedAccessView("ParticlePool", particlePoolUAV);
		emitCS->SetUnorderedAccessView("DeadList", particleDeadUAV);
		emitCS->CopyAllBufferData();
		emitCS->DispatchByThreads(emitCount, 1, 1);
	}


	context->CSSetUnorderedAccessViews(0, 8, none, 0);

	// For sphere below
	static float x;
	x = (float)sin(tt) * 10.0f;

	// Update
	updateCS->SetShader();
	updateCS->SetFloat("DT", dt);
	updateCS->SetFloat("TotalTime", tt);
	updateCS->SetFloat("Lifetime", lifetime);
	updateCS->SetInt("MaxParticles", maxParticles);
	updateCS->SetFloat("SphereRadius", 1.0f);
	updateCS->SetFloat3("SpherePosition", XMFLOAT3(x,0,15));
	updateCS->SetUnorderedAccessView("ParticlePool", particlePoolUAV);
	updateCS->SetUnorderedAccessView("DeadList", particleDeadUAV);
	updateCS->SetUnorderedAccessView("DrawList", particleDrawUAV, 0); // Reset counter for update!

	updateCS->CopyAllBufferData();
	updateCS->DispatchByThreads(maxParticles, 1, 1);

	// Binding order issues with next stage, so just reset here
	context->CSSetUnorderedAccessViews(0, 8, none, 0);

	// Get draw data
	copyDrawCountCS->SetShader();
	copyDrawCountCS->SetInt("VertsPerParticle", 6);
	copyDrawCountCS->CopyAllBufferData();
	copyDrawCountCS->SetUnorderedAccessView("DrawArgs", drawArgsUAV);
	copyDrawCountCS->SetUnorderedAccessView("DrawList", particleDrawUAV); // Don't reset counter!!!
	copyDrawCountCS->DispatchByThreads(1, 1, 1);

	// Reset here too
	context->CSSetUnorderedAccessViews(0, 8, none, 0);

}


void Emitter::Draw(Camera* camera, float aspectRatio, float width, float height, bool additive)
{
	if (additive)
	{
		context->OMSetBlendState(additiveBlend, 0, 0xFFFFFFFF);
		context->OMSetDepthStencilState(depthWriteOff, 0);
	}

	// Assuming triangles, set index buffer
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Gotta do it manually! SimpleShader doesn't currently
	// handle structured buffers in Vertex Shaders
	context->VSSetShaderResources(0, 1, &particlePoolSRV);
	context->VSSetShaderResources(1, 1, &particleDrawSRV);

	particleVS->SetShader();
	particleVS->SetMatrix4x4("view", camera->GetViewMatrix());
	particleVS->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	particleVS->SetFloat("aspectRatio", aspectRatio);
	particleVS->CopyAllBufferData();

	particlePS->SetShader();

	// Draw using indirect args
	context->DrawIndexedInstancedIndirect(drawArgsBuffer, 0);

	ID3D11ShaderResourceView* none[16] = {};
	context->VSSetShaderResources(0, 16, none);

	if (additive)
	{
		context->OMSetBlendState(0, 0, 0xFFFFFFFF);
		context->OMSetDepthStencilState(0, 0);
	}
}