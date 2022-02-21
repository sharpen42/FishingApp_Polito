#include "Mesh.h"

Mesh::Mesh() {
	verts = vector<Vector3>(0);
	norms = vector<Vector3>(0);
	uvs = vector<Vector2>(0);
	faces = vector<int>(0);
	V = U = F = 0;
}

Mesh::Mesh(vector<Vector3> verts0, vector<Vector3> norms0, vector<Vector2> uvs0, vector<int> faces0) {
	verts = verts0;
	norms = norms0;
	uvs = uvs0;
	faces = faces0;
	V = verts.size();
	U = uvs.size();
	F = faces.size();
}

int Mesh::sizeVerts() { return V; }
int Mesh::sizeUVs() { return U; }
int Mesh::sizeFaces() { return F; }
bool Mesh::isEmpty() { return (V == 0); }

double* Mesh::vertsArray(double* v) {
	v = new double[3 * V];

	for (int i = 0; i < V; i++) {
		v[3 * i] = verts[i].x;
		v[3 * i + 1] = verts[i].y;
		v[3 * i + 2] = verts[i].z;
	}
	
	return v;
}

double* Mesh::normsArray(double* v) {
	v = new double[3 * V];

	for (int i = 0; i < V; i++) {
		v[3 * i] = norms[i].x;
		v[3 * i + 1] = norms[i].y;
		v[3 * i + 2] = norms[i].z;
	}

	return v;
}

double* Mesh::uvsArray(double* v) {
	v = new double[2 * U];

	for (int i = 0; i < U; i++) {
		v[(i << 1)] = uvs[i].x;
		v[(i << 1) + 1] = uvs[i].y;
	}

	return v;
}

int* Mesh::faceArray(int* v) {
	v = new int[F];

	for (int i = 0; i < F; i++) 
		v[i] = faces[i];

	return v;
}

Curve::Curve() {
	points = vector<Vector3>(0);
	segments = vector<int>(0);
	P = 0;
	S = 0;
}

Curve::Curve(vector<Vector3> points0, vector<int> segments0) {
	points = points0;
	segments = segments0;
	P = points0.size();
	S = segments0.size();
}

int Curve::sizePoints() { return P; }
int Curve::sizeSegments() { return S; }
bool Curve::isEmpty(){ return (P == 0); }

double* Curve::pointsArray()
{
	double* v = new double[3 * P];

	for (int i = 0; i < P; i++) {
		v[3 * i] = points[i].x;
		v[3 * i + 1] = points[i].y;
		v[3 * i + 2] = points[i].z;
	}

	return v;
}
