#pragma once

typedef enum RayGE_EngineState
{
	ENGINE_STATE_IDLE = 0,
	ENGINE_STATE_DESERIALISING,
	ENGINE_STATE_PROCESSING_INPUT,
	ENGINE_STATE_PROCESSING_LOGIC,
	ENGINE_STATE_SIMULATING,
	ENGINE_STATE_RENDERING,
	ENGINE_STATE_SERIALISING,
	ENGINE_STATE_INTER_FRAME
} RayGE_EngineState;

void Engine_StartUp(void);
void Engine_ShutDown(void);
void Engine_RunToCompletion(void);

RayGE_EngineState Engine_GetCurrentState(void);
