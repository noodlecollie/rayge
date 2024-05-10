add_library(nuklear INTERFACE
	nuklear-4.12.0/nuklear.h
)

target_include_directories(nuklear INTERFACE nuklear-4.12.0)
