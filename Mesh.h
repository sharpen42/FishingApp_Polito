#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <GL/glut.h>

#include "Vector.h"

using namespace std;

class Mesh {
private:
	int V, U, F;
public:
	vector<Vector3> verts;
	vector<Vector3> norms;
	vector<Vector2> uvs;
	vector<int> faces;

	Mesh();
	Mesh(vector<Vector3> verts0, vector<Vector3> norms0, vector<Vector2> uvs0, vector<int> faces0);
	int sizeVerts();
	int sizeUVs();
	int sizeFaces();
	bool isEmpty();
	double* vertsArray(double*);  // allocation is done inside
	double* normsArray(double*);  // allocation is done inside
	double* uvsArray(double*);    // allocation is done inside
	int* faceArray(int*);         // allocation is done inside
};

class Curve {
private:
	int P, S;
public:
	vector<Vector3> points;
	vector<int> segments;

	Curve();
	Curve(vector<Vector3> points0, vector<int> segments0);
	int sizePoints();
	int sizeSegments();
	bool isEmpty();
	double* pointsArray();
	void follow(Vector3, Vector3);
};