#include "Commands/CommandParser.h"
#include "EngineSubsystems/CommandSubsystem.h"
#include "Logging/Logging.h"

bool CommandParser_ParseAndExecute(const char* commandString)
{
	// TODO: Tokenise string once we have support for arguments.
	const CommandSubsystem_CommandHandle* handle = CommandSubsystem_FindCommand(commandString);

	if ( !handle )
	{
		Logging_PrintLine(RAYGE_LOG_ERROR, "Unknown command: \"%s\"", commandString);
		return false;
	}

	CommandSubsystem_InvokeCommand(handle);
	return true;
}
