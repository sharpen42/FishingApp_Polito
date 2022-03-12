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

#define STATE_NULL 0

class Transform;
class GameObj;
class Camera;
class BoundingBox2D;

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

class BoundingBox2D {
private:
	bool inside;
public:
	Vector2 position, dimensions;

	BoundingBox2D();
	BoundingBox2D(double dimx, double dimy);
	BoundingBox2D(double dimx, double dimy, Vector2 position);

	bool isEmpty();
	bool collide(Vector2);
	bool collide(BoundingBox2D);
	double getX();
	double getY();
	void setOutside();
	void setInside();
};

class GameObj {
private:
	std::string name;
	bool render;
	int status;
public:
	Mesh mesh;
	Curve curve;
	Transform transform;
	std::map<int, Material> materials;
	BoundingBox2D boundingBox;
	Texture texture;

	GameObj();
	GameObj(std::string n);
	int setState(int);
	int getState();
	string setName(std::string n);
	Mesh addMesh(Mesh m); // returns previous mesh
	Curve addCurve(Curve c); // returns previous curve
	Texture addTexture(Texture t);
	BoundingBox2D setBoundingBox2D();
	BoundingBox2D setBoundingBox2D(double dx, double dy);
	BoundingBox2D setBoundingBox2D(BoundingBox2D);
	const char* getName();
	void place(Vector3 p);
	void place(float x, float y, float z);
	void translate(Vector3 t);
	void translate(float x, float y, float z);
	void rotate(Vector3 r);
	void rotate(float x, float y, float z);
	void scale(Vector3 s);
	void scale(float x, float y, float z);
	void reScale();
	void setVelocity(Vector3 v);
	void setAcceleration(Vector3 v);
	void applyForce(Vector3 f);
	void applyForce(double fx, double fy, double fz);
	void move(float deltaTime);
	void hide();
	void show();
	bool isHidden();
	void renderOpenGL();
	bool checkCollision(double bx, double by, double bz);
	bool checkCollision(double xBound, double yBound, double zBound, Vector3 v);
	bool checkCollision(GameObj);
	bool checkCollision(BoundingBox2D);
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
	bool hasMoved();
};

Mat4x4 translation(double dx, double dy, double dz);
Mat4x4 rotation(double ax, double ay, double az);
Mat4x4 scale(double sx, double sy, double sz);

