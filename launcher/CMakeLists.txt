add_executable(${TARGETNAME_LAUNCHER}
	src/Main.c
)

target_link_libraries(${TARGETNAME_LAUNCHER}
	PRIVATE
	${TARGETNAME_ENGINE}
	raylib
	wzl-cutl
)

if(RAYGE_ENABLE_LEAK_CHECK)
	target_precompile_headers(${TARGETNAME_LAUNCHER} PRIVATE ${CMAKE_SOURCE_DIR}/pch/LeakCheckDefs.h)
endif()
