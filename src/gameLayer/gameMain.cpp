#include <raylib.h>
#include <iostream>
#include "gameMain.h"

struct GameData
{
	float positionX{ 100.0f };
	float positionY{ 100.0f };
	Color playerColor{ RED };
}gameData;

bool InitGame()
{
	return true;
}

bool UpdateGame()
{
	float deltaTime{ GetFrameTime() };

	if (deltaTime > 1.0f / 5)
	{
		deltaTime = 1.0f / .5f;
	}

	if (IsKeyDown(KEY_A))
	{
		gameData.positionX -= 200.0f * deltaTime;
	}
	if (IsKeyDown(KEY_D))
	{
		gameData.positionX += 200.0f * deltaTime;
	}
	if (IsKeyDown(KEY_W))
	{
		gameData.positionY -= 200.0f * deltaTime;
	}
	if (IsKeyDown(KEY_S))
	{
		gameData.positionY += 200.0f * deltaTime;
	}

	DrawRectangle(gameData.positionX, gameData.positionY, 50.0f, 50.0f, gameData.playerColor);

	return true;
}

void CloseGame()
{
	std::cout << "\n\nCLOSING GAME!!!!!!!!!!!!!\n\n";
}