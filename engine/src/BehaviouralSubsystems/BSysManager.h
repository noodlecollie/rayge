#pragma once

#include <stdint.h>

typedef enum
{
	// Deserialisation of state, from a save file, network stream, etc.
	BSYS_STAGE_DESERIALISATION,

	// Running game logic, based on the state and the input.
	BSYS_STAGE_LOGIC,

	// Runnning simulations (eg. physics, particle effects) which
	// do not require bespoke logic.
	BSYS_STAGE_SIMULATION,

	// Rendering the results to the screen.
	BSYS_STAGE_RENDERING,

	// Serialising state to save file, network stream, etc.
	BSYS_STAGE_SERIALISATION,
} BSys_Stage;

// For constructing the mask of stages that a BSys wants to act upon.
// Pass the enum constant for the stage into this macro.
// We need to explicitly specify 1ULL so that this becomes a 64-bit shift.
#define BSYS_STAGE_FLAG(stage) (1ULL << (stage))

typedef struct
{
	// To opt into being invoked for a particular stage,
	// add the stage flag to the mask.
	uint64_t stageMask;
	void (*Init)(void);
	void (*ShutDown)(void);
	void (*Invoke)(BSys_Stage);
} BSys_Definition;

void BSysManager_Init(void);
void BSysManager_ShutDown(void);
void BSysManager_Invoke(BSys_Stage stage);
