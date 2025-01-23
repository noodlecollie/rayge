add_executable(${TARGETNAME_LAUNCHER}
	src/Main.c
)

target_link_libraries(${TARGETNAME_LAUNCHER}
	PRIVATE
	${TARGETNAME_ENGINE}
	raylib
	wzl-cutl
)

target_compile_definitions(${TARGETNAME_LAUNCHER}
	PRIVATE
	$<$<BOOL:${RAYGE_ENABLE_LEAK_CHECK}>:RAYGE_ENABLE_LEAK_CHECK>
)
