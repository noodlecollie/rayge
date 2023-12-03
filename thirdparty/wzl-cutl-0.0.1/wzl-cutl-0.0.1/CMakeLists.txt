# Chosen minimum CMake version chosen based on https://www.scivision.dev/cmake-changelog/
cmake_minimum_required(VERSION 3.24)

# This directory may be added to the CMAKE_MODULE_PATH of any client
# CMake project, to make use of the CMake helper scripts defined here.
# These scripts are also used for building the C library itself.
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_public")

project(wzl-cutl LANGUAGES C CXX)

option(BUILD_SHARED_LIBS "If set, builds a shared library. If not set, builds a static library." FALSE)
option(CMAKE_COMPILE_WARNING_AS_ERROR "If set, any compiler warnings are treated as errors." TRUE)
option(WZL_CUTL_INSTALL "If set, and the project is a top-level project, generates an install target for the library." FALSE)

set(GENERATED_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_headers")
set(GENERATED_EXPORT_HEADER "${GENERATED_HEADER_DIR}/wzl_cutl/libexport.h")

set(COMPILER_IS_MSVC FALSE)
set(COMPILER_IS_CLANG FALSE)
set(COMPILER_IS_GNU FALSE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(COMPILER_IS_MSVC TRUE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	set(COMPILER_IS_CLANG FALSE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(COMPILER_IS_GNU TRUE)
else()
	message(FATAL_ERROR "Compiler \"${CMAKE_CXX_COMPILER_ID}\" is not currently supported")
endif()

include(generate_export_header_template)
generate_export_header_template(
	TARGET wzl-cutl
	OUTPUT ${GENERATED_EXPORT_HEADER}
	PRODUCER_DEFINE WZL_CUTL_PRODUCER
)

add_library(wzl-cutl
	${GENERATED_EXPORT_HEADER}

	include/wzl_cutl/libloader.h
	include/wzl_cutl/memory.h
	include/wzl_cutl/platform_defs.h
	include/wzl_cutl/string.h

	src/wzl_cutl_/common/memory/memory_impl.h
	src/wzl_cutl_/common/memory/memory_impl.c
	src/wzl_cutl_/common/string/string_impl.c

	$<$<PLATFORM_ID:Linux>:src/wzl_cutl_/linux/libloader/libloader_impl.c>

	$<$<PLATFORM_ID:Windows>:src/wzl_cutl_/windows/libloader/libloader_impl.c>
	$<$<PLATFORM_ID:Windows>:src/wzl_cutl_/windows/winapi_min.h>
)

target_compile_definitions(wzl-cutl
	PRIVATE
	WZL_CUTL_PRODUCER
)

if(COMPILER_IS_MSVC)
	target_compile_options(wzl-cutl PRIVATE
		/W4
	)
elseif(COMPILER_IS_CLANG OR COMPILER_IS_GNU)
	target_compile_options(wzl-cutl PRIVATE
		-Wall
		-Wextra
		-pedantic
	)
	target_link_options(wzl-cutl PRIVATE
		-Wl,--no-undefined
	)
else()
	message(FATAL_ERROR "Unsupported compiler")
endif()

target_include_directories(wzl-cutl
	PUBLIC
	include
	${GENERATED_HEADER_DIR}

	PRIVATE
	src
)

if(PROJECT_IS_TOP_LEVEL AND WZL_CUTL_INSTALL)
	install(TARGETS wzl-cutl)
endif()
