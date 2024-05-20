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
	src/BinaryResources/OpenSans_Medium.h
	src/BinaryResources/OpenSans_Medium.c
	src/Engine/Engine.h
	src/Engine/Engine.c
	src/Engine/EngineAPI.h
	src/Engine/EngineAPI.c
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
	src/JSON/JSONUtils.h
	src/JSON/JSONUtils.c
	src/Launcher/Launcher.c
	src/Launcher/LaunchParams.h
	src/Launcher/LaunchParams.c
	src/Logging/Logging.h
	src/Logging/Logging.c
	src/Modules/CommandModule.h
	src/Modules/CommandModule.c
	src/Modules/FilesystemModule.h
	src/Modules/FilesystemModule.c
	src/Modules/InputHookModule.h
	src/Modules/InputHookModule.c
	src/Modules/InputModule.h
	src/Modules/InputModule.c
	src/Modules/MemPoolModule.h
	src/Modules/MemPoolModule.c
	src/Modules/ModuleManager.h
	src/Modules/ModuleManager.c
	src/Modules/RendererModule.h
	src/Modules/RendererModule.c
	src/Modules/SceneModule.h
	src/Modules/SceneModule.c
	src/Modules/UIModule.h
	src/Modules/UIModule.c
	src/Rendering/RenderableAPI.h
	src/Rendering/RenderableAPI.c
	src/Rendering/RenderablePrimitives.h
	src/Rendering/RenderablePrimitives.c
	src/Rendering/Renderer.h
	src/Rendering/Renderer.c
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
	src/UI/SceneDebugUI.h
	src/UI/SceneDebugUI.c
	src/UI/TestUI.h
	src/UI/TestUI.c
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
	nuklear
	ut-utils
)

target_compile_definitions(${TARGETNAME_ENGINE}
	PRIVATE
	RAYGE_PRODUCER
	$<$<BOOL:BUILD_TESTING>:RAYGE_BUILD_TESTING_FLAG>
)
