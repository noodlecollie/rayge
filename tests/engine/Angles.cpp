#include <catch2/catch_test_macros.hpp>
#include "RayGE/Angles.h"

static bool operator==(const Vector3& a, const Vector3& b)
{
	return ::Vector3Equals(a, b);
}

static bool operator==(const EulerAngles& a, const EulerAngles& b)
{
	return ::EulerAnglesEqual(a, b);
}

static std::ostream& operator<<(std::ostream& os, const Vector3& value)
{
	os << "(" << value.x << ", " << value.y << ", " << value.z << ")";
	return os;
}

static std::ostream& operator<<(std::ostream& os, const EulerAngles& value)
{
	os << "(" << value.pitch << ", " << value.yaw << ", " << value.roll << ")";
	return os;
}

static EulerAngles FromRaylibAngles(Vector3 in)
{
	return NormaliseEulerAngles(CanonicalEulerAngles(in));
}

static EulerAngles MakeAnglesSimple(const Vector3& axis, float degrees)
{
	Vector3 out = QuaternionToEuler(QuaternionFromAxisAngle(axis, DEG2RAD * degrees));
	return FromRaylibAngles(out);
}

static bool FuzzyEquals(const EulerAngles& a, const EulerAngles& b)
{
	static constexpr float COMPARE_EPSILON = 0.0001;

	return fabsf(a.pitch - b.pitch) < COMPARE_EPSILON && fabsf(a.yaw - b.yaw) < COMPARE_EPSILON &&
		fabsf(a.roll - b.roll) < COMPARE_EPSILON;
}

TEST_CASE("Angle normalisation is correct", "[angles]")
{
	CHECK(NormaliseDegreeValue(0.0f) == 0.0f);
	CHECK(NormaliseDegreeValue(-0.0f) == 0.0f);
	CHECK(NormaliseDegreeValue(90.0f) == 90.0f);
	CHECK(NormaliseDegreeValue(180.0f) == 180.0f);
	CHECK(NormaliseDegreeValue(270.0f) == 270.0f);
	CHECK(NormaliseDegreeValue(360.0f) == 0.0f);
	CHECK(NormaliseDegreeValue(-90.0f) == 270.0f);
	CHECK(NormaliseDegreeValue(-180.0f) == 180.0f);
	CHECK(NormaliseDegreeValue(-270.0f) == 90.0f);
	CHECK(NormaliseDegreeValue(-360.0f) == 0.0f);
	CHECK(NormaliseDegreeValue(45.0f) == 45.0f);
	CHECK(NormaliseDegreeValue(-45.0f) == 315.0f);

	// Coalesce really small values:
	CHECK(NormaliseDegreeValue(-0.0000001f) == 0.0f);
	CHECK(NormaliseDegreeValue(0.0000001f) == 0.0f);

	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 0.0f, 0.0f}) == EulerAngles {0.0f, 0.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 90.0f, 0.0f}) == EulerAngles {0.0f, 90.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 180.0f, 0.0f}) == EulerAngles {0.0f, 180.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 270.0f, 0.0f}) == EulerAngles {0.0f, 270.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 360.0f, 0.0f}) == EulerAngles {0.0f, 0.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, -90.0f, 0.0f}) == EulerAngles {0.0f, 270.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, -180.0f, 0.0f}) == EulerAngles {0.0f, 180.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, -270.0f, 0.0f}) == EulerAngles {0.0f, 90.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, -360.0f, 0.0f}) == EulerAngles {0.0f, 0.0f, 0.0f});

	CHECK(NormaliseEulerAngles(EulerAngles {90.0f, 0.0f, 0.0f}) == EulerAngles {90.0f, 0.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {-90.0f, 0.0f, 0.0f}) == EulerAngles {-90.0f, 0.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {180.0f, 0.0f, 0.0f}) == EulerAngles {0.0f, 180.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {-180.0f, 0.0f, 0.0f}) == EulerAngles {0.0f, 180.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {270.0f, 0.0f, 0.0f}) == EulerAngles {-90.0f, 0.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {-270.0f, 0.0f, 0.0f}) == EulerAngles {90.0f, 0.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {45.0f, 0.0f, 0.0f}) == EulerAngles {45.0f, 0.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {-45.0f, 0.0f, 0.0f}) == EulerAngles {-45.0f, 0.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {135.0f, 0.0f, 0.0f}) == EulerAngles {45.0f, 180.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {-135.0f, 0.0f, 0.0f}) == EulerAngles {-45.0f, 180.0f, 0.0f});

	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 0.0f, 90.0f}) == EulerAngles {0.0f, 0.0f, 90.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 0.0f, -90.0f}) == EulerAngles {0.0f, 0.0f, -90.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 0.0f, 180.0f}) == EulerAngles {0.0f, 0.0f, -180.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 0.0f, -180.0f}) == EulerAngles {0.0f, 0.0f, -180.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 0.0f, 270.0f}) == EulerAngles {0.0f, 0.0f, -90.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 0.0f, -270.0f}) == EulerAngles {0.0f, 0.0f, 90.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 0.0f, 360.0f}) == EulerAngles {0.0f, 0.0f, 0.0f});
	CHECK(NormaliseEulerAngles(EulerAngles {0.0f, 0.0f, -360.0f}) == EulerAngles {0.0f, 0.0f, 0.0f});
}

