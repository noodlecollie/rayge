add_library(cJSON STATIC
	src/cJSON.c
)

target_include_directories(cJSON PUBLIC include)

if(NOT WIN32)
	target_link_libraries(cJSON PRIVATE m)
endif()
