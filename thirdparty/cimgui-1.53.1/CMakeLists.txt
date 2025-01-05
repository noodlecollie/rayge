# The build script in the directory is a bit lame, so we do it ourselves.
add_library(cimgui STATIC
	cimgui-1.53.1/cimgui.cpp
	cimgui-1.53.1/imgui/imgui.cpp
	cimgui-1.53.1/imgui/imgui_draw.cpp
	cimgui-1.53.1/imgui/imgui_demo.cpp
	cimgui-1.53.1/imgui/imgui_widgets.cpp
	cimgui-1.53.1/imgui/imgui_tables.cpp
	cimgui_assert.h
	cimgui_assert.c
)

target_compile_definitions(cimgui PUBLIC
	"IMGUI_USER_CONFIG=\"cimgui_config.hpp\""
)

target_include_directories(cimgui
	PUBLIC
	.
	cimgui-1.53.1

	PRIVATE
	cimgui-1.53.1/imgui
	imgui_config
)
