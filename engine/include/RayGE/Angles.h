#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include "RayGE/Math.h"

// References for angles:
// (0, 0, 0) is looking along +X, with up pointing along +Z.
// (0, 90, 0) is a counter-clockwise rotation from above: looks along +Y.
// (0, -90, 0) is a clockwise rotation from above: looks along -Y.
// (90, 0, 0) points down: looks along -Z.
// (-90, 0, 0) points up: looks along +Z.
// (0, 0, 15) rotates clockwise (right) along forward axis.
// (0, 0, -15) rotates counter-clockwise (left) along forward axis.
typedef struct EulerAngles
{
	float pitch;
	float yaw;
	float roll;
} EulerAngles;

// Ensures value is in range [0 360).
static inline float NormaliseDegreeValue(float val)
{
	// Don't do this with tiny values as it can cause unexpected behaviour.
	if ( FloatEquals(val, 0.0f) )
	{
		return 0.0f;
	}

	float modVal = fmodf(val, 360.0f);

	if ( modVal == 0.0f )
	{
		// Make sure the 0 we return is positive.
		return 0.0f;
	}
	else if ( modVal > 0.0f  )
	{
		return modVal;
	}

	// Value is negative, and represents something we should
	// subtract from 360 (by adding the negative number).
	float outVal = 360.0f + modVal;

	// Sometimes, if modVal is really small, the result
	// of the above can just be 360. If this is the case,
	// return 0 instead.
	return FloatEquals(outVal, 360.0f) ? 0.0f : outVal;
}

// Normalises angles as follows:
// - Pitch converted to [-90 90]
// - Yaw converted to [0 360)
// - Roll converted to [-180 180)
static inline EulerAngles NormaliseEulerAngles(EulerAngles angles)
{
	// Roll is easy.
	angles.roll = NormaliseDegreeValue(angles.roll + 180.0f) - 180.0f;

	// Pitch may affect yaw.
	// Begin with the interval [-180 180).
	angles.pitch = NormaliseDegreeValue(angles.pitch + 180.0f) - 180.0f;

	// Is the pitch wild enough to flip our yaw?
	if ( angles.pitch < -90.0f )
	{
		angles.pitch = -(180.0f + angles.pitch);

		if ( angles.pitch == -0.0f )
		{
			// Ensure 0 is positive.
			angles.pitch = 0.0f;
		}

		angles.yaw += 180.0f;
	}
	else if ( angles.pitch > 90.0f )
	{
		angles.pitch = 180.0f - angles.pitch;
		angles.yaw += 180.0f;
	}

	// Now normalise yaw.
	angles.yaw = NormaliseDegreeValue(angles.yaw);

	return angles;
}

static inline void EulerAnglesToBasis(EulerAngles angles, Vector3* forward, Vector3* right, Vector3* up)
{
	float sinYaw = sinf(DEG2RAD * angles.yaw);
	float cosYaw = cosf(DEG2RAD * angles.yaw);

	float sinPitch = sinf(DEG2RAD * angles.pitch);
	float cosPitch = cosf(DEG2RAD * angles.pitch);

	if ( forward )
	{
		forward->x = cosPitch * cosYaw;
		forward->y = cosPitch * sinYaw;
		forward->z = -sinPitch;
	}

	if ( !right && !up )
	{
		return;
	}

	float sinRoll = sinf(DEG2RAD * angles.roll);
	float cosRoll = cosf(DEG2RAD * angles.roll);

	if ( right )
	{
		right->x = (-1.0f * sinRoll * sinPitch * cosYaw + -1.0f * cosRoll * -sinYaw);
		right->y = (-1.0f * sinRoll * sinPitch * sinYaw + -1.0f * cosRoll * cosYaw);
		right->z = (-1.0f * sinRoll * cosPitch);
	}

	if ( up )
	{
		up->x = (cosRoll * sinPitch * cosYaw + -sinRoll * -sinYaw);
		up->y = (cosRoll * sinPitch * sinYaw + -sinRoll * cosYaw);
		up->z = (cosRoll * cosPitch);
	}
}

// Returns forward vector.
static inline Vector3 EulerAnglesToDirection(EulerAngles angles)
{
	Vector3 forward;
	EulerAnglesToBasis(angles, &forward, NULL, NULL);
	return forward;
}

static inline EulerAngles DirectionToEulerAngles(Vector3 direction)
{
	if ( direction.x == 0.0f && direction.y == 0.0f )
	{
		float pitch = 0.0f;

		if ( direction.z > 0 )
		{
			pitch = -90.0f;
		}
		else if ( direction.z < 0 )
		{
			pitch = 90.0f;
		}

		EulerAngles outAngles = {pitch, 0.0f, 0.0f};
		return outAngles;
	}

	float yaw = atan2f(direction.y, direction.x) * RAD2DEG;
	yaw = NormaliseDegreeValue(yaw);

	float hyp = sqrtf((direction.x * direction.x) + (direction.y * direction.y));

	float pitch = atan2f(-direction.z, hyp) * RAD2DEG;
	pitch = NormaliseDegreeValue(pitch + 180.0f) - 180.0f;

	EulerAngles outAngles = {pitch, yaw, 0};
	return outAngles;
}

static inline bool EulerAnglesEqual(EulerAngles a, EulerAngles b)
{
	return FloatEquals(a.pitch, b.pitch) && FloatEquals(a.yaw, b.yaw) && FloatEquals(a.roll, b.roll);
}

// Raymath has a couple of functions which return Euler angles, but these
// have components in a different order, and whose angles are radians.
// This function converts back to out canonical angle type.
static inline EulerAngles CanonicalEulerAngles(Vector3 raymathAngles)
{
	EulerAngles outAngles =  {RAD2DEG * raymathAngles.y, RAD2DEG * raymathAngles.z, RAD2DEG * raymathAngles.x};
	return outAngles;
}
