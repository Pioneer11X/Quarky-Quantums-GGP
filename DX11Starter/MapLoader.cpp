#include "MapLoader.h"

MapLoader::MapLoader(ID3D11Device* dev, float objScale, vector<Material*> mats, vector<Mesh*> meshes, b2World* physWorld)
{
	device = dev;
	scale = objScale;
	materials = mats;
	meshObjs = meshes;
	world = physWorld;
}


MapLoader::~MapLoader()
{

}

void MapLoader::LoadLevel(string fileName)
{
	ifstream file("./Assets/LevelMaps/" + fileName, ifstream::in);
	string currentLine;

	int yOffset = 0;

	while (getline(file, currentLine)) 
	{
		for (int x = 0; x < currentLine.length(); x++) 
		{
			CreateEntity(currentLine[x], x, 0 - yOffset);
		}
		yOffset++;
	}
}

vector<Entity*> MapLoader::GetLevelEntities()
{
	return levelEntities;
}

float MapLoader::GetPlayerSpawnLocationX()
{
	return playerSpawnX;
}

float MapLoader::GetPlayerSpawnLocationY()
{
	return playerSpawnY;
}

void MapLoader::CreateEntity(char identifier, int xOffset, int yOffset)
{
	switch (identifier) 
	{
		case 'P': //Basic Platform
		{
			Entity* newEntity = new Entity(meshObjs[5], materials[1], 
				xOffset * scale, 
				yOffset * scale, 
				0.0f, world, false, true,
				0.5f * scale, 0.5f * scale, 
				scale, scale, scale);
			newEntity->SetTranslation(xOffset * scale, yOffset * scale, 0.0f);
			levelEntities.push_back(newEntity);
		} break;
		case 'T': //Transparent Platform (Fades in in light)
		{
			Entity* newEntity = new Entity(meshObjs[5], materials[1], 
				xOffset * scale, 
				yOffset * scale, 
				0.0f, world, false, 
				0.5f * scale, 0.5f * scale, 
				scale, scale, scale);
			newEntity->SetTranslation(xOffset * scale, yOffset * scale, 0.0f);
			newEntity->SetAlpha(0.25f);
			levelEntities.push_back(newEntity);
			}break;
		case 'S': //Solid Platform (Fades out in light)
		{
			}break;
		case 'G': //Goal
		{
			}break;
		case 'X': //X marks the spot (where the player spawns)
		{
			playerSpawnX = xOffset * scale;
			playerSpawnY = yOffset * scale;
			}break;
		default: {} break;
	}
}
