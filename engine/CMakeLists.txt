add_library(${TARGETNAME_ENGINE} SHARED)

set_target_properties(${TARGETNAME_ENGINE} PROPERTIES OUTPUT_NAME_DEBUG "${TARGETNAME_ENGINE}-debug")

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

configure_file(configure/VCS.h.in ${GENERATED_HEADER_DIR}/Identity/VCS.h)

target_sources(${TARGETNAME_ENGINE} PRIVATE
	${GENERATED_HEADER_DIR}/Identity/VCS.h

	include/RayGE/Private/InterfaceUtils.h
	include/RayGE/Private/Launcher.h

	src/BinaryResources/DMMono_Regular.h
	src/BinaryResources/DMMono_Regular.c
	src/Engine/Engine.h
	src/Engine/Engine.c
	src/Engine/EngineAPI.h
	src/Engine/EngineAPI.c
	src/Game/GameData.h
	src/Game/GameData.c
	src/Game/GameLoader.h
	src/Game/GameLoader.c
	src/Identity/Identity.h
	src/Identity/Identity.c
	src/JSON/JSONUtils.h
	src/JSON/JSONUtils.c
	src/Launcher/Launcher.c
	src/Launcher/LaunchParams.h
	src/Launcher/LaunchParams.c
	src/Rendering/Renderer.h
	src/Rendering/Renderer.c
	src/Scene/Component.h
	src/Scene/Component.c
	src/Scene/Entity.h
	src/Scene/Entity.c
	src/Scene/Scene.h
	src/Scene/Scene.c
	src/Scene/SceneAPI.h
	src/Scene/SceneAPI.c
	src/Subsystems/FileSubsystem.h
	src/Subsystems/FileSubsystem.c
	src/Subsystems/LoggingSubsystem.h
	src/Subsystems/LoggingSubsystem.c
	src/Subsystems/MemPoolSubsystem.h
	src/Subsystems/MemPoolSubsystem.c
	src/Subsystems/RendererSubsystem.h
	src/Subsystems/RendererSubsystem.c
	src/Subsystems/SubsystemManager.h
	src/Subsystems/SubsystemManager.c
	src/Subsystems/UISubsystem.h
	src/Subsystems/UISubsystem.c
	src/UI/TestUI.h
	src/UI/TestUI.c
	src/Debugging.h
	src/Debugging.c
	src/Testing.h
	src/Utils.h
)

target_sources(${TARGETNAME_ENGINE}
	PUBLIC FILE_SET HEADERS
	BASE_DIRS include ${GENERATED_HEADER_DIR} ${EXTERNAL_HEADER_DIR}
	FILES
	${EXPORT_HEADER}
	${RAYMATH_HEADER}
	include/RayGE/APIs/Engine.h
	include/RayGE/APIs/Game.h
	include/RayGE/APIs/Logging.h
	include/RayGE/APIs/Scene.h
	include/RayGE/Angles.h
	include/RayGE/InterfaceUtils.h
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
	debugbreak
	nuklear
)

target_compile_definitions(${TARGETNAME_ENGINE}
	PRIVATE
	RAYGE_PRODUCER
	$<$<BOOL:BUILD_TESTING>:RAYGE_BUILD_TESTING_FLAG>
)
