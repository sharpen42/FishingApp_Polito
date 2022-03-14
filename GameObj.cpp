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
	status = STATE_NULL;
	name.assign("gameObj");
	transform = Transform();
	mesh = Mesh();
	render = false;
}

GameObj::GameObj(std::string n){
	status = STATE_NULL;
	name.assign(n);
	transform = Transform();
	mesh = Mesh();
	render = true;
}

int GameObj::setState(int s) {
	int t = status;
	status = s;
	return t;
}
int GameObj::getState() { return status; }

string GameObj::setName(std::string n0) {
	string t;
	t.assign(name);
	name.assign(n0);
	return t;
}

Mesh GameObj::addMesh(Mesh m0) {
	Mesh t = mesh;
	Vector2 min_d, max_d;
	mesh = m0;
	return t;
}

Curve GameObj::addCurve(Curve c0) {
	Curve t = curve;
	curve = c0;
	return t;
}

Texture GameObj::addTexture(Texture t0) {
	Texture t = texture;
	texture = t0;
	return t;
}

BoundingBox2D GameObj::setBoundingBox2D() {
	Vector2 max_d = Vector2(-HUGE_VAL, -HUGE_VAL), min_d = Vector2(HUGE_VAL, HUGE_VAL);
	BoundingBox2D t = boundingBox;
	if (mesh.isEmpty()) return t;
	for (int i = 0; i < mesh.sizeVerts(); i++) {
		if (min_d.x > mesh.verts[i].x) min_d.x = mesh.verts[i].x;
		if (min_d.y > mesh.verts[i].y) min_d.y = mesh.verts[i].y;
		if (max_d.x < mesh.verts[i].x) max_d.x = mesh.verts[i].x;
		if (max_d.y < mesh.verts[i].y) max_d.y = mesh.verts[i].y;
	}
	boundingBox = BoundingBox2D(max(abs(transform.position.x - max_d.x), abs(transform.position.x - min_d.x)), max(abs(transform.position.y - max_d.y), abs(transform.position.y - min_d.y)), transform.position.xy());
	return t;
}

BoundingBox2D GameObj::setBoundingBox2D(double dx, double dy) {
	BoundingBox2D t = boundingBox;
	if (mesh.isEmpty()) return t;
	boundingBox = BoundingBox2D(dx, dy, transform.position.xy());
	return t;
}

BoundingBox2D GameObj::setBoundingBox2D(BoundingBox2D b) {
	BoundingBox2D t = boundingBox;
	boundingBox = b;
	return t;
}

const char* GameObj::getName(){
	return name.c_str();
}


void GameObj::place(Vector3 p) { 
	boundingBox.position.x = transform.position.x = p.x; 
	boundingBox.position.y = transform.position.y = p.y; 
	transform.position.z = p.z; 
}

void GameObj::place(float x, float y, float z) { 
	boundingBox.position.x = transform.position.x = x; 
	boundingBox.position.y = transform.position.y = y;
	transform.position.z = z;
}

void GameObj::translate(Vector3 t){
	transform.position.sum(t.x, t.y, t.z);
	if (!boundingBox.isEmpty())
		boundingBox.position.sum(t.x, t.y);
}

void GameObj::translate(float x, float y, float z){
	transform.position.sum(x, y, z);
	if (!boundingBox.isEmpty())
		boundingBox.position.sum(x, y);
}

void GameObj::rotate(Vector3 r){
	transform.rotation.sum(r.x, r.y, r.z);
	if (transform.rotation.x > 360) transform.rotation.x -= 360;
	if (transform.rotation.y > 360) transform.rotation.y -= 360;
	if (transform.rotation.z > 360) transform.rotation.z -= 360;
}

void GameObj::rotate(float x, float y, float z) {
	transform.rotation.sum(x, y, z);
	if (transform.rotation.x > 360) transform.rotation.x -= 360;
	if (transform.rotation.y > 360) transform.rotation.y -= 360;
	if (transform.rotation.z > 360) transform.rotation.z -= 360;
}

void GameObj::scale(Vector3 s){
	transform.scale.scale(s.x, s.y, s.z);
}

void GameObj::scale(float x, float y, float z) {
	transform.scale.scale(x, y, z);
}

void GameObj::resetTransform() {
	transform.scale.x = transform.scale.y = transform.scale.z = 1.0;
	transform.rotation.x = transform.rotation.y = transform.rotation.z = 0.0;
	transform.position.set(0, 0, 0);
	transform.velocity.set(0, 0, 0);
	transform.acceleration.set(0, 0, 0);
}

void GameObj::setVelocity(Vector3 v) { 
	// modifica solo la velocità
	transform.setVelocity(v); 
}

void GameObj::setAcceleration(Vector3 v) {
	// modifica solo l'accelerazione
	transform.setAcceleration(v);
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
	Vector3 prev_pos = transform.position;
	transform.move(deltaTime);
	if(!boundingBox.isEmpty())
		boundingBox.position.sum((transform.position - prev_pos).xy());
}
void GameObj::hide() { render = false; }
void GameObj::show() { render = true;  }
bool GameObj::isHidden() { return (!render); }

