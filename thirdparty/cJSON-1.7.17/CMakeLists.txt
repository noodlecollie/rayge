# cJSON's CMake file doesn't seem to have the include directory set
# properly on the target, so we just build the files ourselves. >:c
add_library(cjson STATIC
	cJSON-1.7.17/cJSON.h
	cJSON-1.7.17/cJSON.c
)

target_include_directories(cjson PUBLIC cJSON-1.7.17)
