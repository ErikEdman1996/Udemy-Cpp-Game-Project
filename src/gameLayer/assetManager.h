#pragma once
#include <Raylib.h>

struct AssetManager
{
	Texture2D dirt{ 0 };
	Texture2D textureAtlas{ 0 };
	Texture2D frame{ 0 };
	Texture2D treeAtlas{ 0 };

	void LoadAll();
};