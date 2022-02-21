#pragma once
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstring>
#include <map>
#include <vector>

#include "Vector.h"
#include "Mesh.h"
#include "Color.h"

class Transform {
public:
	Vector3 position, velocity, acceleration, scale, rotation;
	Transform();
	Transform(Vector3 pos, Vector3 scl, Vector3 rot);
	Mat3x3 asMat3x3();
	void setVelocity(Vector3 v);
	void setAcceleration(Vector3 a);
	void move(double dt);
};

class GameObj {
private:
	std::string name;
	bool render;
public:
	Texture texture;
	Mesh mesh;
	Curve curve;
	Transform transform;
	std::map<int, Material> materials;

	GameObj();
	GameObj(std::string n);
	string setName(std::string n);
	Mesh addMesh(Mesh m); // returns previous mesh
	Curve addCurve(Curve c); // returns previous curve
	const char* getName();
	void place(Vector3 p);
	void place(float x, float y, float z);
	void translate(Vector3 t);
	void translate(float x, float y, float z);
	void rotate(Vector3 r);
	void rotate(float x, float y, float z);
	void scale(Vector3 s);
	void scale(float x, float y, float z);
	void setVelocity(Vector3 v);
	void applyForce(Vector3 f);
	void applyForce(double fx, double fy, double fz);
	void move(float deltaTime);
	void hide();
	void show();
	void renderOpenGL();
};

class Camera {
private:
	bool has_moved;
public:
	Vector3 position;
	Vector3 focus;
	Vector3 up, forward, left;

	Camera();
	Camera(Vector3 position, Vector3 focus, Vector3 up);
	void translate(Vector3 t);
	void translate(float x, float y, float z);
	void lookAt(Vector3 position, Vector3 focus, Vector3 up);
	void setPersp(float near, float w, float h, float far);
	bool hasMoved();
};

Mat4x4 translation(double dx, double dy, double dz);
Mat4x4 rotation(double ax, double ay, double az);
Mat4x4 scale(double sx, double sy, double sz);
Vector3 i();
Vector3 j();
Vector3 k();

