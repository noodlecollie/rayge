add_library(${TARGETNAME_ENGINE} SHARED)

include(generate_export_header_template)

set(GENERATED_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_headers")
set(EXPORT_HEADER "${GENERATED_HEADER_DIR}/RayGE/Export.h")

generate_export_header_template(
	TARGET ${TARGETNAME_ENGINE}
	OUTPUT ${EXPORT_HEADER}
	SHARED
	PRODUCER_DEFINE RAYGE_PRODUCER
)

target_sources(${TARGETNAME_ENGINE} PRIVATE
	src/GameLoader/GameLoader.h
	src/GameLoader/GameLoader.c
	src/Launcher.c
)

target_sources(${TARGETNAME_ENGINE}
	PUBLIC FILE_SET HEADERS
	BASE_DIRS include ${GENERATED_HEADER_DIR}
	FILES
	${EXPORT_HEADER}
	include/RayGE/Private/Launcher.h
	include/RayGE/InterfaceUtils.h
)

target_include_directories(${TARGETNAME_ENGINE} PRIVATE src)

target_link_libraries(${TARGETNAME_ENGINE}
	PRIVATE
	${TARGETNAME_PLATFORM}
	wzl-cutl
)

target_compile_definitions(${TARGETNAME_ENGINE} PRIVATE RAYGE_PRODUCER)

install(TARGETS ${TARGETNAME_ENGINE}
	ARCHIVE DESTINATION ${INSTALL_DEST}/api/lib
	RUNTIME DESTINATION ${INSTALL_DEST}
	LIBRARY DESTINATION ${INSTALL_DEST}
	FILE_SET HEADERS DESTINATION ${INSTALL_DEST}/api/include
)
