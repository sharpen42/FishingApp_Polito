#include "Vector.h"


Vector2::Vector2() { x = 0.0; y = 0.0; v = Vector3(); }
Vector2::Vector2(double x0, double y0) { x = x0; y = y0; }
Vector2::Vector2(double a[2]) { x = a[0]; y = a[1]; }
//Vector2::~Vector2() { delete a; }

bool Vector2::operator==(Vector2 v) { return (x == v.x && y == v.y); }
bool Vector2::operator!=(Vector2 v) { return (x != v.x || y != v.y); }
Vector2 Vector2::operator+(Vector2 v) { return add(v); }
Vector2 Vector2::operator-(Vector2 v) { return sub(v); }
double Vector2::operator*(Vector2 v) { return dot(v); }
Vector2 Vector2::operator*(double a) { return mult(a); }
double Vector2::operator%(Vector2 v) { return cross(v); }
Vector2 Vector2::operator+=(Vector2 v) { sum(v.x, v.y); return *this; }

double Vector2::magnitude() { return sqrt(x * x + y * y); }
Vector2 Vector2::normalize() { double m = magnitude(); return Vector2(x / m, y / m); }
double Vector2::distance(Vector2 v) { return (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y); }
double Vector2::dot(Vector2 v) { return x * v.x + y * v.y; }
double Vector2::cross(Vector2 v) { return x * v.y - y * v.x; }
Vector2 Vector2::add(Vector2 v) { return Vector2(x + v.x, y + v.y); }
Vector2 Vector2::sub(Vector2 v) { return Vector2(x - v.x, y - v.y); }
Vector2 Vector2::mult(double a) { return Vector2(a * x, a * y); }
Vector2 Vector2::sum(double dx, double dy) { x += dx; y += dy; return *this; }
Vector2 Vector2::scale(double sx, double sy) { x *= sx; y *= sy; return *this; }
Vector3 Vector2::asVector3() {
	v.x = x;
	v.y = y;
	v.z = 0.0;
	return v;
}

double* Vector2::toArray(double a[2]){
	a[0] = x;
	a[1] = y;
	return a;
}

Vector2 Vector2::Origin() { return Vector2(); }
Vector2 Vector2::I() { return Vector2(1, 0); }
Vector2 Vector2::J() { return Vector2(0, 1); }
Vector2 Vector2::Polar(double R, double theta) {
	return Vector2(R * cos(theta), R * sin(theta));
}

Vector3::Vector3() { x = 0.0; y = 0.0; z = 0.0;}
//Vector3::~Vector3(){ delete a; }
Vector3::Vector3(double x0, double y0, double z0) { x = x0; y = y0; z = z0; }
Vector3::Vector3(double a[3]) { x = a[0]; y = a[1]; z = a[2]; }

bool Vector3::operator==(Vector3 v) { return (x == v.x && y == v.y && z == v.z); }
bool Vector3::operator!=(Vector3 v) { return (x != v.x || y != v.y || z != v.z); }
Vector3 Vector3::operator+(Vector3 v) { return add(v); }
Vector3 Vector3::operator-(Vector3 v) { return sub(v); }
double Vector3::operator*(Vector3 v) { return dot(v); }
Vector3 Vector3::operator*(double a) { return mult(a); }
Vector3 Vector3::operator*(Mat3x3 m) { return mult3(m); }
Vector3 Vector3::operator*(Mat4x4 m) { return mult4(m); }
Vector3 Vector3::operator*(Quaternion q) { return mult3(q.toMat3x3()); }
Vector3 Vector3::operator%(Vector3 v) { return cross(v); }
Vector3 Vector3::operator+=(Vector3 v) { sum(v.x, v.y, v.z); return *this; }
Vector3 Vector3::operator-=(Vector3 v) { sum(-v.x, -v.y, -v.z); return *this; }
Vector3 Vector3::operator*=(float f) { x *= f; y *= f; z *= f; return *this; }

