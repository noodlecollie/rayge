cmake_minimum_required(VERSION 3.21)
project(rayge LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)

add_subdirectory(thirdparty)

# These must come after the thirdparty directory:
if(MSVC)
	add_compile_options(
		/W4
	)
else()
	add_compile_options(
		-Wall
		-Wextra
		-pedantic
	)
endif()

add_subdirectory(sanity)
