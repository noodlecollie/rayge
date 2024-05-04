#pragma once

// The logging subsystem is excluded from this process.
// It is expected to be initialised and shut down manually,
// since it may need to be used in the early and late stages
// of the engine's lifetime.

void SubsystemManager_InitAll(void);
void SubsystemManager_ShutDownAll(void);
