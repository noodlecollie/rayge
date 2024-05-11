add_library(nuklear STATIC
	nuklear-4.12.0/nuklear.h
	include/Nuklear/Nuklear.h
	src/NuklearImpl.c
)

target_include_directories(nuklear PUBLIC
	.
	include
)
