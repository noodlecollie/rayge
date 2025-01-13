#include <string.h>
#include "Integrations/ImGuiBackend.h"
#include "Rendering/Renderer.h"
#include "EngineSubsystems/RendererSubsystem.h"
#include "Rendering/TextureResources.h"
#include "cimgui.h"
#include "cimgui_assert.h"
#include "raylib.h"
#include "Debugging.h"

// A slight over-estimate on the size of the enum, just for safety.
// The compiler should catch any compile-time indexes that are
// out of bounds, and we can check ourselves at runtime.
#define NUM_KEY_CODES 400

#define JOYSTICK_DEADZONE 0.2f

typedef struct Data
{
	ImGuiContext* context;

	bool lastFrameFocused;
	bool lastControlPressed;
	bool lastShiftPressed;
	bool lastAltPressed;
	bool lastSuperPressed;

	ImGuiMouseCursor currentMouseCursor;

	int raylibToImGuiKeyMap[NUM_KEY_CODES];
	int imGuiToRaylibCursorMap[ImGuiMouseCursor_COUNT];

	Texture2D fontTexture;
	RayGE_ResourceHandle fontResource;
} Data;

static Data g_Data;
static bool g_Initialised = false;

static void
CImGuiAssertHandler(bool expression, const char* description, const char* file, int line, const char* function)
{
	RayGE_CheckInvariant(true, expression, description, file, line, function, "<ImGui assertion failed>");
}

static bool IsAnyCtrlKeyDown(void)
{
	return IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
}

static bool IsAnyShiftKeyDown(void)
{
	return IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT);
}

static bool IsAnyAltKeyDown(void)
{
	return IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT);
}

static bool IsAnySuperKeyDown(void)
{
	return IsKeyDown(KEY_RIGHT_SUPER) || IsKeyDown(KEY_LEFT_SUPER);
}

static void SetFontTextureFromImage(Data* data, const Image* image)
{
	if ( !RAYGE_IS_NULL_RESOURCE_HANDLE(data->fontResource) )
	{
		TextureResources_UnloadInternalTexture(data->fontResource);
		data->fontResource = RAYGE_NULL_RESOURCE_HANDLE;
		memset(&data->fontTexture, 0, sizeof(data->fontTexture));
	}

	if ( image )
	{
		data->fontResource = TextureResources_LoadInternalTexture("imgui_font", *image);

		TextureResources_Iterator iterator = TextureResources_CreateIterator(data->fontResource);
		data->fontTexture = TextureResourcesIterator_GetTexture(iterator);

		RAYGE_ENSURE(data->fontTexture.id != 0, "Failed to load ImGui font texture!");
	}
}

