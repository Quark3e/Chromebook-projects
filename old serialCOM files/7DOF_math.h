#pragma once

/*7 DOF "4-joint" robot*/
//variant 2

int customBox1_Var_rotations_for7DOF_rig(float input) {
	int degree;
	if (input - 200 > 0) { degree = -45; }
	else if (input - 200 < 0) { degree = -45; }
	return degree;
} // obsolete


void get_Angles(
	Vector3f PP,
	float* q1,
	float* q2,
	float* q3,
	float* q4,
	float* q5,
	float* q6,
	float* a1,
	float* b1,
	Vector3f* P2,
	float a,
	float b,
	float Y,
	char posOption,
	float length_scalar = 1,
	float coord_scalar = 1,
	bool printText = true
) {
	float lambda, mu;
	float d5x, d5z;

	PP.x = PP.x * coord_scalar;
	PP.y = PP.y * coord_scalar;
	PP.z = PP.z * coord_scalar;

	d1 = d1 * length_scalar;
	d2 = d2 * length_scalar;
	d3 = d3 * length_scalar;
	d4 = d4 * length_scalar;
	d5 = d5 * length_scalar;
	d6 = d6 * length_scalar;


	float l = (d5+d6) * cos(b);
	(*P2).x = PP.x - l * sin(a);
	(*P2).y = PP.y - l * cos(a);
	(*P2).z = PP.z - (d5+d6) * sin(b);

	if (printText) {
		printf("\nP2-- x:%f y:%f z:%f	with l:%f\n", (*P2).x, (*P2).y, (*P2).z, l);
		printf("a:%f b:%f Y:%f\n", toDegrees(a), toDegrees(b), toDegrees(Y));
	}

	if ((*P2).y == 0) {
		(*P2).y = 0.00001;
	}
	else if ((*P2).y < 0) {
		(*P2).y = 0;
		if (printText) {
			printf("Y-value of PP is modified, as P2.y is behind y line\n");
		}
	}

	(*q1) = atan((*P2).x / (*P2).y);

	if (posOption == '+') {
		(*q3) = acos(
			(pow((*P2).x, 2) + pow((*P2).y, 2) + pow((*P2).z - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /
			(2 * d2 * (d3 + d4))
		);
	}
	else if (posOption == '-') {
		(*q3) = acos(
			(pow((*P2).x, 2) + pow((*P2).y, 2) + pow((*P2).z - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /
			(2 * d2 * (d3 + d4))
		);
	}
	lambda = atan(((*P2).z - d1) / sqrt(pow((*P2).x, 2) + pow((*P2).y, 2)));
	mu = atan(
		((d3 + d4) * sin(*q3)) /
		(d2 + (d3 + d4) * cos(*q3))
	);

	//cout << "lambda: " << toDegrees(lambda) << " mu: " << toDegrees(mu) << endl;

	if (posOption == '+')
		(*q2) = lambda - mu;
	else if (posOption == '-') {
		(*q2) = lambda + mu;
		(*q3) = 0 - (*q3);
	}

	(*a1) = a - (*q1);
	(*b1) = b - (*q2 + *q3);

	d5x = (d5+d6) * cos(*b1) * sin(*a1); //NOTE: The x and y axis of the X1Y1Z1 frame in the paper was reverse (compared to this. The names need to be changed!)
	d5z = (d5+d6) * sin(*b1);

	//cout << "d5x:" << d5x << " d5z:" << d5z << " a1:" << a1 << " b1:" << b1 << endl;
	if(printText)
		printf("d5x:%f d5z:%f a1:%f b1:%f lambda:%f mu:%f", d5x, d5z, toDegrees(*a1), toDegrees(*b1), toDegrees(lambda), toDegrees(mu));

	if ((*b1) == 0)
		(*q4) = 0;
	else if ((*b1) < 0 || (*b1) > 0)
		(*q4) = atan(d5x / d5z);

	/*if (cos(*b1) * cos(*a1) == 0) {
		(*q5) = 0;
	}*/
	//else {
	double check = asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6));
	if (isnan(check)) {
		if (printText) {
			printf("asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / d5)  is NaN\n");
		}
	}
	else {
		if (sqrt(pow(d5x, 2) + pow(d5z, 2)) > 90.0) {
			(*q5) = toRadians(90);
		}
		else {
			(*q5) = asin(
				sqrt(pow(d5x, 2) + pow(d5z, 2)) /
				(d5+d6)
			);
		}
	}

	//(*q5) = atan(sqrt(pow(d5x, 2) + pow(d5z, 2)) / (cos(*b1) * cos(*a1)));
	//(*q5) = atan(sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5x / tan(*a1)));


	if (d5z < 0) {
		(*q5) = 0.0 - (*q5);
	}
	if (printText) {
		printf("\n-----------q5:%f; d5x:%f; d5z:%f\n", toDegrees(*q5), d5x, d5z);
	}

		//cout << "it ran\n";
	//}
	if (b <= PI / 2 && b >= 0 - PI / 2)
		(*q6) = Y - (*q4);
	else if (b >= PI / 2 || b <= 0 - PI / 2)
		(*q6) = PI - (Y - (*q6));


	//cout << "q1:" << toDegrees(*q1) << " q2:" << toDegrees(*q2) << " q3:" << toDegrees(*q3) << " q4:" << toDegrees(*q4) << " q5:" << toDegrees(*q5) << " q6:" << toDegrees(*q6) << endl;
	if (printText)
		printf("\nq1:%f q2:%f q3:%f q4:%f q5:%f q6:%f\n",
			toDegrees(*q1),
			toDegrees(*q2),
			toDegrees(*q3),
			toDegrees(*q4),
			toDegrees(*q5),
			toDegrees(*q6)
		);

} //note: all angles ares in radian, whether input or output; it will not let point P2 be behind the X-axis (i.e have a negative Y value)











//variant 1
//float J2_P2_distance1(Vector3f P2, float d1) {
//	float d;
//	d = sqrt(pow(P2.x, 2) + pow(P2.y, 2) + pow(P2.z - d1, 2));
//	return d;
//} //equation: 5
//
//float J2_P2_distance2(Vector3f P2, float d2, float d3, float q3) {
//	float d;
//	d = sqrt(pow(d2, 2) + pow(d3, 2) - 2 * d2 * d3 * cos(PI + q3));
//	return d;
//} //equation: 6
//
//
//float q3_positive(Vector3f P2, float d1, float d2, float d3) {
//	float q3;
//	q3 = acos((pow(P2.x, 2) + pow(P2.y, 2) + pow(P2.z - d1, 2) - pow(d2, 2) - pow(d3, 2)) / (2 * d2 * d3));
//	return q3;
//} //in radians
//
//float q3_negative(Vector3f P2, float d1, float d2, float d3) {
//	float q3;
//	q3 = -acos((pow(P2.x, 2) + pow(P2.y, 2) + pow(P2.z - d1, 2) - pow(d2, 2) - pow(d3, 2)) / (2 * d2 * d3));
//	return q3;
//} //in radians
//
//float q2_positive(Vector3f P2, float d1, float d2, float d3, float q3, float* q2) {
//	float ans;
//	ans = atan((P2.z - d1) / sqrt(pow(P2.x, 2) + pow(P2.y, 2))) -
//		atan((d3 * sin(q3)) / (d2 + d3 * cos(q3)));
//	(*q2) = ans;
//	cout << "[+] ";
//	return ans;
//} //in radians
//
//float q2_negative(Vector3f P2, float d1, float d2, float d3, float q3, float* q2) {
//	float ans;
//	ans = atan((P2.z - d1) / sqrt(pow(P2.x, 2) + pow(P2.y, 2))) +
//		atan((d3 * sin(q3)) / (d2 + d3 * cos(q3)));
//	(*q2) = ans;
//	cout << "[-] ";
//	return ans;
//} //in radians
//
//float get_lambdaAngle(Vector3f P2, float d1) {
//	return atan((P2.z - d1) / sqrt(pow(P2.x, 2) + pow(P2.y, 2)));
//}
//
//float get_a1(float a, float q1) {
//	return a - q1;
//} //both parameter values in radians
//
//float get_modified_a1(float a, float q1) {
//	return a - (PI / 2 - q1);
//} //both parameter values in radians
//
//float get_b1(float b, float q2, float q3) {
//	return b - (q2 + q3);
//} //both parameters in radians
//
//float get_d4ry(float d4, float b1, float a1) {
//	return d4 * cos(b1) * sin(a1);
//} //every angle value in radians
//
//float get_d4rz(float d4, float b1) {
//	return d4 * sin(b1);
//} //every angle value in radians
//
//float get_d5x(float d5, float b1, float a1) {
//	return d5 * cos(b1) * sin(a1);
//} //returns a value in radians
//
//float get_d5z(float d5, float b1) {
//	return d5 * sin(b1);
//} //returns a value in radians
//
//float q4_func(float d4ry /*or d5x*/, float d4rz /*or d5z*/) {
//	return atan(d4ry / d4rz);
//} //in radians; do not use if a1 or b1 is 0
//
//float q5_func(float d4ry /*or d5x*/, float d4rz /*or d5z*/, float b1, float a1) {
//	float q5;
//	q5 = atan(sqrt(pow(d4ry, 2) + pow(d4rz, 2)) / (cos(b1) * cos(a1)));
//	return q5;
//} //every angle value in radians
//
//float q6_func(float Y, float q4, float b) {
//	float q6 = 0;
//	if (b >= (0 - PI / 2) && b <= PI / 2)
//		q6 = Y - q4;
//	else if ((b >= 0 - PI && b < PI / 2) || (b > PI / 2 && b <= PI))
//		q6 = PI - (Y - q4);
//	return q6;
//}

