add_executable(${TARGETNAME_LAUNCHER}
	src/Main.c
)

target_link_libraries(${TARGETNAME_LAUNCHER}
	PRIVATE
	${TARGETNAME_ENGINE}
	raylib
	wzl-cutl
)

install(TARGETS ${TARGETNAME_LAUNCHER}
	DESTINATION ${INSTALL_DEST}
)
