#include "Identity/Identity.h"
#include "Identity/VCS.h"
#include "Debugging.h"

#if RAYGE_DEBUG()
#define DEBUG_MODE_STRING "Debug"
#else
#define DEBUG_MODE_STRING "Release"
#endif

#ifdef VCS_COMMIT_ID
#define COMMIT_ID_STRING VCS_COMMIT_ID
#else
#define COMMIT_ID_STRING "notset"
#endif

#ifdef VCS_TAG_ID
#define TAG_ID_STRING VCS_TAG_ID
#else
#define TAG_ID_STRING "notset"
#endif

const char* Identity_GetDebugModeString(void)
{
	return DEBUG_MODE_STRING;
}

const char* Identity_GetCommitID(void)
{
	return COMMIT_ID_STRING;
}

const char* Identity_GetTagID(void)
{
	return TAG_ID_STRING;
}

const char* Identity_GetBuildDescription(void)
{
	return "Build: " DEBUG_MODE_STRING "; Commit: " COMMIT_ID_STRING "; Tag: \"" TAG_ID_STRING "\"";
}
