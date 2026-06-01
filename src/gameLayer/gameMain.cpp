#include <raylib.h>
#include <iostream>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>
#include "gameMap.h"
#include "helpers.h"

struct GameData
{
	GameMap gameMap{ 0 };
	Camera2D camera{ 0 };
}gameData;

AssetManager assetManager;

bool InitGame()
{
	assetManager.LoadAll();

	gameData.gameMap.Create(30, 10);

	for(int y = 0; y < gameData.gameMap.height; y++)
	{
		for(int x = 0; x < gameData.gameMap.width; x++)
		{
			if(y == 0)
			{
				gameData.gameMap.GetBlockUnsafe(x, y).type = Block::Type::GrassBlock;
			}
			else if(y == 1)
			{
				gameData.gameMap.GetBlockUnsafe(x, y).type = Block::Type::Dirt;
			}
			else
			{
				gameData.gameMap.GetBlockUnsafe(x, y).type = Block::Type::Air;
			}
		}
	}

	gameData.camera.target = { 0, 0 };
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 50.0f;

	return true;
}

bool UpdateGame()
{
	float deltaTime{ GetFrameTime() };

	if (deltaTime > 1.0f / 5)
	{
		deltaTime = 1.0f / .5f;
	}

	gameData.camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

	ClearBackground(SKYBLUE);


	if (IsKeyDown(KEY_LEFT))
	{
		gameData.camera.target.x -= 7.f * deltaTime;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		gameData.camera.target.x += 7.f * deltaTime;
	}
	if (IsKeyDown(KEY_UP))
	{
		gameData.camera.target.y -= 7.f * deltaTime;
	}
	if (IsKeyDown(KEY_DOWN))
	{
		gameData.camera.target.y += 7.f * deltaTime;
	}


	BeginMode2D(gameData.camera);

	for (int y = 0; y < gameData.gameMap.height; y++)
	{
		for (int x = 0; x < gameData.gameMap.width; x++)
		{
			auto& block = gameData.gameMap.GetBlockUnsafe(x, y);

			if(block.type != Block::Type::Air)
			{
				DrawTexturePro(assetManager.textureAtlas,
					GetTextureAltasBlock((int)block.type, 0, 32, 32),
					{ (float)x, (float)y, 1, 1 },
					{ 0, 0 },
					0.0f,
					WHITE);
			}
		}
	}

	EndMode2D();

	return true;
}

void CloseGame()
{
	std::cout << "\n\nCLOSING GAME!!!!!!!!!!!!!\n\n";
}
