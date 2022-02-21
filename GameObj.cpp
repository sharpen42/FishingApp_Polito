#include "GameObj.h"
#include <GL/glut.h>

#define CURVE_DEF 10
#define face_n(x) (((x) - ((x) % 3)) / 3)

const Vector3 I = Vector3(1, 0, 0);
const Vector3 J = Vector3(0, 1, 0);
const Vector3 K = Vector3(0, 0, 1);

Shader::Shader() {
	shaderID = -1;
}

Transform::Transform() {
	position = Vector3();
	velocity = Vector3();
	acceleration = Vector3();
	scale = Vector3(1, 1, 1);
	rotation = Vector3();
}

Transform::Transform(Vector3 pos, Vector3 scl, Vector3 rot) {
	position = pos; 
	scale = scl;
	rotation = rot;
	velocity = Vector3();
	acceleration = Vector3();
}

Mat3x3 Transform::asMat3x3() {
	double a[9] = { position.x, velocity.x, acceleration.x,
	                position.y, velocity.y, acceleration.y,
	                position.z, velocity.z, acceleration.z };
	return Mat3x3(a);
}

void Transform::setVelocity(Vector3 v) {
	velocity.x = v.x;
	velocity.y = v.y;
	velocity.z = v.z;
}

void Transform::setAcceleration(Vector3 a) {
	acceleration.x = a.x;
	acceleration.y = a.y;
	acceleration.z = a.z;
}

void Transform::move(double dt) {
	double a[9] = {   1,     0,   0,
					 dt,     1,   0,
					dt * dt, dt,  1 };
	Mat3x3 t = Mat3x3(a);
	t = asMat3x3().mult(t);
	position.set(t.getElement(0, 0), t.getElement(1, 0), t.getElement(2, 0));
	velocity.set(t.getElement(0, 1), t.getElement(1, 1), t.getElement(2, 1));
	acceleration.set(t.getElement(0, 2), t.getElement(1, 2), t.getElement(2, 2));
}

GameObj::GameObj(){
	name.assign("gameObj");
	transform = Transform();
	mesh = Mesh();
	texture = Texture();
	render = false;
}

GameObj::GameObj(std::string n){
	name.assign(n);
	transform = Transform();
	mesh = Mesh();
	texture = Texture();
	render = true;
}

string GameObj::setName(std::string n0) {
	string t;
	t.assign(name);
	name.assign(n0);
	return t;
}

Mesh GameObj::addMesh(Mesh m0) {
	Mesh t = mesh;
	mesh = m0;
	return t;
}

Curve GameObj::addCurve(Curve c0) {
	Curve t = curve;
	curve = c0;
	return t;
}

const char* GameObj::getName(){
	return name.c_str();
}


void GameObj::place(Vector3 p) { transform.position.x = p.x; transform.position.y = p.y; transform.position.z = p.z; }
void GameObj::place(float x, float y, float z) { transform.position.x = x; transform.position.y = y; transform.position.z = z; }

void GameObj::translate(Vector3 t){
	transform.position.sum(t.x, t.y, t.z);
}

void GameObj::translate(float x, float y, float z){
	transform.position.sum(x, y, z);
}

void GameObj::rotate(Vector3 r){
	transform.rotation.sum(r.x, r.y, r.z);
}

void GameObj::rotate(float x, float y, float z) {
	transform.rotation.sum(x, y, z);
}

void GameObj::scale(Vector3 s){
	transform.scale.scale(s.x, s.y, s.z);
}

void GameObj::scale(float x, float y, float z) {
	transform.scale.scale(x, y, z);
}

void GameObj::setVelocity(Vector3 v) { 
	// modifica solo la velocità
	transform.setVelocity(v); 
}

void GameObj::applyForce(Vector3 f){
	// modifica solo l'accelerazione
	transform.acceleration.sum(f.x, f.y, f.z);
}

void GameObj::applyForce(double fx, double fy, double fz) {
	// modifica solo l'accelerazione
	transform.acceleration.sum(fx, fy, fz);
}

void GameObj::move(float deltaTime){
	transform.move(deltaTime);
}
void GameObj::hide() { render = false; }
void GameObj::show() { render = true;  }

