add_executable(${TARGETNAME_ENGINE_TESTS}
	Main.cpp
)

target_link_libraries(${TARGETNAME_ENGINE_TESTS} PRIVATE
	Catch2::Catch2WithMain
)

catch_discover_tests(${TARGETNAME_ENGINE_TESTS})
