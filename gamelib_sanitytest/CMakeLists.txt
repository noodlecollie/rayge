include(rayge_install)

add_library(${TARGETNAME_GAMELIB_SANITYTEST} SHARED
	src/LibExport.h
	src/SanityTest.c
)

target_link_libraries(${TARGETNAME_GAMELIB_SANITYTEST}
	PUBLIC
	${TARGETNAME_ENGINE}
)

target_include_directories(${TARGETNAME_GAMELIB_SANITYTEST}
	PRIVATE
	src
)

target_compile_definitions(${TARGETNAME_GAMELIB_SANITYTEST}
	PRIVATE
	GAMELIB_SANITYTEST_PRODUCER
)

if(RAYGE_INSTALL_SANITY_TEST)
	install(TARGETS ${TARGETNAME_GAMELIB_SANITYTEST}
		LIBRARY DESTINATION ${INSTALL_DEST_SANITY_TEST}/games/defaultgame
		RUNTIME DESTINATION ${INSTALL_DEST_SANITY_TEST}/games/defaultgame
		ARCHIVE DESTINATION ${INSTALL_DEST_SANITY_TEST}/games/defaultgame
	)

	install(
		FILES
		game.json
		pixelworld.json
		pixelworld.png
		DESTINATION ${INSTALL_DEST_SANITY_TEST}/games/defaultgame
	)
endif()
