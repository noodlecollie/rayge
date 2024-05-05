#include <catch2/catch_test_macros.hpp>
#include "RayGE/Angles.h"

static bool operator==(const Vector3& a, const Vector3& b)
{
	return Vector3Equals(a, b);
}

static std::ostream& operator<<(std::ostream& os, const Vector3& value)
{
	os << "(" << value.x << ", " << value.y << ", " << value.z << ")";
	return os;
}

TEST_CASE("Angles convert to the correct direction vectors", "[angles]")
{
	// Roll should not affect any of these

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
}
