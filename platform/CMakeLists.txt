add_library(${TARGETNAME_PLATFORM} STATIC
	include/Platform/Library.h

	$<$<BOOL:UNIX>:src/Unix/Library.c>
)

target_link_libraries(${TARGETNAME_PLATFORM}
	PRIVATE
	$<$<BOOL:UNIX>:dl>
)

target_include_directories(${TARGETNAME_PLATFORM}
	PUBLIC
	include

	PRIVATE
	src
)
