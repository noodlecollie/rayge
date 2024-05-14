#pragma once

#include <stdbool.h>

typedef void (*CommandSubsystem_Callback)(const char* commandName, void* userData);

void CommandSubsystem_Init(void);
void CommandSubsystem_ShutDown(void);

// Any whitespace at the beginning or end of the name will be trimmed.
bool CommandSubsystem_AddCommand(const char* name, CommandSubsystem_Callback callback, void* userData);

// Name is not trimmed here - it is assumed that it is accurate to the command being searched for.
// Returns true if the command was found, or false otherwise.
bool CommandSubsystem_InvokeCommand(const char* commandName);
