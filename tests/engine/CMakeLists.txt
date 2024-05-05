add_executable(${TARGETNAME_ENGINE_TESTS}
	Angles.cpp
)

target_link_libraries(${TARGETNAME_ENGINE_TESTS} PRIVATE
	${TARGETNAME_ENGINE}
	Catch2::Catch2WithMain
)

if(NOT WIN32)
	target_compile_options(${TARGETNAME_ENGINE_TESTS} PRIVATE
		# These are fine in C but make the C++ compiler complain
		-Wno-missing-field-initializers
	)
endif()

catch_discover_tests(${TARGETNAME_ENGINE_TESTS})
