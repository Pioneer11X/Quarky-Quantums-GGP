#pragma once

#include <iostream>
#include <fstream>
#include "Entity.h"
using namespace std;

class MapLoader
{
public:
	MapLoader(ID3D11Device* dev, float objScale, vector<Material*> mats, vector<Mesh*> meshes, b2World* physWorld);
	~MapLoader();
	void LoadLevel(string fileName);
	vector<Entity*> GetLevelEntities();
	inline Entity* GetEndOfLevel() { return endOfLevel; }
	float GetPlayerSpawnLocationX();
	float GetPlayerSpawnLocationY();


private:
	ID3D11Device* device;
	float scale;

	vector<Entity*> levelEntities;
	Entity* endOfLevel; // This is just a reference for easy lookup. Still added to levelEntities and deleted witht he rest of the entities.
	vector<Mesh*> meshObjs;
	vector<Material*> materials;
	b2World* world;
	
	float playerSpawnX;
	float playerSpawnY;

	void CreateEntity(char identifier, int xOffset, int yOffset);
};

