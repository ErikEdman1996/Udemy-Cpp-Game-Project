#include <raylib.h>
#include <iostream>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>
#include "gameMap.h"
#include "helpers.h"
#include <raymath.h>
#include "randomStuff.h"
#include "worldGenerator.h"
#include <imgui.h>

struct GameData
{
	GameMap gameMap{ 0 };
	Camera2D camera{ 0 };
}gameData;

AssetManager assetManager;
std::ranlux24_base rng{ std::random_device{}() };

bool InitGame()
{
	assetManager.LoadAll();

	GenerateWorld(gameData.gameMap);

	gameData.camera.target = { 0, 150.0f };
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 50.0f;

	return true;
}

bool UpdateGame(Block::Type selectedBlock)
{
	float deltaTime{ GetFrameTime() };

	if (deltaTime > 1.0f / 5)
	{
		deltaTime = 1.0f / .5f;
	}

	gameData.camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

	ClearBackground(SKYBLUE);

	static float CAMERA_SPEED = 10.0f;

	if (IsKeyDown(KEY_LEFT))
	{
		gameData.camera.target.x -= CAMERA_SPEED * deltaTime;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		gameData.camera.target.x += CAMERA_SPEED * deltaTime;
	}
	if (IsKeyDown(KEY_UP))
	{
		gameData.camera.target.y -= CAMERA_SPEED * deltaTime;
	}
	if (IsKeyDown(KEY_DOWN))
	{
		gameData.camera.target.y += CAMERA_SPEED * deltaTime;
	}

	Vector2 worldMousePos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floorf(worldMousePos.x);
	int blockY = (int)floorf(worldMousePos.y);

	if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))	// Remove block
	{
		auto blockFg = gameData.gameMap.GetBlockSafe(blockX, blockY);

		if(blockFg != nullptr)
		{
			blockFg->type = Block::Type::Air;
		}
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))	// Place block
	{
		if (selectedBlock >= Block::Type::DirtWall)
		{
			auto block = gameData.gameMap.GetBackgroundBlockSafe(blockX, blockY);

			if (block != nullptr)
			{
				block->type = selectedBlock;
				block->variant = GetRandomInt(rng, 0, 3);
			}
		}
		else
		{
			auto block = gameData.gameMap.GetBlockSafe(blockX, blockY);

			if (block != nullptr)
			{
				block->type = selectedBlock;
				block->variant = GetRandomInt(rng, 0, 3);
			}
		}
	}

	BeginMode2D(gameData.camera);

	Vector2 topLeftViewport = GetScreenToWorld2D({ 0, 0 }, gameData.camera);
	Vector2 bottomRightViewport = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, gameData.camera);

	int startX = (int)floorf(topLeftViewport.x - 1);
	int endX = (int)ceilf(bottomRightViewport.x + 1);
	int startY = (int)floorf(topLeftViewport.y - 1);
	int endY = (int)ceilf(bottomRightViewport.y + 1);

	startX = Clamp(startX, 0, gameData.gameMap.width - 1);
	endX = Clamp(endX, 0, gameData.gameMap.width - 1);
	startY = Clamp(startY, 0, gameData.gameMap.height - 1);
	endY = Clamp(endY, 0, gameData.gameMap.height - 1);


	// Drawing background blocks
	for (int y = startY; y < endY; y++)
	{
		for (int x = startX; x < endX; x++)
		{
			auto& block = gameData.gameMap.GetBackgroundBlockUnsafe(x, y);
			if(block.type != Block::Type::Air && block.type >= Block::Type::DirtWall)
			{
				DrawTexturePro(assetManager.textureAtlas,
					GetTextureAltasBlock((int)block.type, block.variant, 32, 32),
					{ (float)x, (float)y, 1, 1 },
					{ 0, 0 },
					0.0f,
					WHITE);
			}
		}
	}

	// Drawing foreground blocks
	for (int y = startY; y < endY; y++)
	{
		for (int x = startX; x < endX; x++)
		{
			auto& block = gameData.gameMap.GetBlockUnsafe(x, y);

			if(block.type != Block::Type::Air && block.type != Block::Type::WoodLog)
			{
				DrawTexturePro(assetManager.textureAtlas,
					GetTextureAltasBlock((int)block.type, block.variant, 32, 32),
					{ (float)x, (float)y, 1, 1 },
					{ 0, 0 },
					0.0f,
					WHITE);
			}
			else if(block.type == Block::Type::WoodLog)
			{
				Block* blockAboveCurrent = gameData.gameMap.GetBlockSafe(x, y - 1);
				Block* blockBelowCurrent = gameData.gameMap.GetBlockSafe(x, y + 1);
				Block* blockLeftCurrent = gameData.gameMap.GetBlockSafe(x - 1, y);
				Block* blockRightCurrent = gameData.gameMap.GetBlockSafe(x + 1, y);

				bool hasLogAbove = blockAboveCurrent != nullptr && blockAboveCurrent->type == Block::Type::WoodLog;
				bool hasLogBelow = blockBelowCurrent != nullptr && blockBelowCurrent->type == Block::Type::WoodLog;
				bool hasLeavesAbove = blockAboveCurrent != nullptr && blockAboveCurrent->type == Block::Type::Leaves;
				bool hasLeavesRight = blockRightCurrent != nullptr && blockRightCurrent->type == Block::Type::Leaves;
				bool hasLeavesLeft = blockLeftCurrent != nullptr && blockLeftCurrent->type == Block::Type::Leaves;

				int treeColumn = 0;
				int treeRow = abs((x * 31 + y * 17) % 4);

				if (!hasLogAbove && !hasLogBelow && !hasLeavesAbove)
				{
					treeColumn = 7; // single log, no leaves above
				}
				else if (!hasLogAbove && hasLeavesAbove)
				{
					treeColumn = 5; // leaves above piece
				}
				else if (!hasLogAbove)
				{
					treeColumn = 6; // exposed top piece
				}
				else if (!hasLogBelow)
				{
					treeColumn = 4; // base piece
				}
				else if (hasLeavesLeft && hasLeavesRight)
				{
					treeColumn = 1;
				}
				else if (hasLeavesLeft)
				{
					treeColumn = 3; // or 2 if your atlas is swapped
				}
				else if (hasLeavesRight)
				{
					treeColumn = 2; // or 3 if your atlas is swapped
				}
				else
				{
					treeColumn = 0;
				}

				DrawTexturePro(assetManager.treeAtlas,
					GetTextureAltasBlock(treeColumn, treeRow, 32, 32),
					{ (float)x, (float)y, 1, 1 },
					{ 0, 0 },
					0.0f,
					WHITE);
			}
		}
	}

	DrawTexturePro(assetManager.frame,
		{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height },
		{ (float)blockX, (float)blockY, 1, 1 },
		{ 0,0 },
		0.0f,
		WHITE);

	EndMode2D();

	ImGui::Begin("Game Control");
	ImGui::SliderFloat("Camera Zoom", &gameData.camera.zoom, 10.0f, 150.0f);
	ImGui::SliderFloat("Camera Speed", &CAMERA_SPEED, 5.0f, 30.0f);

	ImGui::End();

	DrawFPS(10, 10);

	return true;
}

void CloseGame()
{
	std::cout << "\n\nCLOSING GAME!!!!!!!!!!!!!\n\n";
}


