cmake_minimum_required(VERSION 3.24)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/wzl-cutl/wzl-cutl/cmake_public;${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(rayge LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)

option(CMAKE_COMPILE_WARNING_AS_ERROR "If set, any compiler warnings are treated as errors." TRUE)
option(INSTALL_SANITY_TEST "If set, a separate directory will be created under the install prefix for the sanity test game." FALSE)

# This must go here, so that it is configured before the later options are set up.
add_subdirectory(thirdparty)

include(rayge_definitions)

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

add_compile_definitions(
	LIBNAME_ENGINE="${TARGETNAME_ENGINE}"

	$<$<PLATFORM_ID:Linux>:LIB_PREFIX="lib">
	$<$<PLATFORM_ID:Linux>:LIB_EXTENSION=".so">

	$<$<PLATFORM_ID:Windows>:LIB_PREFIX="">
	$<$<PLATFORM_ID:Windows>:LIB_EXTENSION=".dll">
)

add_subdirectory(engine)
add_subdirectory(launcher)
add_subdirectory(gamelib_sanitytest)

generate_engine_install(${INSTALL_DEST})

if(INSTALL_SANITY_TEST)
	generate_engine_install(${INSTALL_DEST_SANITY_TEST})
endif()
