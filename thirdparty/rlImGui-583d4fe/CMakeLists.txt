project(rlImGui LANGUAGES CXX)

add_library(rlImGui STATIC
	rlImGui-583d4fe/imgui_impl_raylib.h
	rlImGui-583d4fe/rlImGui.cpp
	rlImGui-583d4fe/rlImGui.h
	rlImGui-583d4fe/rlImGuiColors.h
)

target_include_directories(rlImGui PUBLIC
	rlImGui-583d4fe
)

target_link_libraries(rlImGui
	PUBLIC
	raylib
	imgui

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
