#include "Testing/AngleTests.h"
#include "Testing/Testing.h"
#include "RayGE/Angles.h"

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
