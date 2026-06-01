#include <iostream>
#include <raylib.h>

#include <imgui.h>
#include <rlImGui.h>

#include <gameMain.h>

void SetUpImGui();
void DrawImGui(Color &bgColor);

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

		if(!UpdateGame())
		{
			CloseWindow();
		}

		if (showDebugWindow)
		{
			DrawImGui(backgroundColor);
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

void DrawImGui(Color &bgColor)
{
	static int red = 0;
	static int green = 0;
	static int blue = 0;
	static int alpha = 0;

	ImGui::Begin("Debug Window");
	ImGui::Text("Change Background Color");

	ImGui::SliderInt("Red", &red, 0, 255);
	ImGui::SliderInt("Green", &green, 0, 255);
	ImGui::SliderInt("Blue", &blue, 0, 255);
	ImGui::SliderInt("Alpha", &alpha, 0, 255);

	if(ImGui::Button("Apply"))
	{
		bgColor.r = red;
		bgColor.g = green;
		bgColor.b = blue;
		bgColor.a = alpha;
	}

	ImGui::End();
}