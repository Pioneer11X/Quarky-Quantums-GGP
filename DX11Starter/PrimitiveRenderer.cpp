#include "PrimitiveRenderer.h"

#include <algorithm>
#include <PrimitiveBatch.h>
#include <d3dx11effect.h>

using namespace DirectX;

const XMFLOAT3 PrimitiveRenderer::white = XMFLOAT3(1, 1, 1);
const XMFLOAT3 PrimitiveRenderer::red = XMFLOAT3(1, 0, 0);
const XMFLOAT3 PrimitiveRenderer::green = XMFLOAT3(0, 1, 0);
const XMFLOAT3 PrimitiveRenderer::blue = XMFLOAT3(0, 0, 1);
const XMFLOAT3 PrimitiveRenderer::yellow = XMFLOAT3(1, 1, 0);

void PrimitiveRenderer::Reset()
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE vbRes, ibRes;
	hr = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vbRes);
	assert(S_OK == hr);

	hr = context->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ibRes);
	assert(S_OK == hr);

	pVertices = reinterpret_cast<Vertex*>(vbRes.pData);
	pIndices = reinterpret_cast<uint16_t*>(ibRes.pData);

	curVertex = 0;
	curIndex = 0;
}

void PrimitiveRenderer::Render(const DirectX::XMFLOAT4X4& matView, const DirectX::XMFLOAT4X4& matProj, bool useDepth)
{
	if (0 == curIndex || 0 == curVertex) return;
	assert(nullptr != pVertices || nullptr != pIndices);
	assert(curIndex <= maxIndices && curVertex <= maxVertices);

	context->Unmap(vertexBuffer, 0);
	context->Unmap(indexBuffer, 0);
	pVertices = nullptr;
	pIndices = nullptr;

	XMFLOAT4X4 matrices[2] = { matView, matProj };
	context->UpdateSubresource(constantBuffer, 0, nullptr, matrices, 0, 0);

	ID3D11Buffer* vbs[] = { vertexBuffer };
	UINT stride[] = { sizeof(Vertex) };
	UINT offset[] = { 0 };

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	context->IASetInputLayout(inputLayout);
	context->IASetVertexBuffers(0, 1, vbs, stride, offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	effect->GetTechniqueByIndex(0)->GetPassByIndex((useDepth ? 0 : 1))->Apply(0, context);

	ID3D11Buffer* cbs[] = { constantBuffer };
	context->VSSetConstantBuffers(0, 1, cbs);

	context->DrawIndexed(curIndex, 0, 0);

	Reset();
}

void PrimitiveRenderer::Draw(const BoundingBox & bound, const DirectX::XMFLOAT3& color)
{
	XMMATRIX matWorld = XMMatrixScaling(bound.Extents.x, bound.Extents.y, bound.Extents.z);
	XMVECTOR position = XMLoadFloat3(&bound.Center);
	matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

	DrawCube(matWorld, color);
}

void PrimitiveRenderer::Draw(const BoundingFrustum & bound, const DirectX::XMFLOAT3& color)
{
	XMFLOAT3 corners[BoundingFrustum::CORNER_COUNT];
	bound.GetCorners(corners);

	AddIndex(AddVertex(corners[0], color));
	AddIndex(AddVertex(corners[1], color));
	AddIndex(AddVertex(corners[1], color));
	AddIndex(AddVertex(corners[2], color));
	AddIndex(AddVertex(corners[2], color));
	AddIndex(AddVertex(corners[3], color));
	AddIndex(AddVertex(corners[3], color));
	AddIndex(AddVertex(corners[0], color));

	AddIndex(AddVertex(corners[0], color));
	AddIndex(AddVertex(corners[4], color));
	AddIndex(AddVertex(corners[1], color));
	AddIndex(AddVertex(corners[5], color));
	AddIndex(AddVertex(corners[2], color));
	AddIndex(AddVertex(corners[6], color));
	AddIndex(AddVertex(corners[3], color));
	AddIndex(AddVertex(corners[7], color));

	AddIndex(AddVertex(corners[4], color));
	AddIndex(AddVertex(corners[5], color));
	AddIndex(AddVertex(corners[5], color));
	AddIndex(AddVertex(corners[6], color));
	AddIndex(AddVertex(corners[6], color));
	AddIndex(AddVertex(corners[7], color));
	AddIndex(AddVertex(corners[7], color));
	AddIndex(AddVertex(corners[4], color));

}

void PrimitiveRenderer::Draw(const BoundingOrientedBox & bound, const DirectX::XMFLOAT3& color)
{
	XMMATRIX matWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&bound.Orientation));
	XMMATRIX matScale = XMMatrixScaling(bound.Extents.x, bound.Extents.y, bound.Extents.z);
	matWorld = XMMatrixMultiply(matScale, matWorld);
	XMVECTOR position = XMLoadFloat3(&bound.Center);
	matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

	DrawCube(matWorld, color);
}

