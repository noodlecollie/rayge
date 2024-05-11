#include "Rendering/NuklearCommandExecutor.h"
#include "Subsystems/LoggingSubsystem.h"
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

void NuklearCommand_Execute(const struct nk_command* command)
{
	switch ( command->type )
	{
		case NK_COMMAND_NOP:
		{
			break;
		}

		case NK_COMMAND_SCISSOR:
		{
			// TODO
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
		case NK_COMMAND_TEXT:
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
}
