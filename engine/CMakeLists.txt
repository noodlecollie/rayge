add_library(${TARGETNAME_ENGINE} SHARED)

include(generate_export_header_template)
include(rayge_install)

set(GENERATED_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_headers")
set(EXTERNAL_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/external_headers")
set(EXPORT_HEADER "${GENERATED_HEADER_DIR}/RayGE/Export.h")
set(RAYMATH_HEADER_DIR "${EXTERNAL_HEADER_DIR}/RayGE/External")
set(RAYMATH_HEADER "${RAYMATH_HEADER_DIR}/raymath.h")

file(MAKE_DIRECTORY ${RAYMATH_HEADER_DIR})
file(COPY_FILE ${RAYMATH_HEADER_PATH} ${RAYMATH_HEADER} ONLY_IF_DIFFERENT)

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

	src/Game/Game.h
	src/Game/Game.c
	src/Game/GameLoader.h
	src/Game/GameLoader.c
	src/JSON/JSONUtils.h
	src/JSON/JSONUtils.c
	src/Launcher/Launcher.c
	src/Launcher/LaunchParams.h
	src/Launcher/LaunchParams.c
	src/Subsystems/FileSubsystem.h
	src/Subsystems/FileSubsystem.c
	src/Subsystems/LoggingSubsystem.h
	src/Subsystems/LoggingSubsystem.c
	src/Subsystems/MemPoolSubsystem.h
	src/Subsystems/MemPoolSubsystem.c
	src/Debugging.h
	src/EngineAPI.h
	src/EngineAPI.c
)

target_sources(${TARGETNAME_ENGINE}
	PUBLIC FILE_SET HEADERS
	BASE_DIRS include ${GENERATED_HEADER_DIR} ${EXTERNAL_HEADER_DIR}
	FILES
	${EXPORT_HEADER}
	${RAYMATH_HEADER}
	include/RayGE/APIs/Engine.h
	include/RayGE/APIs/Logging.h
	include/RayGE/APIs/Scene.h
	include/RayGE/Math.h
	include/RayGE/Platform.h
	include/RayGE/SceneTypes.h
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
