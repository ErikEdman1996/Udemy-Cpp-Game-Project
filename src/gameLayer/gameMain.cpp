#include <raylib.h>
#include <iostream>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>

struct GameData
{
}gameData;

AssetManager assetManager;

bool InitGame()
{
	assetManager.LoadAll();
	return true;
}

bool UpdateGame()
{
	float deltaTime{ GetFrameTime() };

	if (deltaTime > 1.0f / 5)
	{
		deltaTime = 1.0f / .5f;
	}

	DrawTexturePro(assetManager.dirt, { 0, 0, (float)assetManager.dirt.width, (float)assetManager.dirt.height },
		{ 50, 50, 100, 100 }, { 0, 0 }, 0.0f, WHITE);

	return true;
}

void CloseGame()
{
	std::cout << "\n\nCLOSING GAME!!!!!!!!!!!!!\n\n";
}
