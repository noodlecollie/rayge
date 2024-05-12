# TODO: Make all of these submodules
add_subdirectory(cJSON-1.7.17)
add_subdirectory(raylib-5.0)
add_subdirectory(cwalk-1.2.7)
add_subdirectory(wzl-cutl)
add_subdirectory(cargs-1.1.0)
add_subdirectory(debugbreak-1.0)
add_subdirectory(nuklear-4.12.0)

if(BUILD_TESTING)
	add_subdirectory(catch2-3.5.4)
endif()

if(WIN32)
	add_subdirectory(pthreads-win32)
endif()
