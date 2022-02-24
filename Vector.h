#pragma once
#include <memory>
#include <math.h>

const double PI = 3.141592653589793238462643383279502884197169399;

class Vector2;
class Vector3;
class Mat3x3;
class Mat4x4;
class Quaternion;

class Vector3 {
public:
	double x, y, z;

	Vector3();
	Vector3(double x0, double y0, double z0);
	Vector3(double a[3]);
	//~Vector3();

	bool operator==(Vector3);
	bool operator!=(Vector3);
	Vector3 operator+(Vector3);
	Vector3 operator-(Vector3);
	Vector3 operator*(double);     // scale by a constant
	double operator*(Vector3);     // dot product
	Vector3 operator*(Mat3x3);
	Vector3 operator*(Mat4x4);
	Vector3 operator*(Quaternion); // rotation
	Vector3 operator%(Vector3);    // cross product
	Vector3 operator+=(Vector3);
	Vector3 operator-=(Vector3);
	Vector3 operator*=(float);

	double magnitude();
	Vector3 normalize();
	double distance(Vector3 v);
	double dot(Vector3 v);
	Vector3 cross(Vector3 v);
	Vector3 add(Vector3 v);
	Vector3 sub(Vector3 v);
	Vector3 mult(double a);
	Vector3 mult3(Mat3x3 m);
	Vector3 mult4(Mat4x4 m);
	Vector3 sum(double dx, double dy, double dz); // updates the vector and returns itself
	Vector3 scale(double sx, double sy, double sz); // updates the vector and returns itself
	Vector3 set(Vector3);
	Vector3 set(double x, double y, double z);
	double* toArray(double a[3]);
	float* toArray3f(float a[3]);
	float* toArray4f(float a[4]);
	Vector2 xy();
	Vector2 xz();
	Vector2 yz();

	static Vector3 Origin();
	static Vector3 I();
	static Vector3 J();
	static Vector3 K();
	static Vector3 Polar(double R, double theta, double phi);

};

class Vector2 {
public:
	double x, y;

	Vector2();
	Vector2(double x0, double y0);
	Vector2(double a[2]);
	//~Vector2();

	bool operator==(Vector2);
	bool operator!=(Vector2);
	Vector2 operator+(Vector2);
	Vector2 operator-(Vector2);
	double operator*(Vector2);     // dot product
	double operator%(Vector2);     // cross product
	Vector2 operator*(double);     // scale by a constant
	Vector2 operator+=(Vector2);
	Vector2 operator*=(double);     // scale by a constant

	double magnitude();
	Vector2 normalize();
	double distance(Vector2 v);
	double dot(Vector2 v);
	double cross(Vector2 v);
	Vector2 add(Vector2 v);
	Vector2 sub(Vector2 v);
	Vector2 mult(double a);
	Vector2 sum(Vector2 v);
	Vector2 sum(double dx, double dy);
	Vector2 scale(double sx, double sy);
	Vector2 set(Vector2);
	Vector2 set(double x, double y);
	Vector3 asVector3();
	double* toArray(double v[2]);

	static Vector2 Origin();
	static Vector2 I();
	static Vector2 J();
	static Vector2 Polar(double R, double theta);
};

class Mat3x3 {
private:
	double elements[9];
public:
	Mat3x3();
	Mat3x3(double a[9]);
	//~Mat3x3();

	Mat3x3 operator*(Mat3x3);

	void setElement(int i, int j, double x);
	double getElement(int i, int j);
	Vector3 getRow(int i);
	Vector3 getColumn(int j);
	Mat3x3 mult(Mat3x3 m);
	Mat3x3 transpose();
	double* toArray();
};

class Mat4x4 {
private:
	double elements[16];
public:
	Mat4x4();
	Mat4x4(double a[16]);
	//~Mat4x4();

	Mat4x4 operator*(Mat4x4);

	void setElement(int i, int j, double x);
	double getElement(int i, int j);
	Mat4x4 mult(Mat4x4 m);
	Mat4x4 transpose();
	double* toArray();
};

class Quaternion {
public:
	float angle;
	Vector3 axis;

	Quaternion();
	Quaternion(float, Vector3);
	Quaternion(float, float, float, float);
	Mat3x3 toMat3x3();
};