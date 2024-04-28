include(rayge_install)

add_library(${TARGETNAME_GAMELIB_SANITYTEST} SHARED
	include/GameLib_SanityTest/LibInterface.h
	src/LibInterface.c
)

target_link_libraries(${TARGETNAME_GAMELIB_SANITYTEST}
	PUBLIC
	${TARGETNAME_ENGINE}
)

target_include_directories(${TARGETNAME_GAMELIB_SANITYTEST}
	PUBLIC
	include
	PRIVATE
	src
)
