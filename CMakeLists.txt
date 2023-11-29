cmake_minimum_required(VERSION 3.21)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(rayge LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)

add_subdirectory(thirdparty)

# These must come after the thirdparty directory:
if(MSVC)
	add_compile_options(
		/W4
		/Wx
	)
else()
	add_compile_options(
		-Wall
		-Wextra
		-pedantic
		-Werror
	)
endif()

set(TARGETNAME_ENGINE rayge-engine)
set(TARGETNAME_LAUNCHER launcher)
set(TARGETNAME_PLATFORM platform)

add_compile_definitions("LIBNAME_ENGINE=\"${TARGETNAME_ENGINE}\"")

add_subdirectory(platform)
add_subdirectory(engine)
add_subdirectory(launcher)
