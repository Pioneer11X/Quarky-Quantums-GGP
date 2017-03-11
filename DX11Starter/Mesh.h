#pragma once

#include <DirectXMath.h>
#include "DXCore.h"
#include "Vertex.h"

class Mesh
{
public:
	Mesh();
	Mesh(Vertex* vertices, int indicesInVertexBuffer, int* indices, int indicesInIndexBuffer, ID3D11Device* device);
	Mesh(std::string parameter, ID3D11Device* device);
	~Mesh();
	ID3D11Buffer* GetVertexBuffer() const;
	ID3D11Buffer* GetIndexBuffer() const;
	int GetIndexCount() const;

private:
	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Initialize VertexBuffer
	void InitializeVertexBuffer(Vertex* vertices, int indicesInVertexBuffer, ID3D11Device* device);
	
	// Initialize IndexBuffer
	void InitializeIndexBuffer(UINT* indices, int indicesInIndexBuffer, ID3D11Device* device);

	//The number of indices in Mesh's Index Buffer
	int indexCount;


};

