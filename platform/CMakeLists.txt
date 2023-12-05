add_library(${TARGETNAME_PLATFORM} STATIC
	include/Platform/FileSystem.h
	include/Platform/FileSystemNative.h
	include/Platform/Library.h

	src/Common/FileSystemCommon.h
	src/Common/FileSystemCommon.c

	$<$<PLATFORM_ID:Linux>:src/Linux/FileSystem.c>

	$<$<PLATFORM_ID:Windows>:src/Windows/FileSystem.c>
)

target_link_libraries(${TARGETNAME_PLATFORM}
	PRIVATE
	cwalk
	wzl-cutl
	$<$<PLATFORM_ID:Linux>::dl>
)

target_include_directories(${TARGETNAME_PLATFORM}
	PUBLIC
	include

	PRIVATE
	src
)
