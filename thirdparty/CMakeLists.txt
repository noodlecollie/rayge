add_subdirectory(cJSON)
add_subdirectory(raylib-5.0)
add_subdirectory(cwalk-1.2.7)
add_subdirectory(wzl-cutl)

if(WIN32)
	add_subdirectory(pthreads-win32)
endif()