static void SetUpKeyMap(Data* data)
{
	memset(data->raylibToImGuiKeyMap, ImGuiKey_None, sizeof(data->raylibToImGuiKeyMap));

	data->raylibToImGuiKeyMap[KEY_APOSTROPHE] = ImGuiKey_Apostrophe;
	data->raylibToImGuiKeyMap[KEY_COMMA] = ImGuiKey_Comma;
	data->raylibToImGuiKeyMap[KEY_MINUS] = ImGuiKey_Minus;
	data->raylibToImGuiKeyMap[KEY_PERIOD] = ImGuiKey_Period;
	data->raylibToImGuiKeyMap[KEY_SLASH] = ImGuiKey_Slash;
	data->raylibToImGuiKeyMap[KEY_ZERO] = ImGuiKey_0;
	data->raylibToImGuiKeyMap[KEY_ONE] = ImGuiKey_1;
	data->raylibToImGuiKeyMap[KEY_TWO] = ImGuiKey_2;
	data->raylibToImGuiKeyMap[KEY_THREE] = ImGuiKey_3;
	data->raylibToImGuiKeyMap[KEY_FOUR] = ImGuiKey_4;
	data->raylibToImGuiKeyMap[KEY_FIVE] = ImGuiKey_5;
	data->raylibToImGuiKeyMap[KEY_SIX] = ImGuiKey_6;
	data->raylibToImGuiKeyMap[KEY_SEVEN] = ImGuiKey_7;
	data->raylibToImGuiKeyMap[KEY_EIGHT] = ImGuiKey_8;
	data->raylibToImGuiKeyMap[KEY_NINE] = ImGuiKey_9;
	data->raylibToImGuiKeyMap[KEY_SEMICOLON] = ImGuiKey_Semicolon;
	data->raylibToImGuiKeyMap[KEY_EQUAL] = ImGuiKey_Equal;
	data->raylibToImGuiKeyMap[KEY_A] = ImGuiKey_A;
	data->raylibToImGuiKeyMap[KEY_B] = ImGuiKey_B;
	data->raylibToImGuiKeyMap[KEY_C] = ImGuiKey_C;
	data->raylibToImGuiKeyMap[KEY_D] = ImGuiKey_D;
	data->raylibToImGuiKeyMap[KEY_E] = ImGuiKey_E;
	data->raylibToImGuiKeyMap[KEY_F] = ImGuiKey_F;
	data->raylibToImGuiKeyMap[KEY_G] = ImGuiKey_G;
	data->raylibToImGuiKeyMap[KEY_H] = ImGuiKey_H;
	data->raylibToImGuiKeyMap[KEY_I] = ImGuiKey_I;
	data->raylibToImGuiKeyMap[KEY_J] = ImGuiKey_J;
	data->raylibToImGuiKeyMap[KEY_K] = ImGuiKey_K;
	data->raylibToImGuiKeyMap[KEY_L] = ImGuiKey_L;
	data->raylibToImGuiKeyMap[KEY_M] = ImGuiKey_M;
	data->raylibToImGuiKeyMap[KEY_N] = ImGuiKey_N;
	data->raylibToImGuiKeyMap[KEY_O] = ImGuiKey_O;
	data->raylibToImGuiKeyMap[KEY_P] = ImGuiKey_P;
	data->raylibToImGuiKeyMap[KEY_Q] = ImGuiKey_Q;
	data->raylibToImGuiKeyMap[KEY_R] = ImGuiKey_R;
	data->raylibToImGuiKeyMap[KEY_S] = ImGuiKey_S;
	data->raylibToImGuiKeyMap[KEY_T] = ImGuiKey_T;
	data->raylibToImGuiKeyMap[KEY_U] = ImGuiKey_U;
	data->raylibToImGuiKeyMap[KEY_V] = ImGuiKey_V;
	data->raylibToImGuiKeyMap[KEY_W] = ImGuiKey_W;
	data->raylibToImGuiKeyMap[KEY_X] = ImGuiKey_X;
	data->raylibToImGuiKeyMap[KEY_Y] = ImGuiKey_Y;
	data->raylibToImGuiKeyMap[KEY_Z] = ImGuiKey_Z;
	data->raylibToImGuiKeyMap[KEY_SPACE] = ImGuiKey_Space;
	data->raylibToImGuiKeyMap[KEY_ESCAPE] = ImGuiKey_Escape;
	data->raylibToImGuiKeyMap[KEY_ENTER] = ImGuiKey_Enter;
	data->raylibToImGuiKeyMap[KEY_TAB] = ImGuiKey_Tab;
	data->raylibToImGuiKeyMap[KEY_BACKSPACE] = ImGuiKey_Backspace;
	data->raylibToImGuiKeyMap[KEY_INSERT] = ImGuiKey_Insert;
	data->raylibToImGuiKeyMap[KEY_DELETE] = ImGuiKey_Delete;
	data->raylibToImGuiKeyMap[KEY_RIGHT] = ImGuiKey_RightArrow;
	data->raylibToImGuiKeyMap[KEY_LEFT] = ImGuiKey_LeftArrow;
	data->raylibToImGuiKeyMap[KEY_DOWN] = ImGuiKey_DownArrow;
	data->raylibToImGuiKeyMap[KEY_UP] = ImGuiKey_UpArrow;
	data->raylibToImGuiKeyMap[KEY_PAGE_UP] = ImGuiKey_PageUp;
	data->raylibToImGuiKeyMap[KEY_PAGE_DOWN] = ImGuiKey_PageDown;
	data->raylibToImGuiKeyMap[KEY_HOME] = ImGuiKey_Home;
	data->raylibToImGuiKeyMap[KEY_END] = ImGuiKey_End;
	data->raylibToImGuiKeyMap[KEY_CAPS_LOCK] = ImGuiKey_CapsLock;
	data->raylibToImGuiKeyMap[KEY_SCROLL_LOCK] = ImGuiKey_ScrollLock;
	data->raylibToImGuiKeyMap[KEY_NUM_LOCK] = ImGuiKey_NumLock;
	data->raylibToImGuiKeyMap[KEY_PRINT_SCREEN] = ImGuiKey_PrintScreen;
	data->raylibToImGuiKeyMap[KEY_PAUSE] = ImGuiKey_Pause;
	data->raylibToImGuiKeyMap[KEY_F1] = ImGuiKey_F1;
	data->raylibToImGuiKeyMap[KEY_F2] = ImGuiKey_F2;
	data->raylibToImGuiKeyMap[KEY_F3] = ImGuiKey_F3;
	data->raylibToImGuiKeyMap[KEY_F4] = ImGuiKey_F4;
	data->raylibToImGuiKeyMap[KEY_F5] = ImGuiKey_F5;
	data->raylibToImGuiKeyMap[KEY_F6] = ImGuiKey_F6;
	data->raylibToImGuiKeyMap[KEY_F7] = ImGuiKey_F7;
	data->raylibToImGuiKeyMap[KEY_F8] = ImGuiKey_F8;
	data->raylibToImGuiKeyMap[KEY_F9] = ImGuiKey_F9;
	data->raylibToImGuiKeyMap[KEY_F10] = ImGuiKey_F10;
	data->raylibToImGuiKeyMap[KEY_F11] = ImGuiKey_F11;
	data->raylibToImGuiKeyMap[KEY_F12] = ImGuiKey_F12;
	data->raylibToImGuiKeyMap[KEY_LEFT_SHIFT] = ImGuiKey_LeftShift;
	data->raylibToImGuiKeyMap[KEY_LEFT_CONTROL] = ImGuiKey_LeftCtrl;
	data->raylibToImGuiKeyMap[KEY_LEFT_ALT] = ImGuiKey_LeftAlt;
	data->raylibToImGuiKeyMap[KEY_LEFT_SUPER] = ImGuiKey_LeftSuper;
	data->raylibToImGuiKeyMap[KEY_RIGHT_SHIFT] = ImGuiKey_RightShift;
	data->raylibToImGuiKeyMap[KEY_RIGHT_CONTROL] = ImGuiKey_RightCtrl;
	data->raylibToImGuiKeyMap[KEY_RIGHT_ALT] = ImGuiKey_RightAlt;
	data->raylibToImGuiKeyMap[KEY_RIGHT_SUPER] = ImGuiKey_RightSuper;
	data->raylibToImGuiKeyMap[KEY_KB_MENU] = ImGuiKey_Menu;
	data->raylibToImGuiKeyMap[KEY_LEFT_BRACKET] = ImGuiKey_LeftBracket;
	data->raylibToImGuiKeyMap[KEY_BACKSLASH] = ImGuiKey_Backslash;
	data->raylibToImGuiKeyMap[KEY_RIGHT_BRACKET] = ImGuiKey_RightBracket;
	data->raylibToImGuiKeyMap[KEY_GRAVE] = ImGuiKey_GraveAccent;
	data->raylibToImGuiKeyMap[KEY_KP_0] = ImGuiKey_Keypad0;
	data->raylibToImGuiKeyMap[KEY_KP_1] = ImGuiKey_Keypad1;
	data->raylibToImGuiKeyMap[KEY_KP_2] = ImGuiKey_Keypad2;
	data->raylibToImGuiKeyMap[KEY_KP_3] = ImGuiKey_Keypad3;
	data->raylibToImGuiKeyMap[KEY_KP_4] = ImGuiKey_Keypad4;
	data->raylibToImGuiKeyMap[KEY_KP_5] = ImGuiKey_Keypad5;
	data->raylibToImGuiKeyMap[KEY_KP_6] = ImGuiKey_Keypad6;
	data->raylibToImGuiKeyMap[KEY_KP_7] = ImGuiKey_Keypad7;
	data->raylibToImGuiKeyMap[KEY_KP_8] = ImGuiKey_Keypad8;
	data->raylibToImGuiKeyMap[KEY_KP_9] = ImGuiKey_Keypad9;
	data->raylibToImGuiKeyMap[KEY_KP_DECIMAL] = ImGuiKey_KeypadDecimal;
	data->raylibToImGuiKeyMap[KEY_KP_DIVIDE] = ImGuiKey_KeypadDivide;
	data->raylibToImGuiKeyMap[KEY_KP_MULTIPLY] = ImGuiKey_KeypadMultiply;
	data->raylibToImGuiKeyMap[KEY_KP_SUBTRACT] = ImGuiKey_KeypadSubtract;
	data->raylibToImGuiKeyMap[KEY_KP_ADD] = ImGuiKey_KeypadAdd;
	data->raylibToImGuiKeyMap[KEY_KP_ENTER] = ImGuiKey_KeypadEnter;
	data->raylibToImGuiKeyMap[KEY_KP_EQUAL] = ImGuiKey_KeypadEqual;
}

