#pragma once
#include <Raylib.h>

struct AssetManager
{
	Texture2D dirt{ 0 };

	void LoadAll();
};