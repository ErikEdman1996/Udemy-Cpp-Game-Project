#pragma once
#include <Raylib.h>

struct AssetManager
{
	Texture2D dirt{ 0 };
	Texture2D textureAtlas{ 0 };

	void LoadAll();
};