TEST_CASE("Angles convert to the correct direction vectors", "[angles]")
{
	// Roll should not affect any of these.

	// Default
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, 0.0f, 0.0f}) == Vector3 {1.0f, 0.0f, 0.0f});

	// Default with roll
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, 0.0f, 10.0f}) == Vector3 {1.0f, 0.0f, 0.0f});

	// Yaw 90 degrees counter-clockwise
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, 90.0f, 0.0f}) == Vector3 {0.0f, 1.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, 90.0f, 10.0f}) == Vector3 {0.0f, 1.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, -270.0f, 0.0f}) == Vector3 {0.0f, 1.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, -270.0f, 10.0f}) == Vector3 {0.0f, 1.0f, 0.0f});

	// Yaw 90 degrees clockwise
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, -90.0f, 0.0f}) == Vector3 {0.0f, -1.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, -90.0f, -10.0f}) == Vector3 {0.0f, -1.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, 270.0f, 0.0f}) == Vector3 {0.0f, -1.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, 270.0f, -10.0f}) == Vector3 {0.0f, -1.0f, 0.0f});

	// Yaw 180 degrees
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, 180.0f, 0.0f}) == Vector3 {-1.0f, 0.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, 180.0f, 10.0f}) == Vector3 {-1.0f, 0.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, -180.0f, 0.0f}) == Vector3 {-1.0f, 0.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, -180.0f, 10.0f}) == Vector3 {-1.0f, 0.0f, 0.0f});

	// Yaw 360 degrees
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, 360.0f, 0.0f}) == Vector3 {1.0f, 0.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, 360.0f, 10.0f}) == Vector3 {1.0f, 0.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, -360.0f, 0.0f}) == Vector3 {1.0f, 0.0f, 0.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {0.0f, -360.0f, 10.0f}) == Vector3 {1.0f, 0.0f, 0.0f});

	// Up
	CHECK(EulerAnglesToDirection(EulerAngles {-90.0f, 0.0f, 0.0f}) == Vector3 {0.0f, 0.0f, 1.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {-90.0f, 0.0f, 10.0f}) == Vector3 {0.0f, 0.0f, 1.0f});

	// Down
	CHECK(EulerAnglesToDirection(EulerAngles {90.0f, 0.0f, 0.0f}) == Vector3 {0.0f, 0.0f, -1.0f});
	CHECK(EulerAnglesToDirection(EulerAngles {90.0f, 0.0f, -10.0f}) == Vector3 {0.0f, 0.0f, -1.0f});

	// Oblique angle
	constexpr float OBLIQUE_YAW = 41.0f;
	constexpr float OBLIQUE_PITCH = -13.0f;

	Vector3 dirToRotate = {1.0f, 0.0f, 0.0f};
	dirToRotate = Vector3RotateByAxisAngle(dirToRotate, Vector3 {0.0f, 1.0f, 0.0f}, DEG2RAD * OBLIQUE_PITCH);
	dirToRotate = Vector3RotateByAxisAngle(dirToRotate, Vector3 {0.0f, 0.0f, 1.0f}, DEG2RAD * OBLIQUE_YAW);

	CHECK(EulerAnglesToDirection(EulerAngles {OBLIQUE_PITCH, OBLIQUE_YAW, 0.0f}) == dirToRotate);
	CHECK(EulerAnglesToDirection(EulerAngles {OBLIQUE_PITCH, OBLIQUE_YAW, 10.0f}) == dirToRotate);
}