static void SetUpCursorMap(Data* data)
{
	data->imGuiToRaylibCursorMap[ImGuiMouseCursor_Arrow] = MOUSE_CURSOR_ARROW;
	data->imGuiToRaylibCursorMap[ImGuiMouseCursor_TextInput] = MOUSE_CURSOR_IBEAM;
	data->imGuiToRaylibCursorMap[ImGuiMouseCursor_Hand] = MOUSE_CURSOR_POINTING_HAND;
	data->imGuiToRaylibCursorMap[ImGuiMouseCursor_ResizeAll] = MOUSE_CURSOR_RESIZE_ALL;
	data->imGuiToRaylibCursorMap[ImGuiMouseCursor_ResizeEW] = MOUSE_CURSOR_RESIZE_EW;
	data->imGuiToRaylibCursorMap[ImGuiMouseCursor_ResizeNESW] = MOUSE_CURSOR_RESIZE_NESW;
	data->imGuiToRaylibCursorMap[ImGuiMouseCursor_ResizeNS] = MOUSE_CURSOR_RESIZE_NS;
	data->imGuiToRaylibCursorMap[ImGuiMouseCursor_ResizeNWSE] = MOUSE_CURSOR_RESIZE_NWSE;
	data->imGuiToRaylibCursorMap[ImGuiMouseCursor_NotAllowed] = MOUSE_CURSOR_NOT_ALLOWED;
}

