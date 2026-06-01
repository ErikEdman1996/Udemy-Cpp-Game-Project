#include "assetManager.h"

void AssetManager::LoadAll()
{
	dirt = LoadTexture(RESOURCES_PATH "dirt.png");
	textureAtlas = LoadTexture(RESOURCES_PATH "texturesWithBackgroundVersion.png");
	frame = LoadTexture(RESOURCES_PATH "frame.png");
	treeAtlas = LoadTexture(RESOURCES_PATH "treetextures.png");
}