void GameObj::renderOpenGL() {
	static double a[] = { 2, -4, 2, -3, 4, -1, 1, 0, 0 };
	bool has_texture = false;
	if (!render) return;
	// va chiamato dal Main Game loop
	glPushMatrix();
	glTranslated(transform.position.x, transform.position.y, transform.position.z);
	glScaled(transform.scale.x, transform.scale.y, transform.scale.z);
	glRotated(transform.rotation.z, 0, 0, 1);
	glRotated(transform.rotation.y, 0, 1, 0);
	glRotated(transform.rotation.x, 1, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING); 
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL); 
	if (texture.isEmpty()) {
		glDisable(GL_TEXTURE_2D);
		has_texture = false;
		texture.UnBind();
	} else {
		glEnable(GL_TEXTURE_2D);
		has_texture = true;
		texture.Bind();
	}

	if (!mesh.isEmpty()) {
		int prev_faceIndex = -1;
		int V = mesh.sizeFaces();

		glBegin(GL_TRIANGLES);
		for (int i = 0; i < V; i++) {
			double p[3] = { 0.0, 0.0, 0.0 };
			int vertexIndex = mesh.faces[i];
			int faceIndex = face_n(i);
			float c[4] = {0.0, 0.0, 0.0, 1.0};
			
			if (materials.find(faceIndex) != materials.end() && (prev_faceIndex == -1 || materials[faceIndex] != materials[prev_faceIndex])) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materials[faceIndex].ambient.toFloat4(c));
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materials[faceIndex].diffuse.toFloat4(c));
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materials[faceIndex].specular.toFloat4(c));
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materials[faceIndex].emission.toFloat4(c));
			}
			glColor4fv(materials[faceIndex].diffuse.toFloat4(c));
			if (has_texture) 
				glTexCoord2d(mesh.uvs[vertexIndex].x, 1 - mesh.uvs[vertexIndex].y);
			glNormal3dv(mesh.norms[vertexIndex].toArray(p));
			glVertex3dv(mesh.verts[vertexIndex].toArray(p));
			prev_faceIndex = faceIndex;
		}
		if (has_texture) {
			glDisable(GL_TEXTURE_2D);
			has_texture = false;
			texture.UnBind();
		}
		glEnd();
	}
	/*
	if (!curve.isEmpty()) {
		if (curve.sizePoints() == 4) {
			double t[] = {  curve.points[1].x, curve.points[1].y, curve.points[1].z,
							curve.points[2].x, curve.points[2].y, curve.points[2].z,
							curve.points[3].x, curve.points[3].y, curve.points[3].z };
			Mat3x3 line_matrix = Mat3x3(a);
			Mat3x3 coord = Mat3x3(t);
			Mat3x3 coeff = coord.mult(line_matrix);
			double v[3], ds, ds2, step = 1.0 / (double)CURVE_DEF;

			glColor4f(0.0, 0.0, 0.0, 0.7);
			glBegin(GL_LINES);
			for (int i = 0; i < CURVE_DEF; i++) {
				ds = step * (float)i;
				ds2 = ds * ds;
				glVertex3dv(Vector3(ds2, ds, 1).mult3(coeff).toArray(v));
				ds += step;
				ds2 = ds * ds;
				glVertex3dv(Vector3(ds2, ds, 1).mult3(coeff).toArray(v));
			}
			glEnd();
		} else {
			double v[3];
			glColor4f(0.0, 0.0, 0.0, 1.0);
			glBegin(GL_LINES);
			for (int i = 0; i < curve.sizeSegments() - 1; i++) {
				glVertex3dv(curve.points[curve.segments[i]].toArray(v));
				glVertex3dv(curve.points[curve.segments[i + 1]].toArray(v));
			}
			glEnd();
		}
	}*/
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_COLOR_MATERIAL);
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

bool GameObj::checkCollision(GameObj obj) {
	return boundingBox.collide(obj.boundingBox);
}

bool GameObj::checkCollision(BoundingBox2D b) {
	return boundingBox.collide(b); 
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

bool Camera::hasMoved() {
	bool t = has_moved;
	has_moved = false;
	return t;
}

Mat4x4 translation(double dx, double dy, double dz) {
	double a[] = {
		1, 0, 0, dx, 
		0, 1, 0, dy, 
		0, 0, 1, dz, 
		0, 0, 0, 1};
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


BoundingBox2D::BoundingBox2D(){
	position = Vector2();
	dimensions = Vector2();
	inside = true;
}

BoundingBox2D::BoundingBox2D(double dx, double dy) {
	position = Vector2();
	dimensions = Vector2(dx, dy);
	inside = true;
}

BoundingBox2D::BoundingBox2D(double dx, double dy, Vector2 v) {
	position = Vector2(v.x, v.y);
	dimensions = Vector2(dx, dy);
	inside = true;
}

bool BoundingBox2D::isEmpty() { return (dimensions.x == 0.0 && dimensions.y == 0.0 && position.x == 0.0 && position.y == 0.0); }

bool BoundingBox2D::collide(Vector2 v) {
	Vector2 t = v - position;
	bool res = (abs(t.x) < dimensions.x && abs(t.y) < dimensions.y);
	if (inside)	return (res);
	else return !(res);
}

bool BoundingBox2D::collide(BoundingBox2D b) {
	Vector2 ab = position - b.position;
	bool res = ((abs(ab.x) < dimensions.x + b.dimensions.x) && (abs(ab.y) < dimensions.y + b.dimensions.y));
	if (b.inside)	return (res);
	else return (!res);
}

double BoundingBox2D::getX() { return position.x; }
double BoundingBox2D::getY() { return position.y; }
void BoundingBox2D::setOutside() { inside = false; }
void BoundingBox2D::setInside() { inside = true; }
