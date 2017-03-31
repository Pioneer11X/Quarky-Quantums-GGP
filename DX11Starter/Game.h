#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Entity.h"
#include "Renderer.h"
#include "InputManager.h"
#include "Camera.h"
#include "Material.h"
#include "Lights.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "ControlledEntity.h"

class Camera;

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
	std::vector<Entity*> GetEntities();

	// Get Instance
	static Game* Instance();

	// Get Screen Width
	unsigned int GetScreenWidth();

	// Get Screen Height
	unsigned int GetScreenHeight();

	// Get Camera Position
	XMFLOAT3& GetCameraPostion();
private:

	// Game Instance
	static Game* instance;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* skyVertShader;
	SimplePixelShader* skyPixShader;

	//DirectX related Buffers (for SkyBox)
	ID3D11RasterizerState* rasterizerState;
	ID3D11DepthStencilState* depthStencilState;

	//Texture
	ID3D11ShaderResourceView* earthSRV;
	ID3D11ShaderResourceView* metalSRV;
	ID3D11ShaderResourceView* metalRustSRV;
	ID3D11ShaderResourceView* crateSRV;
	ID3D11SamplerState* sampler;

	//SkyBox
	ID3D11ShaderResourceView* skyBox;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	//Array of Mesh Object pointers
	std::vector<Mesh*> meshObjs;

	//Array of Entity pointers
	Entity* skyObject;
	std::vector<Entity*> entities;

	//Array of materials
	Material* skyMaterial;
	std::vector<Material*> materials;

	//Renderer object
	Renderer* renderer;

	//InputManager Object
	InputManager* inputMgr;

	//Camera Object
	Camera* camera;

	//Array of all Directional Lights
	std::vector<DirectionalLight> dirLights;

	//Array of all point lights
	std::vector<PointLight> pointLights;

	// Array of all spot lights
	std::vector<SpotLight> spotLights;

	ControlledEntity* playerChar;
};

