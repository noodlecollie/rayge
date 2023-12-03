add_subdirectory(cJSON)
add_subdirectory(raylib-5.0)

if(WIN32)
	add_subdirectory(pthreads-win32)
endif()