TEST_CASE("Direction vectors convert to the correct angles", "[angles]")
{
	CHECK(DirectionToEulerAngles(Vector3{1.0f, 0.0f, 0.0f}) == EulerAngles{0.0f, 0.0f, 0.0f});
	CHECK(DirectionToEulerAngles(Vector3{-1.0f, 0.0f, 0.0f}) == EulerAngles{0.0f, 180.0f, 0.0f});
	CHECK(DirectionToEulerAngles(Vector3{0.0f, 1.0f, 0.0f}) == EulerAngles{0.0f, 90.0f, 0.0f});
	CHECK(DirectionToEulerAngles(Vector3{0.0f, -1.0f, 0.0f}) == EulerAngles{0.0f, 270.0f, 0.0f});
	CHECK(DirectionToEulerAngles(Vector3{0.0f, 0.0f, 1.0f}) == EulerAngles{-90.0f, 0.0f, 0.0f});
	CHECK(DirectionToEulerAngles(Vector3{0.0f, 0.0f, -1.0f}) == EulerAngles{90.0f, 0.0f, 0.0f});
}

// This is to make sure that the Raylib functions produce the results we expect,
// given they could be eg. a different handedness.
TEST_CASE("Raylib Euler angle functions produce correct results", "[angles]")
{
	// Default
	CHECK(MakeAnglesSimple(Vector3 {0.0f, 0.0f, 1.0f}, 0.0f) == EulerAngles {0.0f, 0.0f, 0.0f});

	// Yaw 90 degrees counter-clockwise
	CHECK(MakeAnglesSimple(Vector3 {0.0f, 0.0f, 1.0f}, 90.0f) == EulerAngles {0.0f, 90.0f, 0.0f});
	CHECK(MakeAnglesSimple(Vector3 {0.0f, 0.0f, 1.0f}, -270.0f) == EulerAngles {0.0f, 90.0f, 0.0f});

	// Yaw 90 degrees clockwise
	CHECK(MakeAnglesSimple(Vector3 {0.0f, 0.0f, 1.0f}, 270.0f) == EulerAngles {0.0f, 270.0f, 0.0f});
	CHECK(MakeAnglesSimple(Vector3 {0.0f, 0.0f, 1.0f}, -90.0f) == EulerAngles {0.0f, 270.0f, 0.0f});

	// Yaw 180 degrees
	CHECK(MakeAnglesSimple(Vector3 {0.0f, 0.0f, 1.0f}, 180.0f) == EulerAngles {0.0f, 180.0f, 0.0f});
	CHECK(MakeAnglesSimple(Vector3 {0.0f, 0.0f, 1.0f}, -180.0f) == EulerAngles {0.0f, 180.0f, 0.0f});

	// Yaw 360 degrees
	CHECK(FuzzyEquals(MakeAnglesSimple(Vector3 {0.0f, 0.0f, 1.0f}, 360.0f), EulerAngles {0.0f, 0.0f, 0.0f}));
	CHECK(MakeAnglesSimple(Vector3 {0.0f, 0.0f, 1.0f}, -360.0f) == EulerAngles {0.0f, 0.0f, 0.0f});
}
