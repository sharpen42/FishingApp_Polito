#include "GameObj.h"
#include <GL/glut.h>

#define CURVE_DEF 10
#define face_n(x) (((x) - ((x) % 9)) / 9)

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

BoundingSphere2D GameObj::setBoundingSphere2D(double r) {
	BoundingSphere2D t = boundingSphere;
	if (mesh.isEmpty()) return t;
	boundingSphere = BoundingSphere2D(r);
	return t;
}

BoundingSphere2D GameObj::setBoundingSphere2D(BoundingSphere2D b) {
	BoundingSphere2D t = boundingSphere;
	boundingSphere = b;
	return t;
}

void GameObj::placeBoundingSphere(Vector2 v) { boundingSphere.position.set(v); }
void GameObj::placeBoundingSphere(float x, float y) { boundingSphere.position.set(x, y); }

const char* GameObj::getName(){
	return name.c_str();
}


void GameObj::place(Vector3 p) {
	transform.position.set(p.x, p.y, p.z);
}

void GameObj::place(float x, float y, float z) {
	transform.position.set(x, y, z);
}

void GameObj::translate(Vector3 t){
	transform.position.sum(t.x, t.y, t.z);
}

void GameObj::translate(float x, float y, float z){
	transform.position.sum(x, y, z);
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
	transform.scale.set(1.0, 1.0, 1.0);
	transform.rotation.set(0, 0, 0);
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
	transform.move(deltaTime);
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
		for (int i = 0; i < V; i += 3) {
			double p[3] = { 0.0, 0.0, 0.0 };
			int vertexIndex = mesh.faces[i];
			int uvIndex = mesh.faces[i + 1];
			int normIndex = mesh.faces[i + 2];
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
				glTexCoord2d(mesh.uvs[uvIndex].x, 1 - mesh.uvs[uvIndex].y);
			glNormal3dv(mesh.norms[normIndex].toArray(p));
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
	/* // Show bounding sphere
	if (!boundingSphere.isEmpty()) {
		glPushMatrix();
		glColor3i(255, 0, 0);
		glTranslated(boundingSphere.position.x, boundingSphere.position.y, 0);
		glutWireSphere(boundingSphere.r, 6, 4);
		glPopMatrix();
	}
	*/
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
	// uso delle bounding spheres 'dummy' preallocate, i cui attributi vengono assegnati ad ogni chiamata
	static BoundingSphere2D aa = BoundingSphere2D();
	static BoundingSphere2D bb = BoundingSphere2D();
	// si può implementare un adattamento alle rotazioni dei due oggetti
	//position.set(obj.boundingSphere.position * rotation2D(obj.rot.z) + obj.transform.position.xy());
	aa.r = boundingSphere.r;
	aa.position.set(boundingSphere.position * Mat3x3::rotation(transform.rotation.z) + transform.position.xy());
	bb.r = obj.boundingSphere.r;
	bb.position.set(obj.boundingSphere.position * Mat3x3::rotation(obj.transform.rotation.z) + obj.transform.position.xy());
	return aa.collide(bb);
	/*
	* // correzione della posizione dell'oggetto
	* // si corregge sempre la posizione di obj
	if (aa.collide(bb)) { 

		(...) 

		return true; 
	} 
	else return false;
	*/
}

bool GameObj::checkCollision(BoundingSphere2D b) {
	// uso una bounding sphere 'dummy' preallocata, i cui attributi vengono assegnati ad ogni chiamata
	static BoundingSphere2D aa = BoundingSphere2D();
	aa.r = boundingSphere.r;
	aa.position.set(boundingSphere.position + transform.position.xy());
	return aa.collide(b);
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

Vector2 Camera::onScreen(Vector3 v) {
	return Vector2();
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


BoundingSphere2D::BoundingSphere2D(){
	position = Vector2();
	r = 0.0;
	inside = true;
}

BoundingSphere2D::BoundingSphere2D(double r0) {
	position = Vector2();
	r = r0;
	inside = true;
}

BoundingSphere2D::BoundingSphere2D(double r0, Vector2 v) {
	position = Vector2(v.x, v.y);
	r = r0;
	inside = true;
}

bool BoundingSphere2D::isEmpty() { return (r == 0.0 && position.x == 0.0 && position.y == 0.0); }

bool BoundingSphere2D::collide(Vector2 v) {
	Vector2 t = position - v;
	if(abs(t.x) <= 1 || abs(t.y) <= 1) return (inside && (t.magnitude() <= r));
	else return (inside && (t.magnitude2() <= r * r));
}

bool BoundingSphere2D::collide(BoundingSphere2D b) {
	Vector2 t = position - b.position;
	if (abs(t.x) <= 1 || abs(t.x) <= 1) return (inside && (t.magnitude() <= r + b.r));
	else return (inside && (t.magnitude2() <= r * r + b.r * b.r));
}

double BoundingSphere2D::getX() { return position.x; }
double BoundingSphere2D::getY() { return position.y; }
void BoundingSphere2D::setOutside() { inside = false; }
void BoundingSphere2D::setInside() { inside = true; }
