project(imgui LANGUAGES CXX)

add_library(imgui STATIC
	imgui-1.91.6/imgui_demo.cpp
	imgui-1.91.6/imgui_draw.cpp
	imgui-1.91.6/imgui_internal.h
	imgui-1.91.6/imgui_tables.cpp
	imgui-1.91.6/imgui_widgets.cpp
	imgui-1.91.6/imgui.cpp
	imgui-1.91.6/imgui.h
	imgui-1.91.6/imstb_rectpack.h
	imgui-1.91.6/imstb_textedit.h
	imgui-1.91.6/imstb_truetype.h
)

target_include_directories(imgui PUBLIC
	imgui-1.91.6
	include
)

target_compile_definitions(imgui PUBLIC "IMGUI_USER_CONFIG=\"localimconfig.h\"")