void GameObj::renderOpenGL() {
	if (!render) return;
	bool has_UVs = (mesh.sizeUVs() > 0);
	// va chiamato dal Main Game loop
	glPushMatrix();
	glRotated(transform.rotation.x, 1, 0, 0);
	glRotated(transform.rotation.y, 0, 1, 0);
	glRotated(transform.rotation.z, 0, 0, 1);
	glScaled(transform.scale.x, transform.scale.y, transform.scale.z);
	glTranslated(transform.position.x, transform.position.y, transform.position.z);

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
	glEnable(GL_CULL_FACE);
	if (!mesh.isEmpty()) {
		int prev_faceIndex = 0;
		int V = mesh.sizeFaces();

		if (has_UVs) {
			glEnable(GL_TEXTURE_2D);
		} else glDisable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < V; i++) {
			double p[3] = { 0.0, 0.0, 0.0 };
			int vertexIndex = mesh.faces[i];
			int faceIndex = face_n(i);
			if (materials.find(faceIndex) != materials.end()) {
				float c[4];
				if (materials[faceIndex] != materials[prev_faceIndex]) {
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					//glMaterialfv(GL_FRONT, GL_DIFFUSE, materials[faceIndex].diffuse.toFloat4(c));
					//glMaterialfv(GL_FRONT, GL_SPECULAR, materials[faceIndex].specular.toFloat4(c));
					//glMaterialfv(GL_FRONT, GL_AMBIENT, materials[faceIndex].ambient.toFloat4(c));
					//glMaterialfv(GL_FRONT, GL_EMISSION, materials[faceIndex].emission.toFloat4(c));

					glColor4fv(materials[faceIndex].diffuse.toFloat4(c));
					if (has_UVs) {
						if (!materials[prev_faceIndex].texture.isEmpty())
							materials[prev_faceIndex].texture.UnBind();
						if(!materials[faceIndex].texture.isEmpty()) 
							materials[faceIndex].texture.Bind();
					}
					prev_faceIndex = faceIndex;
				}
			}
			if (has_UVs) 
				glTexCoord2d(mesh.uvs[vertexIndex].x, 1 - mesh.uvs[vertexIndex].y);
			glNormal3dv(mesh.norms[vertexIndex].toArray(p));
			glVertex3dv(mesh.verts[vertexIndex].toArray(p));
		}
		if (has_UVs) texture.UnBind();
		glEnd();
	}

	if (!curve.isEmpty()) {
		if (curve.sizePoints() == 3) {
			double a[] = { 2, -4, 2, -3, 4, -1, 1, 0, 0 };
			const Mat3x3 line_matrix = Mat3x3(a);
			Vector3 coeff_x, coeff_y, coeff_z;
			Vector3 x_coord, y_coord, z_coord;
			double step = 1.0 / (double)CURVE_DEF;

			x_coord = Vector3(curve.points[0].x, curve.points[1].x, curve.points[2].x);
			y_coord = Vector3(curve.points[0].y, curve.points[1].y, curve.points[2].y);
			z_coord = Vector3(curve.points[0].z, curve.points[1].z, curve.points[2].z);
			coeff_x = x_coord.mult3(line_matrix);
			coeff_y = y_coord.mult3(line_matrix);
			coeff_z = z_coord.mult3(line_matrix);

			glBegin(GL_LINES);
			for (int i = 0; i < CURVE_DEF; i++) {
				double ds = step * (float)i;
				double ds2 = ds * ds;
				glVertex3d(ds2 * coeff_x.x + ds * coeff_x.y + coeff_x.z,
						   ds2 * coeff_y.x + ds * coeff_y.y + coeff_y.z,
						   ds2 * coeff_z.x + ds * coeff_z.y + coeff_z.z);
				ds += step;
				ds2 = ds * ds;
				glVertex3d(ds2 * coeff_x.x + ds * coeff_x.y + coeff_x.z,
						   ds2 * coeff_y.x + ds * coeff_y.y + coeff_y.z,
						   ds2 * coeff_z.x + ds * coeff_z.y + coeff_z.z);

			}
			glEnd();
		}

	}
	glPopMatrix();
}

