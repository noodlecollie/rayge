add_library(nuklear STATIC
	nuklear-4.12.0/nuklear.h
	include/Nuklear/Nuklear.h
	src/NuklearImpl.c
)

target_include_directories(nuklear PUBLIC
	.
	include
)

target_compile_definitions(nuklear PUBLIC
	NK_INCLUDE_FIXED_TYPES # Ensures no confusion over long vs. long long between compilers
)

target_link_libraries(nuklear PUBLIC
	raylib
)
