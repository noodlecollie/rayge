#include <stdbool.h>
#include "Game/GameWindow.h"
#include "Subsystems/LoggingSubsystem.h"
#include "wzl_cutl/string.h"
#include "raylib.h"

#define WINDOW_MAX_TITLE 32

static Vector2i g_WinDefaultDimensions = { 800, 600 };
static char g_WinDefaultTitle[WINDOW_MAX_TITLE];

void GameWindow_SetDimensions(Vector2i dim)
{
	if ( IsWindowReady() )
	{
		SetWindowSize(dim.x, dim.y);
	}
	else
	{
		g_WinDefaultDimensions = dim;
	}
}

void GameWindow_SetTitle(const char* title)
{
	if ( IsWindowReady() )
	{
		SetWindowTitle(title);
	}
	else
	{
		wzl_strcpy(g_WinDefaultTitle, sizeof(g_WinDefaultTitle), title);
	}
}

void GameWindow_Create(void)
{
	if ( IsWindowReady() )
	{
		return;
	}

	InitWindow(g_WinDefaultDimensions.x, g_WinDefaultDimensions.y, g_WinDefaultTitle);
	SetExitKey(KEY_NULL);

	// TODO: Make this configurable?
	SetTargetFPS(60);
}

void GameWindow_Destroy(void)
{
	if ( !IsWindowReady() )
	{
		return;
	}

	CloseWindow();
}

bool GameWindow_CloseRequested(void)
{
	return IsWindowReady() && WindowShouldClose();
}
