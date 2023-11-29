#include "RayGE/Private/Launcher.h"
#include "Platform/FileSystem.h"

// REMOVE ME
#include <stdio.h>
#include <stdlib.h>

RAYGE_ENGINE_PUBLIC(int32_t) RayGE_Launcher_Run(const RayGE_LaunchParams* params)
{
	if ( !RAYGE_INTERFACE_VERIFY(params, RAYGE_LAUNCHPARAMS_VERSION) || params->argc < 1 )
	{
		return RAYGE_LAUNCH_FAILED_INVALID_PARAMS;
	}

	Platform_SetExecutableFromArgV0(params->argv[0]);

	Platform_Path path = Platform_AllocatePath("api/include/RayGE");
	struct Platform_DirectoryListing* listing = Platform_GetDirectoryListing(path);
	Platform_FreePath(path);

	if ( listing )
	{
		// REMOVE ME
		printf("%s\n", Platform_DirectoryListing_GetDirectoryPath(listing));

		for ( struct Platform_DirectoryEntry* entry = Platform_DirectoryListing_GetFirstEntry(listing); entry;
			entry = Platform_DirectoryListing_GetNextEntry(entry) )
		{
			// REMOVE ME
			printf("  %s%s\n", Platform_DirectoryListing_GetNodeName(entry), Platform_DirectoryListing_GetNodeType(entry) == PLATFORM_NODE_DIRECTORY ? "/" : "");
		}

		Platform_FreeDirectoryListing(listing);
	}

	// TODO
	return 1;
}
