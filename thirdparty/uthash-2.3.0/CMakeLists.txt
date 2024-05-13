add_library(ut-utils INTERFACE
	uthash-2.3.0/src/utarray.h
	uthash-2.3.0/src/uthash.h
	uthash-2.3.0/src/utlist.h
	uthash-2.3.0/src/utringbuffer.h
	uthash-2.3.0/src/utstack.h
	uthash-2.3.0/src/utstring.h
)

target_include_directories(ut-utils INTERFACE
	uthash-2.3.0/src
)