static const char* GetClipTextCallback(ImGuiContext* context)
{
	(void)context;

	return GetClipboardText();
}

static void SetClipTextCallback(ImGuiContext* context, const char* text)
{
	(void)context;

	SetClipboardText(text);
}

static void SetUpBackend(void)
{
	ImGuiIO* io = igGetIO();
	RAYGE_ASSERT_VALID(io);

	io->BackendPlatformName = "imgui_impl_raylib";
	io->BackendFlags |= ImGuiBackendFlags_HasGamepad | ImGuiBackendFlags_HasSetMousePos;

#ifndef PLATFORM_DRM
	io->BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
#endif

	io->MousePos = (ImVec2) {0, 0};

	ImGuiPlatformIO* platformIO = igGetPlatformIO();
	RAYGE_ASSERT_VALID(platformIO);

	platformIO->Platform_SetClipboardTextFn = SetClipTextCallback;
	platformIO->Platform_GetClipboardTextFn = GetClipTextCallback;
	platformIO->Platform_ClipboardUserData = NULL;
}

static void ReloadFonts(Data* data)
{
	ImGuiIO* io = igGetIO();
	RAYGE_ASSERT_VALID(io);

	unsigned char* pixels = NULL;

	int width = 0;
	int height = 0;
	ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &pixels, &width, &height, NULL);

	Image image = GenImageColor(width, height, BLANK);
	memcpy(image.data, pixels, width * height * 4);

	SetFontTextureFromImage(data, &image);
	UnloadImage(image);

	io->Fonts->TexID = data->fontTexture.id;
}

