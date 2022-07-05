#pragma once


/*3 DOF "2-joint" robot*/
float yawServo(float x, float y, float* c, float scalar = 1) {
	float degrees;
	degrees = toDegrees(atan(x / y));
	(*c) = sqrt(pow(x, 2) + pow(y, 2));
	return degrees;
}

void getJoint_2_angle(
	float arm1,
	float arm2,
	float x, //put y, which is c
	float y, //put z
	float* angle,
	string angleChoice,
	float scalar = 1
) {
	if (angleChoice == "positive") {
		(*angle) = toDegrees(acos((pow(scalar * x, 2) + pow(scalar * y, 2) - pow(arm1, 2) - pow(arm2, 2)) / (2 * arm2 * arm1)));
	}
	else if (angleChoice == "negative") {
		(*angle) = 0.0 - toDegrees(acos((pow(scalar * x, 2) + pow(scalar * y, 2) - pow(arm1, 2) - pow(arm2, 2)) / (2 * arm2 * arm1)));
	}
} //angle1 = cos  (positive); angle2 = -cos  (negative)

void getJoint_1_angle_positive(
	float arm1,
	float arm2,
	float x,
	float y,
	float jointAngle2,
	float* angle,
	float scalar = 1
) {
	(*angle) = toDegrees(
		atan(
			(scalar * y) / (scalar * x)
		) -
		atan(
			(arm2 * sin(toRadians(jointAngle2))) /
			(arm1 + arm2 * cos(toRadians(jointAngle2)))
		)
	);
} //when converted to a 3-d movement the x will be the 3d y, and y will be the 3d z

void getJoint_1_angle_negative(
	float arm1,
	float arm2,
	float x,
	float y,
	float jointAngle2,
	float* angle,
	float scalar = 1
) {
	(*angle) = toDegrees(atan((scalar * y) / (scalar * x)) - atan((arm2 * sin(toRadians(jointAngle2))) / (arm1 + arm2 * cos(toRadians(jointAngle2)))));
} //when converted to a 3-d movement the x will be the 3d y, and y will be the 3d z

