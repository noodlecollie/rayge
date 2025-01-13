project(rayge_engine LANGUAGES C)

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

	src/BehaviouralSubsystems/BSysManager.h
	src/BehaviouralSubsystems/BSysManager.c
	src/BehaviouralSubsystems/RenderableBSys.h
	src/BehaviouralSubsystems/RenderableBSys.c
	src/BehaviouralSubsystems/SpatialBSys.h
	src/BehaviouralSubsystems/SpatialBSys.c
	src/BinaryResources/DMMono_Regular.h
	src/BinaryResources/DMMono_Regular.c
	src/BinaryResources/OpenSans_Medium.h
	src/BinaryResources/OpenSans_Medium.c
	src/Commands/CommandParser.h
	src/Commands/CommandParser.c
	src/Engine/Engine.h
	src/Engine/Engine.c
	src/Engine/EngineAPI.h
	src/Engine/EngineAPI.c
	src/EngineSubsystems/CommandSubsystem.h
	src/EngineSubsystems/CommandSubsystem.c
	src/EngineSubsystems/FilesystemSubsystem.h
	src/EngineSubsystems/FilesystemSubsystem.c
	src/EngineSubsystems/InputHookSubsystem.h
	src/EngineSubsystems/InputHookSubsystem.c
	src/EngineSubsystems/InputSubsystem.h
	src/EngineSubsystems/InputSubsystem.c
	src/EngineSubsystems/EngineSubsystemManager.h
	src/EngineSubsystems/EngineSubsystemManager.c
	src/EngineSubsystems/RendererSubsystem.h
	src/EngineSubsystems/RendererSubsystem.c
	src/EngineSubsystems/SceneSubsystem.h
	src/EngineSubsystems/SceneSubsystem.c
	src/EngineSubsystems/UISubsystem.h
	src/EngineSubsystems/UISubsystem.c
	src/Game/GameData.h
	src/Game/GameData.c
	src/Game/GameLoader.h
	src/Game/GameLoader.c
	src/Hooks/HookManager.h
	src/Hooks/HookManager.c
	src/Hooks/MenuHooks.h
	src/Hooks/MenuHooks.c
	src/Identity/Identity.h
	src/Identity/Identity.c
	src/Input/InputBuffer.h
	src/Input/InputBuffer.c
	src/Input/InputBufferKeyboard.h
	src/Input/InputBufferKeyboard.c
	src/Input/KeyboardModifiers.h
	src/Integrations/ImGuiBackend.h
	src/Integrations/ImGuiBackend.c
	src/JSON/JSONUtils.h
	src/JSON/JSONUtils.c
	src/Launcher/Launcher.c
	src/Launcher/LaunchParams.h
	src/Launcher/LaunchParams.c
	src/Logging/Logging.h
	src/Logging/Logging.c
	src/MemPool/MemPoolManager.h
	src/MemPool/MemPoolManager.c
	src/Rendering/RenderableAPI.h
	src/Rendering/RenderableAPI.c
	src/Rendering/RenderablePrimitives.h
	src/Rendering/RenderablePrimitives.c
	src/Rendering/Renderer.h
	src/Rendering/Renderer.c
	src/Rendering/TextureResources.h
	src/Rendering/TextureResources.c
	src/ResourceManagement/ResourceHandleUtils.h
	src/ResourceManagement/ResourceHandleUtils.c
	src/Scene/Component.h
	src/Scene/Component.c
	src/Scene/Entity.h
	src/Scene/Entity.c
	src/Scene/Scene.h
	src/Scene/Scene.c
	src/Scene/SceneAPI.h
	src/Scene/SceneAPI.c
	src/UI/DeveloperConsole.h
	src/UI/DeveloperConsole.c
	src/UI/ImGuiDemo.h
	src/UI/ImGuiDemo.c
	src/UI/ResourceViewer.h
	src/UI/ResourceViewer.c
	src/UI/SceneDebugUI.h
	src/UI/SceneDebugUI.c
	src/UTUtils/UTHash_Pooled.h
	src/Conversions.h
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
	include/RayGE/APIs/Renderable.h
	include/RayGE/APIs/Scene.h
	include/RayGE/Angles.h
	include/RayGE/Color.h
	include/RayGE/InterfaceUtils.h
	include/RayGE/Math.h
	include/RayGE/Platform.h
	include/RayGE/ResourceHandle.h
	include/RayGE/SceneTypes.h
)

target_include_directories(${TARGETNAME_ENGINE}
	PRIVATE
	src
)

target_link_libraries(${TARGETNAME_ENGINE}
	PRIVATE
	wzl-cutl
	cjson
	raylib
	cwalk
	cargs
	debugbreak
	ut-utils
	cimgui
)

target_compile_definitions(${TARGETNAME_ENGINE}
	PRIVATE
	RAYGE_PRODUCER
	$<$<BOOL:BUILD_TESTING>:RAYGE_BUILD_TESTING_FLAG>

	# This is required to be defined on the consumer
	# of cimgui ONLY. If it's defined on cimgui itself,
	# everything breaks.
	CIMGUI_DEFINE_ENUMS_AND_STRUCTS
)
