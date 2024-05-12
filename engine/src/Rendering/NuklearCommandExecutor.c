#include "Rendering/NuklearCommandExecutor.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Subsystems/RendererSubsystem.h"
#include "Debugging.h"
#include "raylib.h"

static Rectangle NKToRaylibRect(short x, short y, unsigned short w, unsigned short h)
{
	return (Rectangle) {(float)x, (float)y, (float)w, (float)h};
}

static Color NKToRaylibColor(struct nk_color colour)
{
	return (Color) {colour.r, colour.g, colour.b, colour.a};
}

static Vector2 NKToRaylibVec2(struct nk_vec2i vec)
{
	return (Vector2) {(float)vec.x, (float)vec.y};
}

static void NKScissor(const struct nk_command_scissor* command)
{
	BeginScissorMode((int)command->x, (int)command->y, (int)command->w, (int)command->h);
}

static void NKDrawLine(const struct nk_command_line* command)
{
	DrawLineEx(
		NKToRaylibVec2(command->begin),
		NKToRaylibVec2(command->end),
		(float)command->line_thickness,
		NKToRaylibColor(command->color)
	);
}

static void NKDrawRect(const struct nk_command_rect* command)
{
	DrawRectangleLinesEx(
		NKToRaylibRect(command->x, command->y, command->w, command->h),
		(float)command->line_thickness,
		NKToRaylibColor(command->color)
	);
}

static void NKDrawRectFilled(const struct nk_command_rect_filled* command)
{
	DrawRectangleRec(NKToRaylibRect(command->x, command->y, command->w, command->h), NKToRaylibColor(command->color));
}

static void NKDrawText(const struct nk_command_text* command)
{
	// TODO: Needs a proper UI font
	DrawTextPro(
		RendererSubsystem_GetDefaultFont(),
		command->string,
		(Vector2) {(float)command->x, (float)command->y},
		(Vector2) {0.0f, 0.0f},
		0.0f,
		command->font->height,
		0.0f,
		NKToRaylibColor(command->foreground)
	);
}

static bool ProcessCommand(const struct nk_command* command)
{
	bool appliedScissor = false;

	switch ( command->type )
	{
		case NK_COMMAND_NOP:
		{
			break;
		}

		case NK_COMMAND_SCISSOR:
		{
			NKScissor((const struct nk_command_scissor*)command);
			appliedScissor = true;
			break;
		}

		case NK_COMMAND_LINE:
		{
			NKDrawLine((const struct nk_command_line*)command);
			break;
		}

		case NK_COMMAND_CURVE:
		{
			// TODO
			break;
		}

		case NK_COMMAND_RECT:
		{
			NKDrawRect((const struct nk_command_rect*)command);
			break;
		}

		case NK_COMMAND_RECT_FILLED:
		{
			NKDrawRectFilled((const struct nk_command_rect_filled*)command);
			break;
		}

		case NK_COMMAND_RECT_MULTI_COLOR:
		case NK_COMMAND_CIRCLE:
		case NK_COMMAND_CIRCLE_FILLED:
		case NK_COMMAND_ARC:
		case NK_COMMAND_ARC_FILLED:
		case NK_COMMAND_TRIANGLE:
		case NK_COMMAND_TRIANGLE_FILLED:
		case NK_COMMAND_POLYGON:
		case NK_COMMAND_POLYGON_FILLED:
		case NK_COMMAND_POLYLINE:
		{
			// TODO
			break;
		}

		case NK_COMMAND_TEXT:
		{
			NKDrawText((const struct nk_command_text*)command);
			break;
		}

		case NK_COMMAND_IMAGE:
		case NK_COMMAND_CUSTOM:
		{
			// TODO
			break;
		}

		default:
		{
			RAYGE_ASSERT_UNREACHABLE("Encountered unrecognised Nuklear draw command: %d", command->type);

			LoggingSubsystem_PrintLine(
				RAYGE_LOG_WARNING,
				"Encountered unrecognised Nuklear draw command: %d",
				command->type
			);

			break;
		}
	}

	return appliedScissor;
}

void NuklearCommand_ProcessCommands(struct nk_context* context)
{
	if ( !context )
	{
		return;
	}

	const struct nk_command* cmd = 0;
	bool appliedScissor = false;

	nk_foreach(cmd, context)
	{
		if ( ProcessCommand(cmd) && !appliedScissor )
		{
			appliedScissor = true;
		}
	}

	nk_clear(context);

	if ( appliedScissor )
	{
		EndScissorMode();
	}
}