static void HandleKeyEvent(ImGuiIO* io, ImGuiKey key, bool isPressed, bool* lastPressedState)
{
	if ( isPressed != *lastPressedState )
	{
		ImGuiIO_AddKeyEvent(io, key, isPressed);
	}

	*lastPressedState = isPressed;
}

static void HandleMouseEvenet(ImGuiIO* io, int rayMouse, int imGuiMouse)
{
	if ( IsMouseButtonPressed(rayMouse) )
	{
		ImGuiIO_AddMouseButtonEvent(io, imGuiMouse, true);
	}
	else if ( IsMouseButtonReleased(rayMouse) )
	{
		ImGuiIO_AddMouseButtonEvent(io, imGuiMouse, false);
	}
}

void HandleGamepadButtonEvent(ImGuiIO* io, GamepadButton button, ImGuiKey key)
{
	if ( IsGamepadButtonPressed(0, button) )
	{
		ImGuiIO_AddKeyEvent(io, key, true);
	}
	else if ( IsGamepadButtonReleased(0, button) )
	{
		ImGuiIO_AddKeyEvent(io, key, false);
	}
}

void HandleGamepadStickEvent(ImGuiIO* io, GamepadAxis axis, ImGuiKey negKey, ImGuiKey posKey)
{
	const float axisValue = GetGamepadAxisMovement(0, axis);

	ImGuiIO_AddKeyAnalogEvent(
		io,
		negKey,
		axisValue < -JOYSTICK_DEADZONE,
		axisValue < -JOYSTICK_DEADZONE ? -axisValue : 0
	);

	ImGuiIO_AddKeyAnalogEvent(io, posKey, axisValue > JOYSTICK_DEADZONE, axisValue > JOYSTICK_DEADZONE ? axisValue : 0);
}