bool GameObj::checkCollision(double xBound, double yBound, double zBound)
{
	bool t = false;
	if (transform.position.x > xBound) {
		transform.position.x = xBound - 0.001;
		t = true;
	} else if (transform.position.x < -xBound) {
		transform.position.x = -xBound + 0.001;
		t = true;
	}
	if (transform.position.y > yBound) {
		transform.position.y = yBound - 0.001;
		t = true;
	} else if (transform.position.y < -yBound) {
		transform.position.y = -yBound + 0.001;
		t = true;
	}
	if (transform.position.z > zBound) {
		transform.position.z = zBound - 0.001;
		t = true;
	} else if (transform.position.z < -zBound) {
		transform.position.z = -zBound + 0.001;
		t = true;
	}
	return t;
}

bool GameObj::checkCollision(double xBound, double yBound, double zBound, Vector3 v)
{
	bool t = false;
	if (transform.position.x > v.x + xBound) {
		transform.position.x = xBound - 0.001;
		t = true;
	}
	else if (transform.position.x < v.x - xBound) {
		transform.position.x = -xBound + 0.001;
		t = true;
	}
	if (transform.position.y > v.y + yBound) {
		transform.position.y = yBound - 0.001;
		t = true;
	}
	else if (transform.position.y < v.y - yBound) {
		transform.position.y = -yBound + 0.001;
		t = true;
	}
	if (transform.position.z > v.z + zBound) {
		transform.position.z = zBound - 0.001;
		t = true;
	}
	else if (transform.position.z < v.z - zBound) {
		transform.position.z = -zBound + 0.001;
		t = true;
	}
	return t;
}

Camera::Camera() {
	position = Vector3(-1, 0, 0);
	focus = Vector3();
	up = Vector3(0, 0, 1);
	forward = position.sub(focus).normalize();
	left = forward.cross(up).normalize();
	has_moved = false;
}

Camera::Camera(Vector3 position0, Vector3 focus0, Vector3 up0){
	position = position0;
	focus = focus0;
	up = up0;
	forward = position.sub(focus).normalize();
	left = forward.cross(up).normalize();
	has_moved = false;
}

void Camera::translate(Vector3 t){
	position = position.sum(t.x, t.y, t.z);
	forward = position.sub(focus).normalize();
	left = forward.cross(up).normalize();
	has_moved = true;
}

void Camera::translate(float x, float y, float z) {
	position = position.sum(x, y, z);
	forward = position.sub(focus).normalize();
	left = forward.cross(up).normalize();
	has_moved = true;
}

void Camera::lookAt(Vector3 pos0, Vector3 foc0, Vector3 up0) {
	if (position != pos0) has_moved = true;
	position = pos0;
	focus = foc0;
	up = up0;
	forward = position.sub(focus).normalize();
	left = forward.cross(up).normalize();
}

void Camera::setPersp(float n, float w, float h, float f) {
	const double aspectRatio = w / h, fieldOfView = 45.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, aspectRatio, n, f);  // Znear and Zfar 
}

bool Camera::hasMoved() {
	bool t = has_moved;
	has_moved = false;
	return t;
}

Mat4x4 translation(double dx, double dy, double dz) {
	double a[] = {1, 0, 0, dx, 0, 1, 0, dy, 0, 0, 1, dz};
	return Mat4x4(a);
}

Mat4x4 rotation(double ax, double ay, double az) {
	Mat4x4 rx = Mat4x4();
	Mat4x4 ry = Mat4x4();
	Mat4x4 rz = Mat4x4();
	double a_cos, a_sin;

	a_cos = cos(ax);
	a_sin = sin(ax);
	rx.setElement(1, 1, a_cos);
	rx.setElement(1, 2, a_sin);
	rx.setElement(2, 1, -a_sin);
	rx.setElement(2, 2, a_cos);

	a_cos = cos(ay);
	a_sin = sin(ay);
	ry.setElement(0, 0, a_cos);
	ry.setElement(0, 2, a_sin);
	ry.setElement(2, 0, -a_sin);
	ry.setElement(2, 2, a_cos);

	a_cos = cos(az);
	a_sin = sin(az);
	rz.setElement(0, 0, a_cos);
	rz.setElement(0, 1, a_sin);
	rz.setElement(1, 0, -a_sin);
	rz.setElement(1, 1, a_cos);

	return rz * ry * rx;
}

Mat4x4 scale(double sx, double sy, double sz) {
	Mat4x4 t = Mat4x4();
	t.setElement(0, 0, sx);
	t.setElement(1, 1, sy);
	t.setElement(2, 2, sz);
	return t;
}

Vector3 i() { return I; }
Vector3 j() { return J; }
Vector3 k() { return K; }