void PrimitiveRenderer::Draw(const BoundingSphere & bound, const DirectX::XMFLOAT3& color)
{
	XMVECTOR origin = XMLoadFloat3(&bound.Center);

	const float radius = bound.Radius;

	XMVECTOR xaxis = g_XMIdentityR0 * radius;
	XMVECTOR yaxis = g_XMIdentityR1 * radius;
	XMVECTOR zaxis = g_XMIdentityR2 * radius;

	DrawRing(origin, xaxis, zaxis, color);
	DrawRing(origin, xaxis, yaxis, color);
	DrawRing(origin, yaxis, zaxis, color);
}

void PrimitiveRenderer::DrawLine(const DirectX::XMFLOAT3 & p1, const DirectX::XMFLOAT3 & p2, const DirectX::XMFLOAT3 & color)
{
	XMVECTOR _p1 = XMLoadFloat3(&p1);
	XMVECTOR _p2 = XMLoadFloat3(&p2);
	DrawLine(_p1, _p2, color);
}

void PrimitiveRenderer::DrawCoordinate(const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projMatrix, const DirectX::XMFLOAT3 clipSpacePos, float scale)
{
	XMMATRIX matVP, matVP_Inv;
	matVP = XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix));
	matVP = XMMatrixMultiply(matVP, XMMatrixTranspose(XMLoadFloat4x4(&projMatrix)));
	matVP_Inv = XMMatrixInverse(nullptr, matVP);

	XMVECTOR origin = XMVector3TransformCoord(XMLoadFloat3(&clipSpacePos), matVP_Inv);

	DrawLine(origin, origin + XMVectorSet(scale, 0, 0, 0), red);
	DrawLine(origin, origin + XMVectorSet(0, scale, 0, 0), green);
	DrawLine(origin, origin + XMVectorSet(0, 0, scale, 0), blue);
}

void PrimitiveRenderer::DrawGrid(const DirectX::XMFLOAT3 & xAxis, const DirectX::XMFLOAT3 & yAxis, const DirectX::XMFLOAT3 & origin, uint32_t xdivs, uint32_t ydivs, const DirectX::XMFLOAT3 & color)
{
	XMVECTOR _xAxis = XMLoadFloat3(&xAxis);
	XMVECTOR _yAxis = XMLoadFloat3(&yAxis);
	XMVECTOR _origin = XMLoadFloat3(&origin);
	DrawGrid(_xAxis, _yAxis, _origin, xdivs, ydivs, color);
}

void PrimitiveRenderer::DrawLine(DirectX::FXMVECTOR p1, DirectX::FXMVECTOR p2, const DirectX::XMFLOAT3& color)
{
	XMStoreFloat3(&pVertices[curVertex].position, p1);
	XMStoreFloat3(&pVertices[curVertex + 1].position, p2);

	pVertices[curVertex].color = color;
	pVertices[curVertex + 1].color = color;

	pIndices[curIndex] = curVertex;
	pIndices[curIndex + 1] = curVertex + 1;

	curVertex += 2;
	curIndex += 2;
}

void PrimitiveRenderer::DrawCube(DirectX::CXMMATRIX & matWorld, const DirectX::XMFLOAT3& color)
{
	static const XMVECTORF32 s_verts[8] =
	{
		{ -1.f, -1.f, -1.f, 0.f },
		{ 1.f, -1.f, -1.f, 0.f },
		{ 1.f, -1.f,  1.f, 0.f },
		{ -1.f, -1.f,  1.f, 0.f },
		{ -1.f,  1.f, -1.f, 0.f },
		{ 1.f,  1.f, -1.f, 0.f },
		{ 1.f,  1.f,  1.f, 0.f },
		{ -1.f,  1.f,  1.f, 0.f }
	};

	static const WORD s_indices[] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	for (size_t i = 0; i < 8; ++i)
	{
		XMVECTOR v = XMVector3Transform(s_verts[i], matWorld);
		XMStoreFloat3(&pVertices[curVertex + i].position, v);
		pVertices[curVertex + i].color = color;
	}

	for (size_t i = 0; i < 24; i++)
	{
		pIndices[curIndex + i] = curVertex + s_indices[i];
	}

	curVertex += 8;
	curIndex += 24;
}

