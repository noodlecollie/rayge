#include "Testing/AngleTests.h"
#include "Testing/Testing.h"
#include "RayGE/Angles.h"

static bool FuzzyEquals(EulerAngles a, EulerAngles b)
{
#define COMPARE_EPSILON 0.0001f

	return fabsf(a.pitch - b.pitch) < COMPARE_EPSILON && fabsf(a.yaw - b.yaw) < COMPARE_EPSILON &&
		fabsf(a.roll - b.roll) < COMPARE_EPSILON;

#undef COMPARE_EPSILON
}

static EulerAngles MakeAnglesSimple(Vector3 axis, float degrees)
{
	Vector3 out = QuaternionToEuler(QuaternionFromAxisAngle(axis, DEG2RAD * degrees));
	return NormaliseEulerAngles(CanonicalEulerAngles(out));
}

void Testing_RunAngleNormalisationTests(void)
{
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(0.0f), 0.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(-0.0f), 0.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(90.0f), 90.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(180.0f), 180.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(270.0f), 270.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(360.0f), 0.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(-90.0f), 270.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(-180.0f), 180.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(-270.0f), 90.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(-360.0f), 0.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(45.0f), 45.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(-45.0f), 315.0f);

	// Coalesce really small values:
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(-0.0000001f), 0.0f);
	TEST_EXPECT_EQL_FLOAT(NormaliseDegreeValue(0.0000001f), 0.0f);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 0.0f, 0.0f}),
		((EulerAngles) {0.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 90.0f, 0.0f}),
		((EulerAngles) {0.0f, 90.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 180.0f, 0.0f}),
		((EulerAngles) {0.0f, 180.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 270.0f, 0.0f}),
		((EulerAngles) {0.0f, 270.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 360.0f, 0.0f}),
		((EulerAngles) {0.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, -90.0f, 0.0f}),
		((EulerAngles) {0.0f, 270.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, -180.0f, 0.0f}),
		((EulerAngles) {0.0f, 180.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, -270.0f, 0.0f}),
		((EulerAngles) {0.0f, 90.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, -360.0f, 0.0f}),
		((EulerAngles) {0.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {90.0f, 0.0f, 0.0f}),
		((EulerAngles) {90.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {-90.0f, 0.0f, 0.0f}),
		((EulerAngles) {-90.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {180.0f, 0.0f, 0.0f}),
		((EulerAngles) {0.0f, 180.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {-180.0f, 0.0f, 0.0f}),
		((EulerAngles) {0.0f, 180.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {270.0f, 0.0f, 0.0f}),
		((EulerAngles) {-90.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {-270.0f, 0.0f, 0.0f}),
		((EulerAngles) {90.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {45.0f, 0.0f, 0.0f}),
		((EulerAngles) {45.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {-45.0f, 0.0f, 0.0f}),
		((EulerAngles) {-45.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {135.0f, 0.0f, 0.0f}),
		((EulerAngles) {45.0f, 180.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {-135.0f, 0.0f, 0.0f}),
		((EulerAngles) {-45.0f, 180.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 0.0f, 90.0f}),
		((EulerAngles) {0.0f, 0.0f, 90.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 0.0f, -90.0f}),
		((EulerAngles) {0.0f, 0.0f, -90.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 0.0f, 180.0f}),
		((EulerAngles) {0.0f, 0.0f, -180.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 0.0f, -180.0f}),
		((EulerAngles) {0.0f, 0.0f, -180.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 0.0f, 270.0f}),
		((EulerAngles) {0.0f, 0.0f, -90.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 0.0f, -270.0f}),
		((EulerAngles) {0.0f, 0.0f, 90.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 0.0f, 360.0f}),
		((EulerAngles) {0.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		NormaliseEulerAngles((EulerAngles) {0.0f, 0.0f, -360.0f}),
		((EulerAngles) {0.0f, 0.0f, 0.0f})
	);
}

void Testing_RunAngleToDirectionVectorTests(void)
{
	// Roll should not affect any of these.

	// Default
	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, 0.0f, 0.0f}),
		((Vector3) {1.0f, 0.0f, 0.0f})
	);

	// Default with roll
	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, 0.0f, 10.0f}),
		((Vector3) {1.0f, 0.0f, 0.0f})
	);

	// Yaw 90 degrees counter-clockwise
	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, 90.0f, 0.0f}),
		((Vector3) {0.0f, 1.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, 90.0f, 10.0f}),
		((Vector3) {0.0f, 1.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, -270.0f, 0.0f}),
		((Vector3) {0.0f, 1.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, -270.0f, 10.0f}),
		((Vector3) {0.0f, 1.0f, 0.0f})
	);

	// Yaw 90 degrees clockwise
	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, -90.0f, 0.0f}),
		((Vector3) {0.0f, -1.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, -90.0f, -10.0f}),
		((Vector3) {0.0f, -1.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, 270.0f, 0.0f}),
		((Vector3) {0.0f, -1.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, 270.0f, -10.0f}),
		((Vector3) {0.0f, -1.0f, 0.0f})
	);

	// Yaw 180 degrees
	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, 180.0f, 0.0f}),
		((Vector3) {-1.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, 180.0f, 10.0f}),
		((Vector3) {-1.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, -180.0f, 0.0f}),
		((Vector3) {-1.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, -180.0f, 10.0f}),
		((Vector3) {-1.0f, 0.0f, 0.0f})
	);

	// Yaw 360 degrees
	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, 360.0f, 0.0f}),
		((Vector3) {1.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, 360.0f, 10.0f}),
		((Vector3) {1.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, -360.0f, 0.0f}),
		((Vector3) {1.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {0.0f, -360.0f, 10.0f}),
		((Vector3) {1.0f, 0.0f, 0.0f})
	);

	// Up
	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {-90.0f, 0.0f, 0.0f}),
		((Vector3) {0.0f, 0.0f, 1.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {-90.0f, 0.0f, 10.0f}),
		((Vector3) {0.0f, 0.0f, 1.0f})
	);

	// Down
	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {90.0f, 0.0f, 0.0f}),
		((Vector3) {0.0f, 0.0f, -1.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {90.0f, 0.0f, -10.0f}),
		((Vector3) {0.0f, 0.0f, -1.0f})
	);

	// Oblique angle
	const float obliqueYaw = 41.0f;
	const float obliquePitch = -13.0f;

	Vector3 dirToRotate = {1.0f, 0.0f, 0.0f};
	dirToRotate = Vector3RotateByAxisAngle(dirToRotate, (Vector3) {0.0f, 1.0f, 0.0f}, DEG2RAD * obliquePitch);
	dirToRotate = Vector3RotateByAxisAngle(dirToRotate, (Vector3) {0.0f, 0.0f, 1.0f}, DEG2RAD * obliqueYaw);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {obliquePitch, obliqueYaw, 0.0f}),
		dirToRotate
	);

	TEST_EXPECT_TRUE_CALLABLE(
		Vector3Equals,
		EulerAnglesToDirection((EulerAngles) {obliquePitch, obliqueYaw, 10.0f}),
		dirToRotate
	);
}

