add_library(${TARGETNAME_PLATFORM} STATIC
	include/Platform/FileSystem.h
	include/Platform/Library.h
	include/Platform/String.h

	src/Common/FileSystemCommon.h
	src/Common/FileSystemCommon.c
	src/Common/StringCommon.c

	$<$<PLATFORM_ID:Linux>::src/Linux/FileSystem.c>
)

target_link_libraries(${TARGETNAME_PLATFORM}
	PRIVATE
	cwalk
	$<$<PLATFORM_ID:Linux>::dl>
)

target_include_directories(${TARGETNAME_PLATFORM}
	PUBLIC
	include

	PRIVATE
	src
)
