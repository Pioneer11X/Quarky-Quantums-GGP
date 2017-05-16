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
#include "Box2D\Box2D.h"
#include "MapLoader.h"
#include "Renderer.h"

class Camera;

class Game 
	: public DXCore
{
	friend class Renderer;
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

	/// Required for shadows. Used in Renderer
	// Get BackBufferRTV
	ID3D11RenderTargetView* GetBackBufferRTV();

	// Get DepthStencil View
	ID3D11DepthStencilView* GetDSV();

	void InitBox2D();

	// Working -- A Way to Pause Physics to do stuff like Level Transition and Pausing the Game.

	//bool IsPhysicsRunning() {
	//	return PhysicsRunning;
	//}

	//void StartPhysics() {
	//	PhysicsRunning = true;
	//}

	//void PausePhysics() {
	//	PhysicsRunning = false;
	//}

private:

	// Game Instance
	static Game* instance;
	//static b2Vec2 gravity;
	//static b2World world;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* skyVertShader;
	SimplePixelShader* skyPixShader;

	//Texture
	ID3D11ShaderResourceView* borderSRV;
	ID3D11ShaderResourceView* platformSRV;
	ID3D11ShaderResourceView* fadeOutSRV;
	ID3D11ShaderResourceView* fadeInSRV;
	ID3D11ShaderResourceView* goalSRV;
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

	// Spotlight for the player
	SpotLightWrapper* spotLightEntity;

	ControlledEntity* playerChar;

	// Box2D Shit.

	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	b2Body* groundBody;
	b2Body* playerBody;

	bool PhysicsRunning = false;

	//Map Loader
	MapLoader* mapLoader;
	

};