void Testing_RunDirectionVectorToAngleTests(void)
{
	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		DirectionToEulerAngles((Vector3) {1.0f, 0.0f, 0.0f}),
		((EulerAngles) {0.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		DirectionToEulerAngles((Vector3) {-1.0f, 0.0f, 0.0f}),
		((EulerAngles) {0.0f, 180.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		DirectionToEulerAngles((Vector3) {0.0f, 1.0f, 0.0f}),
		((EulerAngles) {0.0f, 90.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		DirectionToEulerAngles((Vector3) {0.0f, -1.0f, 0.0f}),
		((EulerAngles) {0.0f, 270.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		DirectionToEulerAngles((Vector3) {0.0f, 0.0f, 1.0f}),
		((EulerAngles) {-90.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		EulerAnglesExactlyEqual,
		DirectionToEulerAngles((Vector3) {0.0f, 0.0f, -1.0f}),
		((EulerAngles) {90.0f, 0.0f, 0.0f})
	);
}

// This is to make sure that the Raylib functions produce the results we expect,
// given they could be eg. a different handedness.
void Testing_RunRaylibSanityChecks(void)
{
	// Default
	TEST_EXPECT_TRUE_CALLABLE(
		FuzzyEquals,
		MakeAnglesSimple((Vector3) {0.0f, 0.0f, 1.0f}, 0.0f),
		((EulerAngles) {0.0f, 0.0f, 0.0f})
	);

	// Yaw 90 degrees counter-clockwise
	TEST_EXPECT_TRUE_CALLABLE(
		FuzzyEquals,
		MakeAnglesSimple((Vector3) {0.0f, 0.0f, 1.0f}, 90.0f),
		((EulerAngles) {0.0f, 90.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		FuzzyEquals,
		MakeAnglesSimple((Vector3) {0.0f, 0.0f, 1.0f}, -270.0f),
		((EulerAngles) {0.0f, 90.0f, 0.0f})
	);

	// Yaw 90 degrees clockwise
	TEST_EXPECT_TRUE_CALLABLE(
		FuzzyEquals,
		MakeAnglesSimple((Vector3) {0.0f, 0.0f, 1.0f}, 270.0f),
		((EulerAngles) {0.0f, 270.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		FuzzyEquals,
		MakeAnglesSimple((Vector3) {0.0f, 0.0f, 1.0f}, -90.0f),
		((EulerAngles) {0.0f, 270.0f, 0.0f})
	);

	// Yaw 180 degrees
	TEST_EXPECT_TRUE_CALLABLE(
		FuzzyEquals,
		MakeAnglesSimple((Vector3) {0.0f, 0.0f, 1.0f}, 180.0f),
		((EulerAngles) {0.0f, 180.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		FuzzyEquals,
		MakeAnglesSimple((Vector3) {0.0f, 0.0f, 1.0f}, -180.0f),
		((EulerAngles) {0.0f, 180.0f, 0.0f})
	);

	// Yaw 360 degrees
	TEST_EXPECT_TRUE_CALLABLE(
		FuzzyEquals,
		MakeAnglesSimple((Vector3) {0.0f, 0.0f, 1.0f}, 360.0f),
		((EulerAngles) {0.0f, 0.0f, 0.0f})
	);

	TEST_EXPECT_TRUE_CALLABLE(
		FuzzyEquals,
		MakeAnglesSimple((Vector3) {0.0f, 0.0f, 1.0f}, -360.0f),
		((EulerAngles) {0.0f, 0.0f, 0.0f})
	);
}