double Vector3::magnitude() { return sqrt(x * x + y * y + z * z); }
Vector3 Vector3::normalize() { double m = magnitude(); if (m != 1.0 && m != 0.0) return Vector3(x / m, y / m, z / m); else return *this; }
double Vector3::distance(Vector3 v) { return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z)); }
double Vector3::dot(Vector3 v) { return x * v.x + y * v.y + z * v.z; }
Vector3 Vector3::cross(Vector3 v) { return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
Vector3 Vector3::add(Vector3 v) { return Vector3(x + v.x, y + v.y, z + v.z); }
Vector3 Vector3::sub(Vector3 v) { return Vector3(x - v.x, y - v.y, z - v.z); }
Vector3 Vector3::mult(double a){ return Vector3(a * x, a * y, a * z); }

Vector3 Vector3::mult3(Mat3x3 m) {
	double v0[3], v1[3] = {x, y, z};
	for (int i = 0; i < 3; i++) {
		v0[i] = 0;
		for (int j = 0; j < 3; j++) {
			v0[i] += v1[i] * m.getElement(i, j);
		}
	}
	return Vector3(v0[0], v0[1], v0[2]);
}
Vector3 Vector3::mult4(Mat4x4 m) {
	double v0[4], v1[4] = { x, y, z, 1 };
	for (int i = 0; i < 4; i++) {
		v0[i] = 0;
		for (int j = 0; j < 4; j++) {
			v0[i] += v1[i] * m.getElement(i, j);
		}
	}
	return Vector3(v0[0], v0[1], v0[2]);
}
Vector3 Vector3::sum(double dx, double dy, double dz) { x += dx; y += dy; z += dz; return *this; }
Vector3 Vector3::scale(double sx, double sy, double sz) { x *= sx; y *= sy; z *= sz; return *this; }
Vector3 Vector3::set(Vector3 v) { x = v.x; y = v.y; z = v.z; return *this; }
Vector3 Vector3::set(double x0, double y0, double z0) { x = x0; y = y0; z = z0; return *this; }

double* Vector3::toArray(double a[3]){
	a[0] = x;
	a[1] = y;
	a[2] = z;
	return a;
}

float* Vector3::toArray3f(float d[3]) {
	d[0] = (float)x;
	d[1] = (float)y;
	d[2] = (float)z;
	return d;
}

float* Vector3::toArray4f(float d[4]) {
	d[0] = (float) x;
	d[1] = (float) y;
	d[2] = (float) z;
	d[3] = 1.0f;
	return d;
}

Vector2 Vector3::xy() { return Vector2(x, y); }
Vector2 Vector3::xz() { return Vector2(x, z); }
Vector2 Vector3::yz() { return Vector2(y, z); }

Vector3 Vector3::Origin() { return Vector3(); }
Vector3 Vector3::I() { return Vector3(1, 0, 0); }
Vector3 Vector3::J() { return Vector3(0, 1, 0); }
Vector3 Vector3::K() { return Vector3(0, 0, 1); }
Vector3 Vector3::Polar(double R, double theta, double phi) { 
	return Vector3(R * cos(theta) * cos(phi), R * sin(theta) * cos(phi), R * sin(phi)); 
}


Mat3x3::Mat3x3() {
	elements = new double[9];
	for (int i = 0; i < 9; i++) {
		if (i % 4 == 0) elements[i] = 1;
		else elements[i] = 0;
	}
}
//Mat3x3::~Mat3x3() { delete elements; }
Mat3x3::Mat3x3(double a[9]) {
	elements = new double[9];
	for (int i = 0; i < 9; i++) {
		elements[i] = a[i];
	}
}

Mat3x3 Mat3x3::operator*(Mat3x3 m) { return mult(m); }

void Mat3x3::setElement(int i, int j, double x) { elements[i * 3 + j] = x; }
double Mat3x3::getElement(int i, int j) { return elements[i * 3 + j]; }
Vector3 Mat3x3::getRow(int i) {
	switch (i) {
	case 0: return Vector3(elements[0], elements[1], elements[2]);
	case 1: return Vector3(elements[3], elements[4], elements[5]);
	case 2: return Vector3(elements[6], elements[7], elements[8]);
	default: return Vector3();
	}
}
Vector3 Mat3x3::getColumn(int j) {
	switch (j) {
	case 0: return Vector3(elements[0], elements[3], elements[6]);
	case 1: return Vector3(elements[1], elements[4], elements[7]);
	case 2: return Vector3(elements[2], elements[5], elements[8]);
	default: return Vector3();
	}
}
Mat3x3 Mat3x3::mult(Mat3x3 m) {
	Mat3x3 t = Mat3x3();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == j) t.elements[i * 3 + j] = 0;
			for (int k = 0; k < 3; k++) {
				t.elements[i * 3 + j] += elements[i * 3 + k] * m.elements[k * 3 + j];
			}
		}
	}
	return t;
}

Mat3x3 Mat3x3::transpose()
{
	Mat3x3 t = Mat3x3();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			t.elements[i * 3 + j] = elements[j * 3 + i];
		}
	}
	return t;
}

double* Mat3x3::toArray(){ return elements; }

Mat4x4::Mat4x4() {
	elements = new double[16];
	for (int i = 0; i < 16; i++) {
		if (i % 5 == 0) elements[i] = 1;
		else elements[i] = 0;
	}
}

//Mat4x4::~Mat4x4() { delete elements; }
Mat4x4::Mat4x4(double a[16]) {
	elements = new double[16];
	for (int i = 0; i < 9; i++){
		elements[i] = a[i];
	}
}

Mat4x4 Mat4x4::operator*(Mat4x4 m) { return mult(m); }

void Mat4x4::setElement(int i, int j, double x) { elements[i * 4 + j] = x; }
double Mat4x4::getElement(int i, int j) { return elements[i * 4 + j]; }
Mat4x4 Mat4x4::mult(Mat4x4 m) {
	Mat4x4 t = Mat4x4();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j) t.elements[i * 4 + j] = 0.0;
			for (int k = 0; k < 4; k++) {
				t.elements[i * 4 + j] += elements[i * 4 + k] * m.elements[k * 4 + j];
			}
		}
	}
	return t;
}

Mat4x4 Mat4x4::transpose()
{
	Mat4x4 t = Mat4x4();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			t.elements[i * 4 + j] = elements[j * 4 + i];
		}
	}
	return t;
}

double* Mat4x4::toArray() { return elements; }

Quaternion::Quaternion() {
	angle = 0;
	axis = Vector3(1.0, 0.0, 0.0);
}

Quaternion::Quaternion(float a, Vector3 v) {
	angle = a;
	axis = v;
}

Quaternion::Quaternion(float a, float x, float y, float z) {
	angle = a;
	axis = Vector3(x, y, z).normalize();
}

Mat3x3 Quaternion::toMat3x3(){
	double cosa = cos(angle);
	double sina = sin(angle);
	double elem[9] = {     cosa,     -sina * axis.z, sina * axis.y,
					   sina * axis.z,     cosa,     -sina * axis.x,
					  -sina * axis.y, sina * axis.x,     cosa      };
	return Mat3x3(elem);
}
