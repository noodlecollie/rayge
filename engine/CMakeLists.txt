include(generate_export_header_template)

set(GENERATED_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_headers")
set(EXPORT_HEADER "${GENERATED_HEADER_DIR}/RayGE/Export.gen")
generate_export_header_template(${TARGETNAME_ENGINE} ${EXPORT_HEADER})

add_library(${TARGETNAME_ENGINE} SHARED
	${EXPORT_HEADER}
	include/RayGE/Private/Launcher.h

	src/Launcher.c
)

target_include_directories(${TARGETNAME_ENGINE}
	PUBLIC
	include
	${GENERATED_HEADER_DIR}

	PRIVATE
	src
)
