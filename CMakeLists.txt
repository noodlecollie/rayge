cmake_minimum_required(VERSION 3.24)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/wzl-cutl/wzl-cutl/cmake_public;${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(rayge LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_POSITION_INDEPENDENT_CODE YES)

option(CMAKE_COMPILE_WARNING_AS_ERROR "If set, any compiler warnings are treated as errors." TRUE)
option(RAYGE_INSTALL_SANITY_TEST "If set, a separate directory will be created under the install prefix for the sanity test game." FALSE)
option(BUILD_TESTING "If set, builds tests for the engine" OFF)

# This must go here, so that it is configured before the later options are set up.
add_subdirectory(thirdparty)

if(BUILD_TESTING)
	enable_testing()
endif()

include(rayge_definitions)
include(rayge_utilities)

# Not sure of a better way to do this right now.
# Rather than manually duplicating certain third party
# headers that are required in the engine interface,
# we keep paths to them at the root level so that other
# targets can make use of them.
declare_dependent_file("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/raylib-5.0/raylib-5.0/src/raymath.h" RAYMATH_HEADER_PATH)

set(VCS_COMMIT_ID "notset")
set(VCS_TAG_ID "notset")

find_package(Git QUIET)

if(Git_FOUND)
	execute_process(
		COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE VCS_COMMIT_ID
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	execute_process(
		COMMAND "${GIT_EXECUTABLE}" describe --tags
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE VCS_TAG_ID
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

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
	LIBNAME_ENGINE_DEBUG="${TARGETNAME_ENGINE}-debug"

	$<$<PLATFORM_ID:Linux>:LIB_PREFIX="lib">
	$<$<PLATFORM_ID:Linux>:LIB_EXTENSION=".so">

	$<$<PLATFORM_ID:Windows>:LIB_PREFIX="">
	$<$<PLATFORM_ID:Windows>:LIB_EXTENSION=".dll">
)

add_subdirectory(engine)
add_subdirectory(launcher)
add_subdirectory(gamelib_sanitytest)

if(BUILD_TESTING)
add_subdirectory(tests)
endif()

generate_engine_install(${INSTALL_DEST} FALSE)

if(RAYGE_INSTALL_SANITY_TEST)
	generate_engine_install(${INSTALL_DEST_SANITY_TEST} TRUE)
endif()
