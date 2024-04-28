cmake_minimum_required(VERSION 3.24)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/wzl-cutl/wzl-cutl/cmake_public")

project(rayge LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_COMPILE_WARNING_AS_ERROR YES)

# This must go here, so that it is configured before the later options are set up.
add_subdirectory(thirdparty)

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

set(TARGETNAME_ENGINE rayge-engine)
set(TARGETNAME_LAUNCHER launcher)
set(TARGETNAME_GAMELIB_SANITYTEST gamelib_sanitytest)
set(INSTALL_DEST rayge)

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
