#pragma once

#include <stdbool.h>

typedef struct CommandModule_CommandHandle CommandModule_CommandHandle;
typedef void (*CommandModule_Callback)(const char* commandName, void* userData);

void CommandModule_Init(void);
void CommandModule_ShutDown(void);

// Any whitespace at the beginning or end of the name will be trimmed.
const CommandModule_CommandHandle*
CommandModule_AddCommand(const char* name, CommandModule_Callback callback, void* userData);

// Name is not trimmed here - it is assumed that it is accurate to the command being searched for.
const CommandModule_CommandHandle* CommandModule_FindCommand(const char* commandName);
void CommandModule_InvokeCommand(const CommandModule_CommandHandle* command);
