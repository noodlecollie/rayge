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

set(SOURCES
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
	src/EngineSubsystems/ResourceSubsystem.h
	src/EngineSubsystems/ResourceSubsystem.c
	src/EngineSubsystems/SceneSubsystem.h
	src/EngineSubsystems/SceneSubsystem.c
	src/Game/GameData.h
	src/Game/GameData.c
	src/Game/GameLoader.h
	src/Game/GameLoader.c
	src/Hooks/HookManager.h
	src/Hooks/HookManager.c
	src/Identity/Identity.h
	src/Identity/Identity.c
	src/Input/InputBuffer.h
	src/Input/InputBuffer.c
	src/Input/InputBufferKeyboard.h
	src/Input/InputBufferKeyboard.c
	src/Input/KeyboardModifiers.h
	src/Integrations/RaylibCustomAllocFunctions.c
	src/JSON/JSONUtils.h
	src/JSON/JSONUtils.c
	src/Launcher/Launcher.c
	src/Launcher/LaunchParams.h
	src/Launcher/LaunchParams.c
	src/Logging/Logging.h
	src/Logging/Logging.c
	src/MemPool/MemPoolManager.h
	src/MemPool/MemPoolManager.c
	src/PixelWorld/PixelWorld.h
	src/PixelWorld/PixelWorld.c
	src/Resources/PixelWorldResources.h
	src/Resources/PixelWorldResources.c
	src/Resources/RenderablePrimitives.h
	src/Resources/RenderablePrimitives.c
	src/Resources/ResourceDomains.h
	src/Resources/ResourceHandleUtils.h
	src/Resources/ResourceHandleUtils.c
	src/Resources/ResourceList.h
	src/Resources/ResourceList.c
	src/Resources/ResourceListUtils.h
	src/Resources/ResourceListUtils.c
	src/Resources/ResourcesAPI.h
	src/Resources/ResourcesAPI.c
	src/Resources/TextureResources.h
	src/Resources/TextureResources.c
	src/Scene/Component.h
	src/Scene/Component.c
	src/Scene/Entity.h
	src/Scene/Entity.c
	src/Scene/Scene.h
	src/Scene/Scene.c
	src/Scene/SceneAPI.h
	src/Scene/SceneAPI.c
	src/Utils/StringUtils.h
	src/Utils/StringUtils.c
	src/Utils/Utils.h
	src/UTUtils/UTHash_Pooled.h
	src/Conversions.h
	src/Debugging.h
	src/Debugging.c
	src/Headless.h

	# Testing
	src/Testing/Testing.h
	$<$<BOOL:${BUILD_TESTING}>:src/Testing/AngleTests.h>
	$<$<BOOL:${BUILD_TESTING}>:src/Testing/AngleTests.c>
	$<$<BOOL:${BUILD_TESTING}>:src/Testing/Testing.c>
)

set(NON_HEADLESS_SOURCES
	src/Non-Headless/EngineSubsystems/RendererSubsystem.h
	src/Non-Headless/EngineSubsystems/RendererSubsystem.c
	src/Non-Headless/EngineSubsystems/UISubsystem.h
	src/Non-Headless/EngineSubsystems/UISubsystem.c
	src/Non-Headless/Hooks/MenuHooks.h
	src/Non-Headless/Hooks/MenuHooks.c
	src/Non-Headless/Integrations/ImGuiBackend.h
	src/Non-Headless/Integrations/ImGuiBackend.c
	src/Non-Headless/Rendering/Renderer.h
	src/Non-Headless/Rendering/Renderer.c
	src/Non-Headless/UI/DeveloperConsole.h
	src/Non-Headless/UI/DeveloperConsole.c
	src/Non-Headless/UI/ImGuiDemo.h
	src/Non-Headless/UI/ImGuiDemo.c
	src/Non-Headless/UI/ResourceViewer.h
	src/Non-Headless/UI/ResourceViewer.c
	src/Non-Headless/UI/SceneDebugUI.h
	src/Non-Headless/UI/SceneDebugUI.c
)

target_sources(${TARGETNAME_ENGINE} PRIVATE
	${SOURCES}
	$<$<NOT:$<BOOL:${RAYGE_HEADLESS}>>:${NON_HEADLESS_SOURCES}>
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
	include/RayGE/APIs/Resources.h
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

	$<$<NOT:$<BOOL:${RAYGE_HEADLESS}>>:cimgui>
)

target_compile_definitions(${TARGETNAME_ENGINE}
	PRIVATE
	RAYGE_PRODUCER
	$<$<BOOL:${BUILD_TESTING}>:RAYGE_BUILD_TESTING_FLAG>
	$<$<BOOL:${RAYGE_HEADLESS}>:RAYGE_HEADLESS_FLAG>

	# This is required to be defined on the consumer
	# of cimgui ONLY. If it's defined on cimgui itself,
	# everything breaks.
	$<$<NOT:$<BOOL:${RAYGE_HEADLESS}>>:CIMGUI_DEFINE_ENUMS_AND_STRUCTS>
)

if(RAYGE_ENABLE_LEAK_CHECK)
	target_precompile_headers(${TARGETNAME_ENGINE} PRIVATE ${CMAKE_SOURCE_DIR}/pch/LeakCheckDefs.h)
endif()
