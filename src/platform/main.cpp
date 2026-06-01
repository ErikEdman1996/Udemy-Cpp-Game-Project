#include <iostream>
#include <raylib.h>

#include <imgui.h>
#include <rlImGui.h>

#include <gameMain.h>
#include <blocks.h>

void SetUpImGui();
void DrawImGui(Block::Type &selectedBlock);

int main()
{
#if PRODUCTION_BUILD == 1
	SetTraceLogLevel(LOG_NONE);
#endif

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 450, "raylib [core] example - basic window");
	SetExitKey(KEY_NULL);
	SetTargetFPS(240);

	Color backgroundColor{ BLACK };
	Block::Type selectedBlock = Block::Type::GrassBlock;

	bool showDebugWindow = false;

	rlImGuiSetup(true);

	SetUpImGui();

	if(!InitGame())
	{
		return 0;
	}

	while(!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_TAB))
		{
			showDebugWindow = !showDebugWindow;
		}

		BeginDrawing();
		ClearBackground(backgroundColor);

		rlImGuiBegin();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);

		if(!UpdateGame(selectedBlock))
		{
			CloseWindow();
		}

		if (showDebugWindow)
		{
			DrawImGui(selectedBlock);
		}

		rlImGuiEnd();

		EndDrawing();
	}

	CloseWindow();

	CloseGame();

	rlImGuiShutdown();

	return 0;
}

void SetUpImGui()
{
	ImGuiIO& io{ ImGui::GetIO() };
	io.FontGlobalScale = 2.0f;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void DrawImGui(Block::Type &selectedBlock)
{
	static int selectedBlockIndex = (int)Block::Type::GrassBlock;

	ImGui::Begin("Debug Window");

	ImGui::Text("Change block type");

	ImGui::SliderInt(
		"Block ID",
		&selectedBlockIndex,
		1,
		(int)Block::Type::BLOCKS_COUNT - 1
	);

	selectedBlock = (Block::Type)selectedBlockIndex;

	ImGui::Text("Selected block ID: %d", selectedBlockIndex);

	ImGui::End();
}