add_library(debugbreak INTERFACE
	debugbreak-1.0/debugbreak.h
)

target_include_directories(debugbreak INTERFACE debugbreak-1.0)
