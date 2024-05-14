#pragma once

#include <stdbool.h>

typedef struct CommandSubsystem_CommandHandle CommandSubsystem_CommandHandle;
typedef void (*CommandSubsystem_Callback)(const char* commandName, void* userData);

void CommandSubsystem_Init(void);
void CommandSubsystem_ShutDown(void);

// Any whitespace at the beginning or end of the name will be trimmed.
const CommandSubsystem_CommandHandle*
CommandSubsystem_AddCommand(const char* name, CommandSubsystem_Callback callback, void* userData);

// Name is not trimmed here - it is assumed that it is accurate to the command being searched for.
const CommandSubsystem_CommandHandle* CommandSubsystem_FindCommand(const char* commandName);
void CommandSubsystem_InvokeCommand(const CommandSubsystem_CommandHandle* command);
