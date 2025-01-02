# These files are just copied into this directory,
# because until https://github.com/raylib-extras/rlImGui/pull/84
# is in, we need to fix their code for them.

project(rlImGui LANGUAGES CXX)

add_library(rlImGui STATIC
	imgui_impl_raylib.h
	rlImGui.cpp
	rlImGui.h
	rlImGuiColors.h
)

target_include_directories(rlImGui PUBLIC .)

target_link_libraries(rlImGui
	PUBLIC
	raylib
	cimgui

	PRIVATE
	$<$<PLATFORM_ID:Linux>:pthread>
	$<$<PLATFORM_ID:Linux>:m>
	$<$<PLATFORM_ID:Linux>:dl>
	$<$<PLATFORM_ID:Linux>:rt>
	$<$<PLATFORM_ID:Linux>:X11>
	$<$<PLATFORM_ID:Windows>:winmm>
	$<$<PLATFORM_ID:Windows>:gdi32>
)

target_compile_definitions(rlImGui
	PUBLIC
	NO_FONT_AWESOME
	CIMGUI_DEFINE_ENUMS_AND_STRUCTS
	$<$<PLATFORM_ID:Windows>:_WIN32>

	PRIVATE
	PLATFORM_DESKTOP
	${RAYGE_GRAPHICS_TYPE}
	$<$<PLATFORM_ID:Linux>:_GLFW_X11>
	$<$<PLATFORM_ID:Linux>:_GNU_SOURCE>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_definitions(rlImGui
		PRIVATE
		_WINSOCK_DEPRECATED_NO_WARNINGS
		_CRT_SECURE_NO_WARNINGS
	)
endif()
