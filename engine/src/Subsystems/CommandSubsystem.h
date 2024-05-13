#pragma once

#include <stdbool.h>

typedef void (*CommandSubsystem_Callback)(void);

void CommandSubsystem_Init(void);
void CommandSubsystem_ShutDown(void);

bool CommandSubsystem_AddCommand(const char* name, CommandSubsystem_Callback callback);
CommandSubsystem_Callback CommandSubsystem_GetCallback(const char* commandName);
