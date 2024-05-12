#include "Rendering/NuklearCommandExecutor.h"
#include "Subsystems/LoggingSubsystem.h"
#include "Subsystems/RendererSubsystem.h"
#include "Debugging.h"
#include "raylib.h"

#define MAX_POLYGON_POINTS 64

#define MARK_UNIMPLEMENTED(name) \
	do \
	{ \
		static double lastLogTime = -3.0; \
		double currentTime = GetTime(); \
		if ( currentTime - lastLogTime >= 3.0 ) \
		{ \
			LoggingSubsystem_PrintLine(RAYGE_LOG_WARNING, "Nuklear draw command \"" name "\" not yet implemented"); \
			lastLogTime = currentTime; \
		} \
	} \
	while ( false )

static Rectangle NKToRaylibRect(short x, short y, unsigned short w, unsigned short h)
{
	return (Rectangle) {(float)x, (float)y, (float)w, (float)h};
}

static Color NKToRaylibColor(struct nk_color colour)
{
	return (Color) {colour.r, colour.g, colour.b, colour.a};
}

static Color NKColorBlendOpaque(struct nk_color a, struct nk_color b)
{
	return (Color) {
		(unsigned char)(((float)a.r * 0.5f) + ((float)b.r * 0.5f)),
		(unsigned char)(((float)a.g * 0.5f) + ((float)b.g * 0.5f)),
		(unsigned char)(((float)a.b * 0.5f) + ((float)b.b * 0.5f)),
		255,
	};
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

static void NKDrawCurve(const struct nk_command_curve* command)
{
	// TODO: Implement according to https://pomax.github.io/bezierinfo/#flattening
	(void)command;
	MARK_UNIMPLEMENTED("nk_command_curve");
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

static void NKDrawMulticolouredRect(const struct nk_command_rect_multi_color* command)
{
	(void)command;
	MARK_UNIMPLEMENTED("nk_command_rect_multi_color");
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

static void NKDrawCircle(const struct nk_command_circle* command)
{
	const float radiusH = (float)command->w / 2.0f;
	const float radiusV = (float)command->h / 2.0f;

	// This doesn't currently support different line thicknesses
	DrawEllipseLines(
		command->x + (int)radiusH,
		command->y + (int)radiusV,
		radiusH,
		radiusV,
		NKToRaylibColor(command->color)
	);
}

static void NKDrawCircleFilled(const struct nk_command_circle_filled* command)
{
	const float radiusH = (float)command->w / 2.0f;
	const float radiusV = (float)command->h / 2.0f;

	DrawEllipse(
		command->x + (int)radiusH,
		command->y + (int)radiusV,
		radiusH,
		radiusV,
		NKToRaylibColor(command->color)
	);
}

static void NKDrawArc(const struct nk_command_arc* command)
{
	(void)command;
	MARK_UNIMPLEMENTED("nk_command_arc");
}

static void NKDrawArcFilled(const struct nk_command_arc_filled* command)
{
	(void)command;
	MARK_UNIMPLEMENTED("nk_command_arc_filled");
}

static void NKDrawTriangle(const struct nk_command_triangle* command)
{
	// No ability to set line width here yet
	DrawTriangleLines(
		NKToRaylibVec2(command->c),
		NKToRaylibVec2(command->b),
		NKToRaylibVec2(command->a),
		NKToRaylibColor(command->color)
	);
}

static void NKDrawTriangleFilled(const struct nk_command_triangle_filled* command)
{
	DrawTriangle(
		NKToRaylibVec2(command->c),
		NKToRaylibVec2(command->b),
		NKToRaylibVec2(command->a),
		NKToRaylibColor(command->color)
	);
}

static void NKDrawPolygon(const struct nk_command_polygon* command)
{
	(void)command;
	MARK_UNIMPLEMENTED("nk_command_polygon");
}

// TODO: This is currently untested as I'm not sure which Nuklear
// UI items actually use it. I'm assuming the Nuklear points are
// provided the opposite way around to what Raylib requires, as
// this was the case when drawing triangles, but this may not be
// true and might need swapping. It may also not be a triangle
// fan, and could be a strip or simply a number of sequential
// triangles - again, evaluate.
static void NKDrawPolygonFilled(const struct nk_command_polygon_filled* command)
{
	static Vector2 points[MAX_POLYGON_POINTS];

	size_t numPoints = command->point_count;

	if ( numPoints > MAX_POLYGON_POINTS )
	{
		numPoints = MAX_POLYGON_POINTS;
	}

	for ( unsigned short index = 0; index < numPoints; ++index )
	{
		points[index] = NKToRaylibVec2(command->points[numPoints - index - 1]);
	}

	DrawTriangleFan(points, (int)numPoints, NKToRaylibColor(command->color));
}

static void NKDrawPolyLine(const struct nk_command_polyline* command)
{
	(void)command;
	MARK_UNIMPLEMENTED("nk_command_polyline");
}

static void NKDrawImage(const struct nk_command_image* command)
{
	(void)command;
	MARK_UNIMPLEMENTED("nk_command_image");
}

static void NKDrawCustom(const struct nk_command_custom* command)
{
	(void)command;
	MARK_UNIMPLEMENTED("nk_command_custom");
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
			NKDrawCurve((const struct nk_command_curve*)command);
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
		{
			NKDrawMulticolouredRect((const struct nk_command_rect_multi_color*)command);
			break;
		}

		case NK_COMMAND_CIRCLE:
		{
			NKDrawCircle((const struct nk_command_circle*)command);
			break;
		}

		case NK_COMMAND_CIRCLE_FILLED:
		{
			NKDrawCircleFilled((const struct nk_command_circle_filled*)command);
			break;
		}

		case NK_COMMAND_ARC:
		{
			NKDrawArc((const struct nk_command_arc*)command);
			break;
		}

		case NK_COMMAND_ARC_FILLED:
		{
			NKDrawArcFilled((const struct nk_command_arc_filled*)command);
			break;
		}

		case NK_COMMAND_TRIANGLE:
		{
			NKDrawTriangle((const struct nk_command_triangle*)command);
			break;
		}

		case NK_COMMAND_TRIANGLE_FILLED:
		{
			NKDrawTriangleFilled((const struct nk_command_triangle_filled*)command);
			break;
		}

		case NK_COMMAND_POLYGON:
		{
			NKDrawPolygon((const struct nk_command_polygon*)command);
			break;
		}

		case NK_COMMAND_POLYGON_FILLED:
		{
			NKDrawPolygonFilled((const struct nk_command_polygon_filled*)command);
			break;
		}

		case NK_COMMAND_POLYLINE:
		{
			NKDrawPolyLine((const struct nk_command_polyline*)command);
			break;
		}

		case NK_COMMAND_TEXT:
		{
			NKDrawText((const struct nk_command_text*)command);
			break;
		}

		case NK_COMMAND_IMAGE:
		{
			NKDrawImage((const struct nk_command_image*)command);
			break;
		}

		case NK_COMMAND_CUSTOM:
		{
			NKDrawCustom((const struct nk_command_custom*)command);
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
