add_subdirectory(cJSON)
add_subdirectory(raylib-5.0)
add_subdirectory(cwalk-1.2.7)
add_subdirectory(wzl-cutl)
add_subdirectory(cargs-1.0.3)

if(WIN32)
	add_subdirectory(pthreads-win32)
endif()