static void ProcessEvents(Data* data)
{
	ImGuiIO* io = igGetIO();
	RAYGE_ASSERT_VALID(io);

	{
		const bool focused = IsWindowFocused();

		if ( focused != data->lastFrameFocused )
		{
			ImGuiIO_AddFocusEvent(io, focused);
		}

		data->lastFrameFocused = focused;
	}

	HandleKeyEvent(io, ImGuiMod_Ctrl, IsAnyCtrlKeyDown(), &data->lastControlPressed);
	HandleKeyEvent(io, ImGuiMod_Shift, IsAnyShiftKeyDown(), &data->lastShiftPressed);
	HandleKeyEvent(io, ImGuiMod_Alt, IsAnyAltKeyDown(), &data->lastAltPressed);
	HandleKeyEvent(io, ImGuiMod_Super, IsAnySuperKeyDown(), &data->lastSuperPressed);

	for ( int raylibKey = 0; raylibKey < NUM_KEY_CODES; ++raylibKey )
	{
		const int imGuiKey = data->raylibToImGuiKeyMap[raylibKey];

		if ( imGuiKey == 0 )
		{
			continue;
		}

		if ( IsKeyReleased(raylibKey) )
		{
			ImGuiIO_AddKeyEvent(io, imGuiKey, false);
		}
		else if ( IsKeyPressed(raylibKey) )
		{
			ImGuiIO_AddKeyEvent(io, imGuiKey, true);
		}
	}

	if ( io->WantCaptureKeyboard )
	{
		// add the text input in order
		unsigned int pressed = GetCharPressed();

		while ( pressed != 0 )
		{
			ImGuiIO_AddInputCharacter(io, pressed);
			pressed = GetCharPressed();
		}
	}

	if ( !io->WantSetMousePos )
	{
		ImGuiIO_AddMousePosEvent(io, (float)GetMouseX(), (float)GetMouseY());
	}

	HandleMouseEvenet(io, MOUSE_BUTTON_LEFT, ImGuiMouseButton_Left);
	HandleMouseEvenet(io, MOUSE_BUTTON_RIGHT, ImGuiMouseButton_Right);
	HandleMouseEvenet(io, MOUSE_BUTTON_MIDDLE, ImGuiMouseButton_Middle);
	HandleMouseEvenet(io, MOUSE_BUTTON_FORWARD, ImGuiMouseButton_Middle + 1);
	HandleMouseEvenet(io, MOUSE_BUTTON_BACK, ImGuiMouseButton_Middle + 2);

	{
		const Vector2 mouseWheel = GetMouseWheelMoveV();
		ImGuiIO_AddMouseWheelEvent(io, mouseWheel.x, mouseWheel.y);
	}

	if ( (io->ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) && IsGamepadAvailable(0) )
	{
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_LEFT_FACE_UP, ImGuiKey_GamepadDpadUp);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_LEFT_FACE_RIGHT, ImGuiKey_GamepadDpadRight);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_LEFT_FACE_DOWN, ImGuiKey_GamepadDpadDown);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_LEFT_FACE_LEFT, ImGuiKey_GamepadDpadLeft);

		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_RIGHT_FACE_UP, ImGuiKey_GamepadFaceUp);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, ImGuiKey_GamepadFaceLeft);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_RIGHT_FACE_DOWN, ImGuiKey_GamepadFaceDown);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_RIGHT_FACE_LEFT, ImGuiKey_GamepadFaceRight);

		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_LEFT_TRIGGER_1, ImGuiKey_GamepadL1);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_LEFT_TRIGGER_2, ImGuiKey_GamepadL2);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_RIGHT_TRIGGER_1, ImGuiKey_GamepadR1);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_RIGHT_TRIGGER_2, ImGuiKey_GamepadR2);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_LEFT_THUMB, ImGuiKey_GamepadL3);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_RIGHT_THUMB, ImGuiKey_GamepadR3);

		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_MIDDLE_LEFT, ImGuiKey_GamepadStart);
		HandleGamepadButtonEvent(io, GAMEPAD_BUTTON_MIDDLE_RIGHT, ImGuiKey_GamepadBack);

		HandleGamepadStickEvent(io, GAMEPAD_AXIS_LEFT_X, ImGuiKey_GamepadLStickLeft, ImGuiKey_GamepadLStickRight);
		HandleGamepadStickEvent(io, GAMEPAD_AXIS_LEFT_Y, ImGuiKey_GamepadLStickUp, ImGuiKey_GamepadLStickDown);

		HandleGamepadStickEvent(io, GAMEPAD_AXIS_RIGHT_X, ImGuiKey_GamepadRStickLeft, ImGuiKey_GamepadRStickRight);
		HandleGamepadStickEvent(io, GAMEPAD_AXIS_RIGHT_Y, ImGuiKey_GamepadRStickUp, ImGuiKey_GamepadRStickDown);
	}
}

