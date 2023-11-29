add_executable(${TARGETNAME_LAUNCHER}
	src/Main.c
)

target_link_libraries(${TARGETNAME_LAUNCHER}
	PRIVATE
	${TARGETNAME_ENGINE}
	raylib
)
