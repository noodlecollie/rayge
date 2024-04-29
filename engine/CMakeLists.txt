add_library(${TARGETNAME_ENGINE} SHARED)

include(generate_export_header_template)
include(rayge_install)

set(GENERATED_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_headers")
set(EXPORT_HEADER "${GENERATED_HEADER_DIR}/RayGE/Export.h")

generate_export_header_template(
	TARGET ${TARGETNAME_ENGINE}
	OUTPUT ${EXPORT_HEADER}
	SHARED
	PRODUCER_DEFINE RAYGE_PRODUCER
)

target_sources(${TARGETNAME_ENGINE} PRIVATE
	include/RayGE/Private/CurrentEngineAPI.h
	include/RayGE/Private/InterfaceUtils.h
	include/RayGE/Private/Launcher.h

	src/JSON/JSONUtils.h
	src/JSON/JSONUtils.c
	src/Launcher/Launcher.c
	src/Launcher/LaunchParams.h
	src/Launcher/LaunchParams.c
	src/Subsystems/LoggingSubsystem.h
	src/Subsystems/LoggingSubsystem.c
	src/Subsystems/FileSubsystem.h
	src/Subsystems/FileSubsystem.c
	src/Debugging.h
	src/EngineAPI.h
	src/EngineAPI.c
	src/GameLoader.h
	src/GameLoader.c
)

target_sources(${TARGETNAME_ENGINE}
	PUBLIC FILE_SET HEADERS
	BASE_DIRS include ${GENERATED_HEADER_DIR}
	FILES
	${EXPORT_HEADER}
	include/RayGE/APIs/Engine.h
	include/RayGE/APIs/Logging.h
	include/RayGE/Platform.h
)

target_include_directories(${TARGETNAME_ENGINE}
	PRIVATE
	src
)

target_link_libraries(${TARGETNAME_ENGINE}
	PRIVATE
	wzl-cutl
	cJSON
	raylib
	cwalk
	cargs
)

target_compile_definitions(${TARGETNAME_ENGINE}
	PRIVATE
	RAYGE_PRODUCER
)