static void UpdateImGuiIOValuesOnFrameBegin(Data* data, float deltaTime)
{
	if ( deltaTime <= 0 )
	{
		deltaTime = 0.001f;
	}

	ImGuiIO* io = igGetIO();
	RAYGE_ASSERT_VALID(io);

	Vector2 resolutionScale = GetWindowScaleDPI();

#ifndef PLATFORM_DRM
	if ( IsWindowFullscreen() )
	{
		int monitor = GetCurrentMonitor();
		io->DisplaySize.x = (float)GetMonitorWidth(monitor);
		io->DisplaySize.y = (float)GetMonitorHeight(monitor);
	}
	else
	{
		io->DisplaySize.x = (float)GetScreenWidth();
		io->DisplaySize.y = (float)GetScreenHeight();
	}

#if !defined(__APPLE__)
	if ( !IsWindowState(FLAG_WINDOW_HIGHDPI) )
	{
		resolutionScale = (Vector2) {1, 1};
	}
#endif
#else
	io.DisplaySize.x = (float)GetScreenWidth();
	io.DisplaySize.y = (float)GetScreenHeight();
#endif

	io->DisplayFramebufferScale = (ImVec2) {resolutionScale.x, resolutionScale.y};
	io->DeltaTime = deltaTime;

	if ( (io->BackendFlags & ImGuiBackendFlags_HasMouseCursors) &&
		 !(io->ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) )
	{
		ImGuiMouseCursor imgui_cursor = igGetMouseCursor();

		if ( imgui_cursor != data->currentMouseCursor || io->MouseDrawCursor )
		{
			data->currentMouseCursor = imgui_cursor;

			if ( io->MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None )
			{
				HideCursor();
			}
			else
			{
				ShowCursor();

				if ( !(io->ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) )
				{
					SetMouseCursor(
						(imgui_cursor > -1 && imgui_cursor < ImGuiMouseCursor_COUNT)
							? data->imGuiToRaylibCursorMap[imgui_cursor]
							: MOUSE_CURSOR_DEFAULT
					);
				}
			}
		}
	}
}

void ImGui_ImplRaylib_Init(void)
{
	RAYGE_ASSERT_VALID(!g_Initialised);

	if ( g_Initialised )
	{
		return;
	}

	cimgui_set_assert_handler(CImGuiAssertHandler);

	memset(&g_Data, 0, sizeof(g_Data));

	g_Data.context = igCreateContext(NULL);
	igSetCurrentContext(g_Data.context);

	g_Data.lastFrameFocused = IsWindowFocused();
	g_Data.currentMouseCursor = ImGuiMouseCursor_COUNT;

	ImGuiIO* io = igGetIO();
	RAYGE_ASSERT_VALID(io);
	ImFontAtlas_AddFontDefault(io->Fonts, NULL);
	igStyleColorsDark(NULL);

	SetUpKeyMap(&g_Data);
	SetUpCursorMap(&g_Data);
	SetUpBackend();
	ReloadFonts(&g_Data);

	g_Initialised = true;
}

void ImGui_ImplRaylib_BuildFontAtlas(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	ReloadFonts(&g_Data);
}

void ImGui_ImplRaylib_Shutdown(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	ImGuiIO* io = igGetIO();
	RAYGE_ASSERT_VALID(io);

	io->Fonts->TexID = 0;

	SetFontTextureFromImage(&g_Data, NULL);

	cimgui_set_assert_handler(NULL);
}

void ImGui_ImplRaylib_NewFrame(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	UpdateImGuiIOValuesOnFrameBegin(&g_Data, GetFrameTime());
	igNewFrame();
}

void ImGui_ImplRaylib_ProcessEvents(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	ProcessEvents(&g_Data);
}

void ImGui_ImplRaylib_Render(void)
{
	RAYGE_ASSERT_VALID(g_Initialised);

	if ( !g_Initialised )
	{
		return;
	}

	// Must always be called, or Renderer_DirectDrawImGui() may fail.
	igRender();

	RayGE_Renderer* renderer = RendererSubsystem_GetRenderer();
	Renderer_SetDrawingModeDirect(renderer);
	Renderer_DirectDrawImGui(renderer);
}