void PrimitiveRenderer::DrawRing(FXMVECTOR origin, FXMVECTOR majorAxis, FXMVECTOR minorAxis, const DirectX::XMFLOAT3& color)
{
	static const size_t c_ringSegments = 32;

	XMFLOAT3 verts[c_ringSegments + 1];

	FLOAT fAngleDelta = XM_2PI / float(c_ringSegments);
	// Instead of calling cos/sin for each segment we calculate
	// the sign of the angle delta and then incrementally calculate sin
	// and cosine from then on.
	XMVECTOR cosDelta = XMVectorReplicate(cosf(fAngleDelta));
	XMVECTOR sinDelta = XMVectorReplicate(sinf(fAngleDelta));
	XMVECTOR incrementalSin = XMVectorZero();
	static const XMVECTORF32 s_initialCos =
	{
		1.f, 1.f, 1.f, 1.f
	};
	XMVECTOR incrementalCos = s_initialCos.v;
	for (size_t i = 0; i < c_ringSegments; i++)
	{
		XMVECTOR pos = XMVectorMultiplyAdd(majorAxis, incrementalCos, origin);
		pos = XMVectorMultiplyAdd(minorAxis, incrementalSin, pos);
		XMStoreFloat3(&verts[i], pos);
		//XMStoreFloat4(&verts[i].color, color);
		// Standard formula to rotate a vector.
		XMVECTOR newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
		XMVECTOR newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
		incrementalCos = newCos;
		incrementalSin = newSin;
	}
	verts[c_ringSegments] = verts[0];

	for (uint32_t i = 0; i < c_ringSegments; i++)
	{
		pVertices[curVertex + i] = Vertex{ verts[i], color };
		pIndices[curIndex + i * 2] = curVertex + i;
		pIndices[curIndex + i * 2 + 1] = curVertex + (i + 1) % c_ringSegments;
	}

	curVertex += c_ringSegments;
	curIndex += c_ringSegments * 2;
}

void PrimitiveRenderer::DrawGrid(DirectX::FXMVECTOR xAxis, DirectX::FXMVECTOR yAxis, DirectX::FXMVECTOR origin, uint32_t xdivs, uint32_t ydivs, const DirectX::XMFLOAT3 & color)
{
	xdivs = std::max<uint32_t>(1u, xdivs);
	ydivs = std::max<uint32_t>(1u, ydivs);

	for (size_t i = 0; i <= xdivs; ++i)
	{
		float percent = float(i) / float(xdivs);
		percent = (percent * 2.f) - 1.f;
		XMVECTOR scale = XMVectorScale(xAxis, percent);
		scale = XMVectorAdd(scale, origin);

		DrawLine(
			XMVectorSubtract(scale, yAxis),
			XMVectorAdd(scale, yAxis),
			color);
	}

	for (size_t i = 0; i <= ydivs; i++)
	{
		FLOAT percent = float(i) / float(ydivs);
		percent = (percent * 2.f) - 1.f;
		XMVECTOR scale = XMVectorScale(yAxis, percent);
		scale = XMVectorAdd(scale, origin);

		DrawLine(
			XMVectorSubtract(scale, xAxis),
			XMVectorAdd(scale, xAxis),
			color);
	}
}

void PrimitiveRenderer::Init(ID3D11Device * device, ID3D11DeviceContext * context)
{
	maxVertices = 4096;
	maxIndices = 4096;

	this->device = device;
	this->context = context;

	CD3D11_BUFFER_DESC vbDesc(maxVertices * sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER,
		D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	CD3D11_BUFFER_DESC ibDesc(maxIndices * sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER,
		D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	CD3D11_BUFFER_DESC cbDesc(sizeof(float) * 32, D3D11_BIND_CONSTANT_BUFFER);

	HRESULT hr = S_OK;
	hr = device->CreateBuffer(&vbDesc, nullptr, &vertexBuffer);
	assert(S_OK == hr);

	hr = device->CreateBuffer(&ibDesc, nullptr, &indexBuffer);
	assert(S_OK == hr);

	hr = device->CreateBuffer(&cbDesc, nullptr, &constantBuffer);
	assert(S_OK == hr);

	hr = D3DX11CreateEffectFromFile(L"Assets/ShaderObjs/x86/PrimitiveRenderer.cso", 0, device, &effect);
	assert(S_OK == hr);

	{
		D3DX11_PASS_SHADER_DESC desc = {};
		hr = effect->GetTechniqueByIndex(0)->GetPassByIndex(0)->GetVertexShaderDesc(&desc);
		assert(S_OK == hr);

		D3DX11_EFFECT_SHADER_DESC shdrDesc = {};
		hr = desc.pShaderVariable[desc.ShaderIndex].GetShaderDesc(0, &shdrDesc);
		assert(S_OK == hr);

		D3D11_INPUT_ELEMENT_DESC elems[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		hr = device->CreateInputLayout(elems, 2, shdrDesc.pBytecode, shdrDesc.BytecodeLength, &inputLayout);
		assert(S_OK == hr);
	}

	Reset();
}

void PrimitiveRenderer::CleanUp()
{
	if (nullptr != pVertices)
	{
		context->Unmap(vertexBuffer, 0);
	}
	if (nullptr != pIndices)
	{
		context->Unmap(indexBuffer, 0);
	}
	effect->Release();
	inputLayout->Release();
	vertexBuffer->Release();
	indexBuffer->Release();
	constantBuffer->Release();
}


