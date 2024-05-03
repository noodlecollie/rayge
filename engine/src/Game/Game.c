#include "Game/Game.h"
#include "EngineAPI.h"

void Game_Run()
{
	INVOKE_CALLBACK(g_GameLibCallbacks.game.StartUp);

	// TODO

	INVOKE_CALLBACK(g_GameLibCallbacks.game.ShutDown);
}
