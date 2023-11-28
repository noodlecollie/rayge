#include "raylib.h"

int main(int, char**)
{
	SetTraceLogLevel(LOG_NONE);

	InitWindow(800, 600, "Test");
	SetExitKey(0);
	SetTargetFPS(30);

	bool exitWindow = false;

	while ( !exitWindow )
	{
		exitWindow = WindowShouldClose();

		BeginDrawing();
		ClearBackground((Color){255, 0, 0, 255});
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
