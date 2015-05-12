
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <SDL_surface.h>
#include "Physics.h"
#include "Graphic.h"
#include <sstream>
#include <SDL_mixer.h>
#include <ctime>
using namespace std;
#include "Object.h"

#define bulvel 1.2
#define adlen 0.03
#define FIXED_TIMESTEP 0.033333f
#define TILE_SIZE 0.075
#define TO_RADIANS 0.0174532925

const int BPCOUNT[] = { 1, 1, 2, 2, 3, 4, 3, 2, 5 };
const int BPEACH[] = { 2, 3, 2, 3, 2, 2, 3, 5, 2 };
const int BP1[] = { 6, 2, 4, 5, 7, 3 };
const int BP2[] = { 6, 2, 4, 7, 6, 3, 2, 7, 3, 4, 3, 5 };
const int BP3[] = { 2, 3, 6, 4, 6, 7, 2, 5, 7, 4, 5, 6, 3, 4, 7, 3, 5, 2 };
const int BP4[] = { 2, 4, 6, 7, 2, 5, 6, 7, 3, 4, 5, 6, 2, 4, 5, 7, 2, 3, 5, 6, 3, 4, 5, 7 };
const int BP5[] = { 2, 4, 5, 6, 7, 2, 3, 4, 5, 6, 2, 3, 4, 6, 7, 2, 3, 5, 6, 7, 3, 4, 5, 6, 7, 2, 3, 4, 5, 7 };

float toRads(float deg){
	return deg * 3.14 / 180;
}
void setPerspective(float fov, float aspect, float zNear, float zFar) {

	Matrix m;
	m.identity();
	m.m[0][0] = 1.0f / tanf(fov*TO_RADIANS / 2.0) / aspect;
	m.m[1][1] = 1.0f / tanf(fov*TO_RADIANS / 2.0);
	m.m[2][2] = (zFar + zNear) / (zNear - zFar);
	m.m[3][2] = (2.0f*zFar*zNear) / (zNear - zFar);
	m.m[2][3] = -1.0f;
	m.m[3][3] = 0.0f;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(m.ml);
}
Matrix::Matrix(){
	for (int i = 0; i < 16; i++){
		ml[i] = 0; 
	}
}
void Matrix::identity(){
	for (int i = 0; i < 4; ++i){
		m[i][i] = 1;
	}
}
Matrix Matrix::operator * (const Matrix &m2){
	Matrix a;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			float p = 0;
			for (int k = 0; k < 4; k++){
				p += m[i][k] * m2.m[k][j];
			}
			a.m[i][j] = p;
		}
	}
	return a;
}

Vector::Vector(){
	x = 0;
	y = 0;
	z = 0;
}
Vector::Vector(float ax, float ay, float az){
	x = ax;
	y = ay;
	z = az;
}
float Vector::dotprod(float bx, float by){
	return x * bx + y * by;
}
float Vector::length(){
	return sqrt(x * x + y * y);
}
void Vector::normalize(){
	x = x / length();
	y = y / length();
	z = z / length();
}
float Vector::angle(){
	return atan(toRads(y / x));
}
Vector Vector::operator* (const Matrix& mat){
	float nx;
	float ny;
	float nz;
	nx = x * mat.m[0][0] + y * mat.m[1][0] + z * mat.m[2][0] + mat.m[3][0];
	ny = x * mat.m[0][1] + y * mat.m[1][1] + z * mat.m[2][1] + mat.m[3][1];
	nz = x * mat.m[0][2] + y * mat.m[1][2] + z * mat.m[2][2] + mat.m[3][2];
	Vector nv(nx, ny, nz);
	return nv;
}

ParticleEmitter::ParticleEmitter(unsigned int particleCount){
	maxLifetime = 100;
	for (int i = 0; i < particleCount; i++){
		float x = rand() / RAND_MAX - 0.5;
		float y = rand() / RAND_MAX - 0.5;
		Particle p;
		Vector pvel(x, y, 0);
		p.velocity = pvel;
	}
}
ParticleEmitter::ParticleEmitter(float x, float y, int b){
	maxLifetime = 1000;
	position.x = x;
	position.y = y;
	behavior = b;
	int max = 20;
	int divisor;
	if (behavior == 0 || behavior == 1){
		divisor = 40;
	}
	else if (behavior == 2){
		divisor = 10;
	}
	if (behavior == 1){
		max = 60;
	}
	for (int i = 0; i < max; i++){
		float angle = rand() % 360;
		float ax = cos(toRads(angle)) / divisor;
		float ay = sin(toRads(angle)) / divisor;
		Particle p;
		Vector pvel(ax, ay, 0);
		p.velocity = pvel;
		Vector ppos(x, y, 0);
		p.position = ppos;
		p.lifetime = -1;
		particles.push_back(p);
	}
}
ParticleEmitter::~ParticleEmitter(){

}
void ParticleEmitter::Update(float elapsed){
	for (int i = 0; i < particles.size(); i++){
		particles[i].position.x += particles[i].velocity.x * elapsed;
		particles[i].position.y += particles[i].velocity.y * elapsed;
		if (particles[i].lifetime < 0 && behavior == 0){
			particles[i].lifetime = rand() % 300;
			particles[i].position.x = position.x;
			particles[i].position.y = position.y;
		}
		else if (particles[i].lifetime < 0 && behavior == 1){
			particles[i].lifetime = rand() % 100;
			float angle = rand() % 360;
			float ax = cos(toRads(angle)) / 80;
			float ay = sin(toRads(angle)) / 80;
			Vector pvel(ax, ay, 0);
			particles[i].velocity = pvel;
			particles[i].position.x = position.x - particles[i].velocity.x / elapsed;
			particles[i].position.y = position.y - particles[i].velocity.y / elapsed;
			particles[i].velocity.x = (position.x - particles[i].position.x) / particles[i].lifetime / elapsed;
			particles[i].velocity.y = (position.y - particles[i].position.y) / particles[i].lifetime / elapsed;
		}
		else if (particles[i].lifetime < 0 && behavior == 2){
			particles[i].lifetime = rand() % 30;
			particles[i].position.x = position.x;
			particles[i].position.y = position.y;
		}
		particles[i].lifetime--;
	}
}
void ParticleEmitter::Render(){
	std::vector<float> particleVertices;
	std::vector<float> particleColors;
	for (int i = 0; i < particles.size(); i++) {
		particleVertices.push_back(particles[i].position.x);
		particleVertices.push_back(particles[i].position.y);
		if (behavior == 2){
			particleColors.push_back(1);
			particleColors.push_back(1);
			particleColors.push_back(1);
			particleColors.push_back(1);
			glPointSize(2.0);
		}
		else if (behavior == 1){
			particleColors.push_back(1);
			particleColors.push_back(0);
			particleColors.push_back(0);
			particleColors.push_back(1);
			glPointSize(3.0);
		}
		else{
			particleColors.push_back(0);
			particleColors.push_back(1);
			particleColors.push_back(1);
			particleColors.push_back(1);
			glPointSize(3.0);
		}
	}
	glColorPointer(4, GL_FLOAT, 0, particleColors.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, particleVertices.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glDrawArrays(GL_POINTS, 0, particleVertices.size() / 2);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
void ParticleEmitter::setPos(float x, float y){
	position.x = x;
	position.y = y;
	for (int i = 0; i < particles.size(); i++){
		particles[i].position.x = x;
		particles[i].position.y = y;
	}
}

Entity::Entity(int texture, float xp, float yp)
:tex(texture),
x(xp),
y(yp),
rotation(0),
active(false)
{
}
Entity::~Entity(){

}
void Entity::setPos(float xp, float yp){
	x = xp;
	y = yp;
}
void Entity::move(float xp, float yp){
	x += xp;
	y += yp;
}
bool Entity::isactive(){
	return active;
}
void Entity::activate(){
	active = true;
}
void Entity::deactivate(){
	active = false;
}
float Entity::getx(){
	return x;
}
float Entity::gety(){
	return y;
}
void Entity::incRot(float amt){
	rotation += amt;
}
void Entity::setRot(float amt){
	rotation = amt;
}
float Entity::getRot(){
	return rotation;
}
Matrix Entity::buildTMat(){
	Matrix rotmat;
	rotmat.identity();
	rotmat.m[0][0] = cos(toRads(rotation));
	rotmat.m[0][1] = sin(toRads(rotation));
	rotmat.m[1][1] = cos(toRads(rotation));
	rotmat.m[1][0] = -sin(toRads(rotation));
	Matrix transmat;
	transmat.identity();
	transmat.m[3][0] = x;
	transmat.m[3][1] = y;
	Matrix transform = rotmat * transmat;
	return transform;
}


Sphere::Sphere(GLuint tex, float rad, float x, float y)
	:Entity(tex, x, y)
{
	radius = rad;
}
void Sphere::Update(){
	incRot(5);
}
void Sphere::RenderSphere(){
	glEnable(GL_TEXTURE_2D);
	if (tex != 58){
		bool a = true;
	}
	glBindTexture(GL_TEXTURE_2D, tex);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	setPerspective(90, 1.33, 0.1, 10);

	glMultMatrixf(buildYaw().ml);
	float t = (1.0 + sqrt(5.0)) / 2.0;


	float vertices[36] = { -1.0, t, 0,
		1.0, t, 0,
		-1.0, -t, 0,
		1.0, -t, 0,
		0, -1.0, t,
		0, 1, t,
		0, -1, -t,
		0, 1, -t,
		t, 0, -1,
		t, 0, 1,
		-t, 0, -1,
		-t, 0, 1,
	};


	float colors[36] = { 1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.0f, 0.5f ,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.0f, 0.5f };
	unsigned int indices[60] = {
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,
		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,
		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,
		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};


	float spriteWidth = 1.0;
	float spriteHeight = 1.0;
	GLfloat quadUVs[] = { 0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0
	};
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, indices);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}
void Sphere::RenderCube(){
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	setPerspective(90, 1.33, 0.1, 10);

	glMultMatrixf(buildYaw().ml);


	float vertices[24] = { -1.0, -1.0, 1.0,
		1.0, -1.0, 1.0,
		1.0, 1.0, 1.0,
		-1.0, 1.0, 1.0,
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0, 1.0, -1.0,
		-1.0, 1.0, -1.0 };


	float colors[24] = { 1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.0f, 0.5f };
	unsigned int indices[36] = {
		0, 1, 2,
		2, 3, 0,
		3, 2, 6,
		6, 7, 3,
		7, 6, 5,
		5, 4, 7,
		4, 5, 1,
		1, 0, 4,
		4, 0, 3,
		3, 7, 4,
		1, 5, 6,
		6, 2, 1 };

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colors);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glPopMatrix();
}

Matrix Sphere::buildYaw(){
	Matrix scalmat;
	scalmat.identity();
	scalmat.m[0][0] = radius;
	scalmat.m[1][1] = radius;
	scalmat.m[2][2] = radius;
	Matrix rotmatx;
	rotmatx.identity();
	rotmatx.m[1][1] = cos(toRads(getRot()));
	rotmatx.m[1][2] = sin(toRads(getRot()));
	rotmatx.m[2][2] = cos(toRads(getRot()));
	rotmatx.m[2][1] = -sin(toRads(getRot()));
	Matrix rotmaty;
	rotmaty.identity();
	rotmaty.m[0][0] = cos(toRads(getRot()));
	rotmaty.m[0][2] = -sin(toRads(getRot()));
	rotmaty.m[2][2] = cos(toRads(getRot()));
	rotmaty.m[2][0] = sin(toRads(getRot()));
	Matrix rotmatz;
	rotmatz.identity();
	rotmatz.m[0][0] = cos(toRads(getRot()));
	rotmatz.m[0][1] = sin(toRads(getRot()));
	rotmatz.m[1][1] = cos(toRads(getRot()));
	rotmatz.m[1][0] = -sin(toRads(getRot()));
	Matrix transmat;
	transmat.identity();
	transmat.m[3][0] = getx();
	transmat.m[3][1] = gety();
	transmat.m[3][2] = -1;
	Matrix rotmat = rotmatx * rotmaty * rotmatz;
	Matrix transform = scalmat * rotmat * transmat;
	return transform;
}
void Sphere::setRad(float i){
	radius = i;
}


Square::Square(int texture, float x, float y, float h, float w)
: Entity(texture,x, y)
{
	height = h;
	width = w;
}
void Square::reinit(int texture, float x, float y, float h, float w){
	tex = texture;
	setPos(x, y);
	height = h;
	width = w;
}
vector<Vector> Square::getVerts(){
	Matrix transform = buildTMat();
	Vector v1(width, height, 0);
	v1 = v1 * transform;
	Vector v2(width, -height, 0);
	v2 = v2 * transform;
	Vector v3(-width, height, 0);
	v3 = v3 * transform;
	Vector v4(-width, -height, 0);
	v4 = v4 * transform;
	vector<Vector> vs = { v1, v2, v3, v4 };
	return vs;
}
bool Square::overlaps(Square& b){
	Vector n1(cos(toRads(getRot())), sin(toRads(getRot())), 0);
	Vector n2(cos(toRads(getRot() + 90)), sin(toRads(getRot()) + 90), 0);
	n1.normalize();
	n2.normalize();
	vector<Vector> vs1 = getVerts();
	vector<Vector> vs2 = b.getVerts();

	float vn1max = FLT_MIN;
	float vn1min = FLT_MAX;
	float vn2max = FLT_MIN;
	float vn2min = FLT_MAX;
	float vc1 = n1.dotprod(getx(), gety());
	float vc2 = n1.dotprod(b.getx(), b.gety());
	for (int i = 0; i < vs1.size(); i++){
		float n = n1.dotprod(vs1[i].x, vs1[i].y);
		if (n > vn1max){
			vn1max = n;
		}
		if (n < vn1min){
			vn1min = n;
		}
	}
	for (int i = 0; i < vs2.size(); i++){
		float n = n1.dotprod(vs2[i].x, vs2[i].y);
		if (n > vn2max){
			vn2max = n;
		}
		if (n < vn2min){
			vn2min = n;
		}
	}
	float p;
	p = abs(vc1 - vc2) - (abs(vn1max - vn1min) + abs(vn2max - vn2min)) / 2;
	if (p >= 0){
		return false;
	}
	vn1max = FLT_MIN;
	vn1min = FLT_MAX;
	vn2max = FLT_MIN;
	vn2min = FLT_MAX;
	vc1 = n2.dotprod(getx(), gety());
	vc2 = n2.dotprod(b.getx(), b.gety());
	for (int i = 0; i < vs1.size(); i++){
		float n = n2.dotprod(vs1[i].x, vs1[i].y);
		if (n > vn1max){
			vn1max = n;
		}
		if (n < vn1min){
			vn1min = n;
		}
	}
	for (int i = 0; i < vs2.size(); i++){
		float n = n2.dotprod(vs2[i].x, vs2[i].y);
		if (n > vn2max){
			vn2max = n;
		}
		if (n < vn2min){
			vn2min = n;
		}
	}
	p = abs(vc1 - vc2) - (abs(vn1max - vn1min) + abs(vn2max - vn2min)) / 2;
	if (p >= 0){
		return false;
	}

	return true;
}
void Square::Draw(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	Matrix transform;
	transform = buildTMat();
	glMultMatrixf(transform.ml);

	GLfloat quad[] = { -width, height, -width, -height, width, -height, width, height };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);



	float spriteWidth = 1.0;
	float spriteHeight = 1.0;
	GLfloat quadUVs[] = { 0, 0, 0, 1, 1, 1, 1, 0
	};
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices.data());

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();
}
void Square::resize(float h, float w){
	height = h;
	width = w;
}


Circle::Circle(int texture, float x, float y, float rad)
:Entity(texture, x, y)
{
	radius = rad;
}
void Circle::reinit(int texture, float x, float y, float rad){
	radius = rad;
	setPos(x, y);
	tex = texture;
}
bool Circle::overlaps(Square& b){
//handles both circle/square and square circle
	vector<Vector> verts = b.getVerts();
	//Circle-Square
	Vector closest;
	float dis = FLT_MAX;
	for (int i = 0; i < verts.size(); i++){
		Vector v(getx() - verts[i].x, gety() - verts[i].y, 0);
		if (v.length() < dis){
			dis = v.length();
			closest = verts[i];
		}
	}
	Vector testaxis(getx() - closest.x, gety() - closest.y, 0);
	testaxis.normalize();
	float sc1 = testaxis.dotprod(b.getx(), b.gety());
	float sc2 = testaxis.dotprod(getx(), gety());
	float sv1 = testaxis.dotprod(closest.x, closest.y);
	float p;
	p = abs(sc1 - sc2) - (abs(sv1 - sc1) + abs(sc2 - radius));
	if (p >= 0){
		return false;
	}
	//Square-Circle
	Vector n1(cos(toRads(getRot())), sin(toRads(getRot())), 0);
	Vector n2(cos(toRads(getRot() + 90)), sin(toRads(getRot()) + 90), 0);
	n1.normalize();
	n2.normalize();

	float vnmax = FLT_MIN;
	float vnmin = FLT_MAX;
	for (int i = 0; i < verts.size(); i++){
		float n = n1.dotprod(verts[i].x, verts[i].y);
		if (n > vnmax){
			vnmax = n;
		}
		if (n < vnmin){
			vnmin = n;
		}
	}
	float vc = n1.dotprod(getx(), gety());
	float vs = n1.dotprod(b.getx(), b.gety());
	p = abs(vc - vs) - (abs(vnmax - vnmin) + abs(2 * radius)) / 2;
	if (p >= 0){
		return false;
	}

	vnmax = FLT_MIN;
	vnmin = FLT_MAX;
	for (int i = 0; i < verts.size(); i++){
		float n = n2.dotprod(verts[i].x, verts[i].y);
		if (n > vnmax){
			vnmax = n;
		}
		if (n < vnmin){
			vnmin = n;
		}
	}
	vc = n2.dotprod(getx(), gety());
	vs = n2.dotprod(b.getx(), b.gety());
	p = abs(vc - vs) - (abs(vnmax - vnmin) + abs(2 * radius)) / 2;
	if (p >= 0){
		return false;
	}

	return true;
}
bool Circle::overlaps(Circle& b){
	float totalrad = radius + b.radius;
	Vector cdis(getx() - b.getx(), gety() - b.gety(), 0);
	float cdisf = cdis.length();
	return(cdisf < totalrad);
}
void Circle::Draw(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	Matrix transform;
	transform = buildTMat();
	glMultMatrixf(transform.ml);

	GLfloat quad[] = { -radius, radius, -radius, -radius, radius, -radius, radius, radius };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);



	float spriteWidth = 1.0;
	float spriteHeight = 1.0;
	GLfloat quadUVs[] = { 0, 0, 0, 1, 1, 1, 1, 0
	};
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices.data());

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();
}
void Circle::setRad(float r){
	radius = r;
}
float Circle::getRad(){
	return radius;
}


sBullet::sBullet(int tex, float xp, float yp, int t, int behavior, float w, float h)
:Square(tex, xp, yp, w, h), pe(0, 0, 2)
{
	tier = t;
	movtype = behavior;
	float vx = rand() % 360 - 180;
	float vy = rand() % 360 - 180;
	Vector d(vx, vy, 0);
	d.normalize();
	d.x = d.x / 120 + (1 / 360);
	d.y = d.y / 120 + (1 / 360);
	dir = d;
	mult = 1;
}
void sBullet::Update(){
	if (isactive()){
	move(dir.x * mult, dir.y * mult);
	}
	else if (pframes > 0){
		pe.Update(FIXED_TIMESTEP);
		pe.Render();
		pframes--;
	}
	mult = 1;
}
void sBullet::applyEffect(int i){
	if (i == 1 && tier <= 2){
		mult *= 0.5;
	}
	if (i == 8 && tier < 2 ){
		
	}
}
void sBullet::Render(){
	if (isactive()){
		Square::Draw();
	}
}
void sBullet::Orient(float angle){
	incRot(angle - getRot());
}
int sBullet::getTier(){
	return tier;
}
void sBullet::explode(){
	pe.setPos(getx(), gety());
	pframes = 50;
}
	

float cBullet::tSize(int tier){
	if (tier == 1){
		reinit(tex, getx(), gety(), 0.015);
		return 0.015;
	}
	else if (tier == 2){
		reinit(tex, getx(), gety(), 0.04);
		return 0.04;
	}
	else if (tier == 3){
		reinit(tex, getx(), gety(), 0.08);
		return 0.08;
	}
}
Vector cBullet::tmove(int movt){
	Vector d(0, 0, 0);
	if (movtype == 0){
		d.x = 0;
		d.y = FIXED_TIMESTEP * bulvel * 2;
	}
	else if (movtype == 1){
		d.x = 0;
		d.y = FIXED_TIMESTEP * bulvel * 1.4;
	}
	else if (movtype == 2 || movtype == 3){
		if (tier == 1){
			reinit(tex, getx(), gety(), 0.015);
		}
		else if (tier == 2){
			reinit(tex, getx(), gety(), 0.04);
		}
		else if (tier == 3){
			reinit(tex, getx(), gety(), 0.08);
		}
		health = 1;
		Vector o(-getx(), -gety(), 0);
		if (getx() < 0){
			o.x = -0.6 - getx();
		}
		else{
			o.x = 0.6 - getx();
		}
		o.normalize();
		int ang = (rand() % 120 - 60);
		float div = 3.0;
		if (movtype == 3){
			div = 1.5;
		}
		d.x = (cos(acos(o.x) + toRads(ang))) * FIXED_TIMESTEP * bulvel / div;
		d.y = (sin(asin(o.y) + toRads(ang))) * FIXED_TIMESTEP * bulvel / div;
	}
	else if (movtype == 4){//downward falling
		if (tier == 1){
			reinit(tex, getx(), gety(), 0.015);
		}
		else if (tier == 2){
			reinit(tex, getx(), gety(), 0.04);
		}
		else if (tier == 3){
			reinit(tex, getx(), gety(), 0.08);
		}
		health = 1;
		int ang = 270 + (rand() % 30 - 15);
		d.x = (cos(toRads(ang))) * FIXED_TIMESTEP * bulvel / 3.3;
		d.y = (sin(toRads(ang))) * FIXED_TIMESTEP * bulvel / 3.3;
	}
	else if (movtype == 6){
		d.x = FIXED_TIMESTEP * bulvel / 4;
		d.y = FIXED_TIMESTEP * bulvel / 4;
	}
	return d;
}
cBullet::cBullet(int tex, float xp, float yp, int t, int behavior,Vector vel)
:Circle(tex, xp, yp, 0), pe(0, 0, 2)
{
	stagged = false;
	aframes = 0;
	health = 1;
	tier = t;
	movtype = behavior;
	pframes = 0;
	Vector d = vel;
	d.normalize();
	if (movtype == 0){
		setRad(0.03);
		d.x = 0;
		d.y = d.y * FIXED_TIMESTEP * bulvel * 2;
	}
	else if (movtype == 1){
		health = 7;
		setRad(0.06);
		tSize(tier);
		d.y = d.y * FIXED_TIMESTEP * bulvel * 1.4;
	}
	else if (movtype == 2){
		health = 1;
		tSize(tier);
		vel.normalize();
		d.x = vel.x * FIXED_TIMESTEP * bulvel / 3;
		d.y = vel.y * FIXED_TIMESTEP * bulvel / 3;
	}
	else if (movtype == 3){
		health = 1;
		tSize(tier);
		vel.normalize();
		d.x = vel.x * FIXED_TIMESTEP * bulvel / 1.5;
		d.y = vel.y * FIXED_TIMESTEP * bulvel / 1.5;
	}
	else if (movtype == 4){
		tSize(tier);
		health = 1;
		vel.normalize();
		d.x = vel.x * FIXED_TIMESTEP * bulvel / 2;
		d.y = vel.y * FIXED_TIMESTEP * bulvel / 2;
	}
	else if (movtype == 5){
		setRad(0.03);
		health = 1;
		d.x = vel.x * FIXED_TIMESTEP * bulvel * 3.5;
		d.y = vel.y * FIXED_TIMESTEP * bulvel * 3.5;
	}
	else if (movtype == 6){
		d.x = vel.x * FIXED_TIMESTEP * bulvel / 4;
		d.y = vel.y *  FIXED_TIMESTEP * bulvel / 4;
	}
	dir = d;
}
void cBullet::setDir(Vector vel){
	dir.x = vel.x;
	dir.y = vel.y;
}
cBullet::cBullet(int tex,float xp, float yp, int t, int behavior)
:Circle(tex, xp, yp, 0), pe(0, 0, 2)
{
	stagged = false;
	aframes = 0;
	health = 1;
	tier = t;
	movtype = behavior;
	pframes = 0;
	Vector d(0, 0, 0);
	if (movtype == 0){
		setRad(0.03);
		d.x = 0;
		d.y = FIXED_TIMESTEP * bulvel * 2;
	}
	else if (movtype == 1){
		health = 7;
		setRad(0.06);
		d.x = 0;
		d.y = FIXED_TIMESTEP * bulvel * 1.4;
	}
	else{
		tSize(tier);
	}
	dir = tmove(movtype);
}
void cBullet::Orient(float angle){
	incRot(angle - getRot());
}
void cBullet::Update(){
	if (isactive()){
		move(dir.x * mult, dir.y * mult);
		if (health < 1){
			deactivate();
		}
		mult = 1;
	}
}
void cBullet::Update(float tx, float ty){
	if (isactive() && movtype == 6){
		aframes++;
		float spd = dir.length();
		dir.x = tx - getx();
		dir.y = ty - gety();
		dir.normalize();
		dir.x *= FIXED_TIMESTEP*bulvel / 6;
		dir.y *= FIXED_TIMESTEP * bulvel / 6;
		move(dir.x * mult, dir.y * mult);
		if (health < 1){
			deactivate();
		}
		else if (aframes > 300){
			explode();
			deactivate();
		}
		mult = 1;
	}
	else{
		Update();
	}
}
void cBullet::applyEffect(int i){
	if (i == 1 && tier <= 2){
		mult *= 0.5;
	}
	if (i == 2 && tier < 2){
		mult *= 2;
	}
	if (i == 8 && !stagged && tier < 2){
		if (abs(dir.y) < abs(dir.x)){
			dir.y *= 0;
			dir.x *= 1.2;
		}
		else{
			dir.x *= 0;
			dir.y *= 1.2;
		}
		stagged = true;
	}
}
void cBullet::Render(){
	if (isactive() == true){
		Circle::Draw();
	}
	else if (pframes > 0){
		pe.Update(FIXED_TIMESTEP);
		pe.Render();
		pframes--;
	}
}
int cBullet::damage(int i){
	health = -i;
	if (health < 1){
		deactivate();
	}
	return health;
}
void cBullet::change(GLuint texture, int t, int beh){
	tex = texture;
	stagged = false;
	aframes = 0;
	tier = t;
	movtype = beh;
	pframes = -1;
	if (movtype == 0){ //standard player
		reinit(tex, getx(), gety(), 0.03);
		health = 1;
	}
	else if (movtype == 1){ //Ariel Orb
		reinit(tex, getx(), gety(), 0.06);
		health = 7;
	}
	else if (movtype == 5){
		reinit(tex, getx(), gety(), 0.03);
		health = 1;
	}
	else {
		tSize(tier);
	}
	dir = tmove(movtype);
}
int cBullet::getHealth(){
	return health;
}
void cBullet::explode(){
	pe.setPos(getx(), gety());
	pframes = 50;
}
int cBullet::getTier(){
	return tier;
}

Enemy::Enemy(int tex, float xp, float yp, int t)
:Circle(tex, xp, yp, 0.05)
{
	pnum = 0;
	if (t == 0){
		float ang = rand() % 360;
		Vector d(cos(ang), sin(ang), 0);
		d.normalize();
		d.x = d.x / 120;
		d.y = d.y/120;
		dir = d;
		health = 3;
		firethresh = 30;
	}
	if (t == 1){
		health = 1000;
		firethresh = 50;
		setRad(0.2);
	}
	if (t == 2){
		health = 1500;
		pnum = 27;
		setRad(0.6);
		firethresh = 50;
	}
	tier = t;
	framesalive = 0;
	lastfireframe = 0;
	lastfireframe2 = 0;
}
void Enemy::Update(){
	if (isactive() && tier == 0){
		lastfireframe++;
		framesalive ++;
		move(dir.x, dir.y);
		if (tier == 0 && (!(getx() > -1.2 && getx() < -0.1) && !(getx() < 1.2 && getx() > 0.1))){
			dir.x *= -1;
		}
		if (tier == 0 && (gety() < -0.8 || gety() > 0.9)){
			dir.y *= -1;
		}
	}
	else if (isactive() && tier == 1){
		lastfireframe++;
		lastfireframe2++;
		framesalive++;
		float t = mapValue(framesalive, 0, 100, 0, 1);
		float ny = easeOut(0.9, 0.3, t);
		setPos(getx(), ny);
	}
	else if (isactive() && tier == 2){
		lastfireframe++;
		lastfireframe2++;
		framesalive++;
		float t = mapValue(framesalive, 0, 200, 0, 1);
		float ny = easeOut(1.3, 1.2, t);
		setPos(getx(), ny);
	}
}
void Enemy::Render(){
	if (isactive()){
		Draw();
	}
}
void Enemy::shoot(vector<cBullet>& cbullets, vector<sBullet>& sbullets, queue<int>& cBuck, queue<int>& sBuck, vector<GLuint> tex, Vector target, PTransit& ptrans){
	if (lastfireframe > firethresh && isactive()){
		if (tier == 0){
			if (cBuck.size() != 0 && cbullets.size() - cBuck.size() < 50){
				int index = cBuck.front();
				cBuck.pop();
				GLuint bt;
				if (getx() > 0){
					bt = tex[3];
				}
				else{
					bt = tex[4];
				}
				cbullets[index].setPos(getx(), gety());
				cbullets[index].change(bt, 1, 2);
				cbullets[index].activate();
			}
			else if (cbullets.size() < 50){
				GLuint bt;
				if (getx() > 0){
					bt = tex[3];
				}
				else{
					bt = tex[4];
				}
				cBullet c(bt, getx(), gety(), 1, 2);
				c.activate();
				cbullets.push_back(c);
			}
			lastfireframe = 0;
			if (firethresh > 15){
				firethresh--;
			}
		}	
	}
	if (tier >= 1 && isactive()){
		if (lastfireframe > 5 && bulletcham.size() > 0){
			ptrans.add(Vector(getx(), gety(), 0), bulletcham.front());
			bulletcham.pop();
			lastfireframe = 0;
		}
		if (lastfireframe2 > 200 && pnum < 54){
			int setindex = pnum / 6;
			int ceach = BPEACH[setindex]; // how many of each spawn
			int ccount = BPCOUNT[setindex]; //number of sets per wave
			int snum = pnum % 6;
			for (int i = 0; i < ccount; i++){
			for (int j = 0; j < ceach; j++){
			if (ccount == 1){
				bulletcham.push( BP1[snum] + 1);
			}
			else if (ccount == 2){
				bulletcham.push(BP2[ccount * snum + i] + 1);
			}
			else if (ccount == 3){
				bulletcham.push(BP3[ccount * snum + i] + 1);
			}
			else if (ccount == 4){
				bulletcham.push(BP4[ccount * snum + i] + 1);
			}
			else if (ccount == 5){
				bulletcham.push(BP5[ccount * snum + i] + 1);
			}
			}
			}
			lastfireframe2 = 0;
			pnum++;
		}
	}
	if (tier == 2 && isactive() && pnum >= 54){
		if (lastfireframe2 > 100){
			for (int i = 3; i <= 8; i++){
				bulletcham.push(i);
			}
		}
	}
}
int Enemy::hit(vector<cBullet>& bullets, PTransit& ptrans){
	if (isactive()){
		for (int i = 0; i < bullets.size(); i++){
			if (bullets[i].isactive() && overlaps(bullets[i])){
				if (tier == 1){
					for (int j = 0; j < bullets[i].getHealth(); j++){
						float p = mapValue(rand(), 0, RAND_MAX, 0, 5);
						if (p > 3){
							ptrans.add(Vector(getx(), gety(), 0));
						}
					}
				}
				else if (tier == 2){
					for (int j = 0; j < bullets[i].getHealth(); j++){
						float p = mapValue(rand(), 0, RAND_MAX, 0, 5);
						if (p > 1){
							ptrans.add(Vector(getx(), gety(), 0));
						}
					}
				}
				damage(bullets[i].getHealth());
				bullets[i].damage(health);
				return i;
			}
		}
	}
	return -1;
}
int Enemy::explode(vector<cBullet>& cbullets, vector<sBullet>& sbullets, queue<int>& cBuck, queue<int>& sBuck, PTransit& ptrans){
	int score = (tier + 1) * 50;
	deactivate();
	Circle killcircle(0, getx(), gety(), 0.2);
	for (int i = 0; i < cbullets.size(); i++){
		if (killcircle.overlaps(cbullets[i]) && cbullets[i].isactive() && cbullets[i].getTier() <= 1){
			cbullets[i].deactivate();
			cbullets[i].explode();
			cBuck.push(i);
			score += 10;
			ptrans.add(Vector(cbullets[i].getx(), cbullets[i].gety(), 0));
		}
	}
	for (int i = 0; i < sbullets.size(); i++){
		if (killcircle.overlaps(sbullets[i]) && sbullets[i].isactive() && cbullets[i].getTier() <= 1){
			sbullets[i].deactivate();
			sbullets[i].explode();
			sBuck.push(i);
			score += 10;
			ptrans.add(Vector(sbullets[i].getx(), sbullets[i].gety(), 0));
		}
	}
	for (int i = 0; i < 3; i++){
		Vector v(getx(), gety(), 0);
		ptrans.add(v);
	}
	return score;
}
void Enemy::reinit(int tex, float xp, float yp, int t){
	int nrad = 0.05;
	if (t == 0){
		float ang = rand() % 360;
		Vector d(cos(ang), sin(ang), 0);
		d.normalize();
		d.x = d.x / 120;
		d.y = d.y/120;
		dir = d;
		health = 3;
		firethresh = 30;
	}
	tier = t;
	framesalive = 0;
	lastfireframe = 0;
	setPos(xp, yp);
	activate();
}
int Enemy::damage(int i){
	health -= i;
	return health;
}
int Enemy::getHealth(){
	return health;
}
int Enemy::getTier(){
	return tier;
}

cField::cField(int texture, float x, float y, float radius, int eff)
	:Circle(texture, x, y, radius), crystal(texture, 0, 0, 0)
{
	aniframes = 0;
	framesactive = 0;
	effect = eff;
	ani = 0;
	activate();
	csound = Mix_LoadWAV("se/charge.wav");
	fsound = Mix_LoadWAV("se/eshot.wav");
}
void cField::applyEffect(std::vector<cBullet>& cbullets, std::vector<sBullet>& sbullets, std::queue<int>& cBuck, std::queue<int>& sBuck, vector<GLuint> btex, Vector target){
	if (isactive() && effect == 7 && ani != 11 && aniframes > 50){
		if (cBuck.size() != 0){
			int index = cBuck.front();
			cBuck.pop();
			GLuint bt;
			if (getx() > 0){
				bt = btex[9];
			}
			else{
				bt = btex[10];
			}
			cbullets[index].setPos(getx(), gety());
			cbullets[index].change(bt, 3, 6);
			cbullets[index].activate();
			beginani(11);
		}
		else if (cbullets.size() < 100){
			GLuint bt;
			if (getx() > 0){
				bt = btex[9];
			}
			else{
				bt = btex[10];
			}
			cBullet c(bt, getx(), gety(), 3, 6);
			c.activate();
			cbullets.push_back(c);
			beginani(11);
		}
		Mix_PlayChannel(-1, fsound, 0);
	}
	if (isactive() && effect == 5){
		for (int i = 0; i < cbullets.size(); i++){
			if (overlaps(cbullets[i]) && cbullets[i].isactive() && cbullets[i].getTier() == 2){
				Vector dir(target.x - cbullets[i].getx(), target.y - cbullets[i].gety(), 0);
				dir.normalize();
				float deg = mapValue(rand(), 0, RAND_MAX, 0, 20) - 10;
				dir.x = cos(acos(dir.x) + toRads(deg)) * bulvel * FIXED_TIMESTEP / 2.5;
				dir.y = sin(asin(dir.y) + toRads(deg)) * bulvel * FIXED_TIMESTEP / 2.5;
				cbullets[i].setDir(dir);
			}
		}
		for (int i = 0; i < sbullets.size(); i++){
			if (overlaps(sbullets[i]) && sbullets[i].isactive() && sbullets[i].getTier() <= 1){
				sbullets[i].applyEffect(effect);
			}
		}
	}
	if (isactive() && effect == 4 && 100 - cbullets.size() + cBuck.size() >=6 && ani == 2 && aniframes > 60){
		for (int i = 0; i < 6; i++){
			float ang = toRads(60 * i);
			if (cBuck.size() != 0){
				int index = cBuck.front();
				cBuck.pop();
				GLuint bt;
				if (getx() > 0){
					bt = btex[3];
				}
				else{
					bt = btex[4];
				}
				cbullets[index].setPos(getx(), gety());
				cbullets[index].change(bt, 2, 2);
				cbullets[index].setDir(Vector(cos(ang) * FIXED_TIMESTEP * bulvel /3, sin(ang) * FIXED_TIMESTEP * bulvel /3, 0));
				cbullets[index].activate();
			}
			else if (cbullets.size() < 100){
				GLuint bt;
				if (getx() > 0){
					bt = btex[3];
				}
				else{
					bt = btex[4];
				}
				cBullet c(bt, getx(), gety(), 2, 2, Vector(cos(ang), sin(ang), 0));
				c.activate();
				cbullets.push_back(c);
			}
		}
		Mix_PlayChannel(-1, fsound, 0);
		beginani(7);
	}
	if (isactive() && effect == 3 && framesactive % 40 == 1){
		vector<Vector> vecs = { Vector(0.5 * FIXED_TIMESTEP * bulvel, 0, 0),
			Vector(0, 0.5 * FIXED_TIMESTEP * bulvel, 0), 
			Vector(0, -0.5 * FIXED_TIMESTEP * bulvel, 0), 
			Vector(-0.5 * FIXED_TIMESTEP * bulvel, 0, 0) };
		for (int i = 0; i < 4; i++){
			if (cBuck.size() != 0 && cbullets.size() - cBuck.size() < 100){
				int index = cBuck.front();
				cBuck.pop();
				GLuint bt;
				if (getx() > 0){
					bt = btex[3];
				}
				else{
					bt = btex[4];
				}
				cbullets[index].setPos(getx(), gety());
				cbullets[index].change(bt, 2, 3);
				cbullets[index].setDir(vecs[i]);
				cbullets[index].activate();
			}
			else if (cbullets.size() < 100){
				GLuint bt;
				if (getx() > 0){
					bt = btex[3];
				}
				else{
					bt = btex[4];
				}
				cBullet c(bt, getx(), gety(), 2, 3, vecs[i]);
				c.activate();
				cbullets.push_back(c);
			}
		}
		Mix_PlayChannel(-1, fsound, 0);
	}
	else if (isactive()){
		for (int i = 0; i < cbullets.size(); i++){
			if (overlaps(cbullets[i]) && cbullets[i].isactive()){
				cbullets[i].applyEffect(effect);
			}
		}
		for (int i = 0; i < sbullets.size(); i++){
			if (overlaps(sbullets[i]) && sbullets[i].isactive()){
				sbullets[i].applyEffect(effect);
			}
		}
	}
}
void cField::applyEffect(std::vector<cBullet>& cbullets, std::vector<sBullet>& sbullets, std::queue<int>& cBuck, std::queue<int>& sBuck){
	if (isactive()){
		for (int i = 0; i < cbullets.size(); i++){
			if (overlaps(cbullets[i]) && cbullets[i].isactive()){
				cbullets[i].applyEffect(effect);
			}
		}
		for (int i = 0; i < sbullets.size(); i++){
			if (overlaps(sbullets[i]) && sbullets[i].isactive()){
				sbullets[i].applyEffect(effect);
			}
		}
	}
}
bool cField::Update(){
	aniframes++;
	if (aniframes < 25 && ani == 1 && isactive()){
		float t = mapValue(aniframes, 0, 25, 0, 1);
		float nrad = easeOut(0, 0.3, t);
		setRad(nrad);
		return true;
	}
	else if (aniframes < 25 && ani == 2 && isactive()){
		float t = mapValue(aniframes, 0, 25, 0, 1);
		float nrad = easeOut(0, 0.2, t);
		setRad(nrad);
		return true;
	}
	else if (aniframes < 25 && ani == 3 && isactive()){
		float t = mapValue(aniframes, 0, 25, 0, 1);
		float nrad = easeOut(0.2, 0, t);
		setRad(nrad);
		return true;
	}
	else if (aniframes < 50 && ani == 4 && isactive()){
		float t = mapValue(aniframes, 0, 50, 0, 1);
		float nrad = easeOut(0, 0.6, t);
		setRad(nrad);
		incRot(10);
		return true;
	}
	else if (aniframes < 100 && ani == 5 && isactive()){
		float t = mapValue(aniframes, 0, 100, 0, 1);
		float nrad = easeOut(0, 1.0, t);
		setRad(nrad);
		incRot(10);
		return true;
	}
	else if (aniframes < 150 && ani == 6 && isactive()){
		float t = mapValue(aniframes, 0, 150, 0, 1);
		float nrad = easeOut(0, 1.4, t);
		setRad(nrad);
		incRot(10);
		return true;
	}
	else if (aniframes < 55 && ani == 7 && isactive()){
		if (aniframes >= 30){
			float t = mapValue(aniframes, 30, 55, 0, 1);
			float nrad = easeOut(0.2, 0, t);
			setRad(nrad);
		}
		return true;
	}
	else if (aniframes < 20 && ani == 8 && isactive()){
		float t = mapValue(aniframes, 0, 20, 0, 1);
		float nrad = easeOut(0, 0.1, t);
		setRad(nrad);
		return true;
	}
	else if (aniframes < 2 && ani == 9 && isactive()){
		setRad(0.04);
		return true;
	}
	else if (aniframes < 2 && ani == 10 && isactive()){
		setRad(0.1);
		return true;
	}
	else if (aniframes < 300 && ani == 11 && isactive()){
		if (aniframes >= 30){
			float t = mapValue(aniframes, 0, 300, 0, 1);
			float nrad = easeOut(0.3, 0, t);
			setRad(nrad);
		}
		return true;
	}


	if (effect == 2 && isactive()){
		framesactive++;
		if (framesactive >= 100){
			Reset();
			return false;
		}
		else if (framesactive == 75){
			beginani(3);
		}
	}
	else if (isactive() && effect == 3){
		if (framesactive == 3){
			Mix_PlayChannel(-1, csound, 0);
		}
		framesactive++;
		if (framesactive > 199){
			Reset();
			return false;
		}
		else if (framesactive == 175){
			beginani(3);
		}
	}
	else if (isactive() && (effect == 0)){
		framesactive++;
		if (framesactive > 100){
			Reset();
			return false;
		}
	}
	else if (isactive() && effect == 4){
		if (framesactive == 3){
			Mix_PlayChannel(-1, csound, 0);
		}
		if (framesactive < 25){
			framesactive++;
		}
		else if (ani == 7){
			framesactive++;
			if (framesactive >= 80){
				Reset();
				return false;
			}
		}
		return true;
	}
	else if (effect == 5 && isactive()){
		framesactive++;
		if (framesactive >= 150){
			Reset();
			return false;
		}
		else if (framesactive == 125){
			beginani(3);
		}
	}
	else if (effect == 6 && isactive()){
		framesactive++;
		if (framesactive < 41){
			float t = mapValue(framesactive, 0, 40, 0, 1);
			float nrad = easeOut(0, 0.07, t);
			setRad(nrad);
		}
		crystal.setRad(getRad());
		crystal.Update();
		crystal.setPos(getx(), gety());
		if (framesactive >= 150){
			Reset();
			return false;
		}
		else if (framesactive >= 125){
			float t = mapValue(framesactive, 125, 150, 0, 1);
			float nrad = easeOut(0.07, 0, t);
			setRad(nrad);
		}
	}
	else if (effect == -2 && isactive()){
		if (aniframes < 21){
			float t = mapValue(aniframes, 0, 20, 0, 1);
			float nrad = easeOut(0, 0.04, t);
			setRad(nrad);
		}
		crystal.setRad(getRad());
		crystal.Update();
		crystal.setPos(getx(), gety());
	}
	else if (effect == 7 && isactive()){
		framesactive++;
		if (framesactive == 3){
			Mix_PlayChannel(-1, csound, 0);
		}
		if (framesactive > 300){
			Reset();
			return false;
		}
	}
	return isactive();
}
bool cField::Update(queue<int>& bucket, int myind){
	aniframes++;
	if (aniframes < 25 && ani == 1 && isactive()){
		float t = mapValue(aniframes, 0, 25, 0, 1);
		float nrad = easeOut(0, 0.3, t);
		setRad(nrad);
		return true;
	}
	else if (aniframes < 25 && ani == 2 && isactive()){
		float t = mapValue(aniframes, 0, 25, 0, 1);
		float nrad = easeOut(0, 0.2, t);
		setRad(nrad);
		return true;
	}
	else if (aniframes < 25 && ani == 3 && isactive()){
		float t = mapValue(aniframes, 0, 25, 0, 1);
		float nrad = easeOut(0.2, 0, t);
		setRad(nrad);
		return true;
	}
	else if (aniframes < 50 && ani == 4 && isactive()){
		float t = mapValue(aniframes, 0, 50, 0, 1);
		float nrad = easeOut(0, 0.6, t);
		setRad(nrad);
		incRot(10);
		return true;
	}
	else if (aniframes < 100 && ani == 5 && isactive()){
		float t = mapValue(aniframes, 0, 100, 0, 1);
		float nrad = easeOut(0, 1.0, t);
		setRad(nrad);
		incRot(10);
		return true;
	}
	else if (aniframes < 150 && ani == 6 && isactive()){
		float t = mapValue(aniframes, 0, 150, 0, 1);
		float nrad = easeOut(0, 1.4, t);
		setRad(nrad);
		incRot(10);
		return true;
	}
	else if (aniframes < 55 && ani == 7 && isactive()){
		if (aniframes >= 30){
			float t = mapValue(aniframes, 30, 55, 0, 1);
			float nrad = easeOut(0.2, 0, t);
			setRad(nrad);
		}
		return true;
	}
	else if (aniframes < 20 && ani == 8 && isactive()){
		float t = mapValue(aniframes, 0, 20, 0, 1);
		float nrad = easeOut(0, 0.1, t);
		setRad(nrad);
		return true;
	}
	else if (aniframes < 2 && ani == 9 && isactive()){
		setRad(0.04);
		return true;
	}
	else if (aniframes < 2 && ani == 10 && isactive()){
		setRad(0.1);
		return true;
	}
	else if (aniframes < 300 && ani == 11 && isactive()){
		if (aniframes >= 30){
			float t = mapValue(aniframes, 0, 300, 0, 1);
			float nrad = easeOut(0.3, 0, t);
			setRad(nrad);
		}
		return true;
	}


	if (effect == 2 && isactive()){
		framesactive++;
		if (framesactive >= 100){
			Reset();
			bucket.push(myind);
			return false;
		}
		else if (framesactive == 75){
			beginani(3);
		}
	}
	else if (isactive() && effect == 3){
		if (framesactive == 3){
			Mix_PlayChannel(-1, csound, 0);
		}
		framesactive++;
		if (framesactive > 199){
			Reset();
			bucket.push(myind);
			return false;
		}
		else if (framesactive == 175){
			beginani(3);
		}
	}
	else if (isactive() && (effect == 0)){
		framesactive++;
		if (framesactive > 100){
			Reset();
			bucket.push(myind);
			return false;
		}
	}
	else if (isactive() && effect == 4){
		if (framesactive == 3){
			Mix_PlayChannel(-1, csound, 0);
		}
		if (framesactive < 25){
			framesactive++;
		}
		else if (ani == 7){
			framesactive++;
			if (framesactive >= 80){
				Reset();
				bucket.push(myind);
				return false;
			}
		}
		return true;
	}
	else if (effect == 5 && isactive()){
		framesactive++;
		if (framesactive >= 150){
			Reset();
			bucket.push(myind);
			return false;
		}
		else if (framesactive == 125){
			beginani(3);
		}
	}
	else if (effect == 6 && isactive()){
		framesactive++;
		if (framesactive < 41){
			float t = mapValue(framesactive, 0, 40, 0, 1);
			float nrad = easeOut(0, 0.07, t);
			setRad(nrad);
		}
		crystal.setRad(getRad());
		crystal.Update();
		crystal.setPos(getx(), gety());
		if (framesactive >= 150){
			Reset();
			bucket.push(myind);
			return false;
		}
		else if (framesactive >= 125){
			float t = mapValue(framesactive, 125, 150, 0, 1);
			float nrad = easeOut(0.07, 0, t);
			setRad(nrad);
		}
	}
	else if (effect == -2 && isactive()){
		if (aniframes < 21){
			float t = mapValue(aniframes, 0, 20, 0, 1);
			float nrad = easeOut(0, 0.04, t);
			setRad(nrad);
		}
		crystal.setRad(getRad());
		crystal.Update();
		crystal.setPos(getx(), gety());
	}
	else if (effect == 7 && isactive()){
		framesactive++;
		if (framesactive == 3){
			Mix_PlayChannel(-1, csound, 0);
		}
		if (framesactive > 300){
			Reset();
			bucket.push(myind);
			return false;
		}
	}
	return isactive();
}
void cField::beginani(int i){
	ani = i;
	activate();
	aniframes = 0;
}
void cField::Render(){
	if (isactive()  && effect != 6 && effect != -2){
		Draw();
	}
	else if (isactive() && (effect == 6 || effect == -2) && aniframes > 2){
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		crystal.RenderSphere();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
		glDisable(GL_DEPTH_TEST);
	}
}
void cField::Reset(){
	deactivate();
	setRad(0);
	framesactive = 0;
	ani = 0;
	aniframes = 0;
}
void cField::Reset(GLuint i, int effnum){
	tex = i;
	deactivate();
	setRad(0);
	framesactive = 0;
	ani = 0;
	aniframes = 0;
	effect = effnum;
	crystal.tex = i;

}
void cField::slowkill(){
	if (effect == 2 && framesactive < 75){
		framesactive = 74;
	}
	else if (effect == 3 && framesactive< 175){
		framesactive = 174;
	}
	else if (effect == 4){
		beginani(7);
	}
	else if (effect == 5 && framesactive < 125){
		framesactive = 124;
	}
	else if (effect == 6 && framesactive < 149){
		framesactive = 125;
	}
	else if (effect == 7 && framesactive < 300){
		Reset();
	}
}
int cField::getEff(){
	return effect;
}


chargeBar::chargeBar(GLuint bg, GLuint frame, GLuint col, GLuint u, float x, float y)
:Entity(0, x, y), back(bg, x, y, 0.035, 0.6), f(frame, x, y, 0.035, 0.6), c(col, x, y, 0.035, 0), pfil(u, x, y, 0.035, 0)
{

}
void chargeBar::Render(){
	back.Draw();
	c.Draw();
	pfil.Draw();
	f.Draw();
}
void chargeBar::Update(float i, float j){
	float rat = mapValue(i, 0, 3, 0, 0.6);
	c.resize(0.035, rat);
	c.setPos(getx() - 0.6 + rat, gety());
	float rat2 = mapValue(j, 0, 3, 0, 0.6);
	pfil.resize(0.035, rat2);
	pfil.setPos(getx() - 0.6 + rat2, gety());
}

chargeBar2::chargeBar2(GLuint bg, GLuint frame, GLuint col, GLuint u, float x, float y)
	:Entity(0, x, y), back(bg, x, y, 0.035, 1), f(frame, x, y, 0.035, 1), c(col, x, y, 0.02, 0), pfil(u, x, y, 0.02, 0)
{
	ani = 0;
}
void chargeBar2::beginani(int i){
	ani = i;
	aniframes = 0;
}
void chargeBar2::Render(){
	back.Draw();
	c.Draw();
	pfil.Draw();
	f.Draw();
}
void chargeBar2::Update(float i, float j){
	float green = i;
	float orange = j;
	if (ani == 1 && aniframes < 100){
		aniframes++;
		orange = 0;
		green = mapValue(aniframes, 0, 100, 0, 1);
		green = easeInOut(0, 1, green);
	}
	else if (ani == 2 && aniframes < 150){
		aniframes++;
		green = mapValue(aniframes, 0, 75, 0, 1);
		green = easeInOut(0, 1, green);
		orange = mapValue(aniframes, 75, 150, 0, 1);
		green = easeInOut(0, 1, orange);
	}
	float rat = mapValue(green, 0, 1, 0, 1);
	c.resize(0.02, rat);
	c.setPos(getx() - 1 + rat, gety());
	float rat2 = mapValue(orange, 0, 1, 0, 1);
	pfil.resize(0.02, rat2);
	pfil.setPos(getx() - 1 + rat2, gety());
}


Player::Player()
:Circle(0, 0, 0, 0), pfield(0, 0, 0, 0, 0), hitbox(0, 0, 0, 0), spcirc(0, 0, 0, 0, 0)
{

}
Player::Player(int tex, float xp, float yp, int pnum1, int chara, vector<GLuint>& fte)
: Circle(tex, xp, yp, 0.08), pfield(0, 0, 0, 0, 0), hitbox(0, 0, 0, 0), spcirc(fte[0], 0, 0, 0, 0)
{
	lastfire = 0;
	vel = Vector(0, 0, 0);
	score = 0;
	totpts = 0;
	lives = 5;
	wins = 0;
	spoints = 0;
	cpoints = 0;
	pnum = pnum1;
	character = chara;
	focus = false;
	invframes = 0;
	speff = 0;
	angle = 0;
	hitbox = Circle(0, getx(), gety(), 0.01);
	if (chara == 1){
		ftex = fte[1];
		pfield = cField(ftex, xp, yp, 0, 1);
	}
	else if (chara == 2){
		ftex = fte[2];
	}
	else if (chara == 3){
		ftex = fte[3];
	}
	else if (chara == 4){
		ftex = fte[4];
		pfield = cField(ftex, xp, yp, 0, -1);
	}
	else if (chara == 5){
		ftex = fte[5];
		pfield = cField(ftex, xp, yp + 0.2, 0, -2);
	}
	else if (chara == 6){
		ftex = fte[6];
		pfield = cField(ftex, xp, yp, 0, 8);
	}
	if (chara == 1){
		exthresh = 200;
	}
	else if (chara == 2){
		exthresh = 400;
	}
	else if (chara == 3){
		exthresh = 300;
	}
	else if (chara == 4){
		exthresh = 250;
	}
	else if (chara == 5){
		exthresh = 200;
	}
	else if (chara == 6){
		exthresh = 400;
	}
	expts = 0;
	tbonus = 1;
	lastscoreframes = INT_MAX;
	dsound = Mix_LoadWAV("se/death.wav");
	ssound = Mix_LoadWAV("se/shot.wav");
	ecsound = Mix_LoadWAV("se/excharge.wav");
	spsound = Mix_LoadWAV("se/supercharge.wav");
	susound = Mix_LoadWAV("se/super.wav");
	esound = Mix_LoadWAV("se/endofapocalypse.wav");
}
void Player::Shoot(std::vector<GLuint> BTex, vector<cBullet>& bullets, queue<int>& bucket){
	GLuint ctex;
	float thresh = 3;
	ctex = BTex[0];
	int movt = 0;
	if (character == 1){
		ctex = BTex[1];

	}
	else if (character == 2){
		ctex = BTex[2];
	}
	else if (character == 3){
		ctex = BTex[5];
	}
	else if (character == 4){
		ctex = BTex[6];
	}
	else if (character == 5){
		ctex = BTex[7];
	}
	else if (character == 6){
		ctex = BTex[8];
	}
	if (character == 2 && focus){
		thresh = 15;
		movt = 1;
	}
	else if (character == 3 && focus){
		thresh = 6;
		movt = 5;
	}

	if (lastfire > thresh){
		lastfire = 0;
		if (character == 3 && focus){
			for (int i = 0; i < 8; i++){
				float ang = toRads(45 * i);
				if (bucket.size() != 0){
					int index = bucket.front();
					bucket.pop();
					bullets[index].setPos(getx(), gety());
					bullets[index].change(ctex, 1, movt);
					bullets[index].setDir(Vector(cos(ang) * FIXED_TIMESTEP * bulvel * 3.5, sin(ang) * FIXED_TIMESTEP * bulvel * 3.5, 0));
					bullets[index].activate();
				}
				else if(bullets.size() < 50){
					cBullet c(ctex, getx(), gety(), 1, movt, Vector(cos(ang), sin(ang), 0));
					c.activate();
					bullets.push_back(c);
				}
			}
		}
		else{
			if (bucket.size() != 0){
				int index = bucket.front();
				bucket.pop();
				bullets[index].setPos(getx(), gety());
				bullets[index].change(ctex, 1, movt);
				bullets[index].activate();
			}
			else{
				cBullet c(ctex, getx(), gety(), 1, movt);
				c.activate();
				bullets.push_back(c);
			}
		}
		Mix_PlayChannel(-1, ssound, 0);
	}

	
}
void Player::Update(PTransit& ptrans, bool foc, bool charging){
	lastfire++;
	if (foc && !focus && character == 1){
		pfield.beginani(1);
	}
	else if (foc && !focus && character == 4){
		pfield.setPos(getx(), gety());
		pfield.beginani(8);
	}
	else if (!foc && focus && character == 4){
		setPos(pfield.getx(), pfield.gety());
	}
	else if (foc && !focus && character == 5){
		pfield.setPos(getx(), gety() + 0.4);
		pfield.beginani(9);
	}
	else if (foc && !focus && character == 6){
		pfield.setPos(getx(), gety());
		pfield.beginani(1);
	}

	if (character == 1 || character == 2 || character == 3 || character == 6){
		pfield.setPos(getx(), gety());
	}
	else if (character == 5){
		angle = (angle + 1) % 360;
		pfield.setPos(getx() + 0.4 * cos(toRads(angle)), gety() + 0.4 * sin(toRads(angle)));
	}
	focus = foc;
	if (focus && character != 4){
		pfield.Update();
		move(vel.x/3, vel.y/3);
	}
	else if (focus && character == 4){
		pfield.Update();
		pfield.move(vel.x /2, vel.y / 2);
	}
	else{
		pfield.Reset();
		move(vel.x, vel.y);
	}

	
	if (charging && cpoints == 0 && spoints / 5000 > 0){
		Mix_PlayChannel(-1, ecsound, 0);
	}
	if (charging){
		cpoints += 166;
		if (floor(cpoints / 5000) != floor((cpoints-166) / 5000) && cpoints %5000 != 0){
			Mix_PlayChannel(-1, spsound, 0);
		}
	}
	if (cpoints >= spoints / 5000 * 5000){
		cpoints = spoints / 5000 * 5000;
	}
	if (!charging){
		if (cpoints / 5000 > 0){
			speff = cpoints / 5000;
			spoints -= 5000 * (cpoints / 5000);
			invframes = speff * 50;
			spcirc.setPos(getx(), gety());
			spcirc.beginani(3 + speff);
			Mix_PlayChannel(-1, susound, 0);
		}
		cpoints = 0;
	}
	spcirc.Update();
	hitbox.setPos(getx(), gety());
	vel.x = 0;
	vel.y = 0;

	if (pnum == 1 && getx() > -0.12){
		setPos(-0.12, gety());
	}
	else if (pnum == 1 && getx() < -1.21){
		setPos(-1.21, gety());
	}
	else if (pnum == 2 && getx() < 0.12){
		setPos(0.12, gety());
	}
	else if (pnum == 2 && getx() > 1.21){
		setPos(1.21, gety());
	}
	if (gety() > 0.88){
		setPos(getx(), 0.88);
	}
	else if (gety() < -0.88){
		setPos(getx(), -0.88);
	}
	if (invframes > 0){
		invframes--;
	}
	lastscoreframes++;
	if (lastscoreframes >= 50){
		expts = 0;
		tbonus = 0.95;
	}
	incScore(ptrans);
}
void Player::Render(){
	if (speff != 0){
		spcirc.Render();
	}
	if (focus && character == 1){
		pfield.Render();
	}
	if (focus && character == 4){
		pfield.Render();
	}
	if (focus && character == 6){
		pfield.Render();
	}
	if (focus && character == 5){
		pfield.Render();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
		glDisable(GL_DEPTH_TEST);
	}
	if ((invframes / 5) % 2 == 0){
		Draw();
	}
	if (focus){
		hitbox.Draw();
	}
}
bool Player::hit(vector<cBullet>& cbullets, vector<sBullet>& sbullets, queue<int>& cBuck, queue<int>& sBuck, PTransit& ptrans, vector<cField>& fields, queue<int>& fbuck){
	if (invframes == 0){
		for (int i = 0; i < cbullets.size(); i++){
			if (hitbox.overlaps(cbullets[i]) && cbullets[i].isactive()){
				cbullets[i].deactivate();
				cBuck.push(i);
				lastscoreframes = INT_MAX;
				tbonus = 1;
				Mix_PlayChannel(-1, dsound, 0);
				return true;
			}
		}
		for (int i = 0; i < sbullets.size(); i++){
			if (hitbox.overlaps(sbullets[i]) && sbullets[i].isactive()){
				sbullets[i].deactivate();
				sBuck.push(i);
				lastscoreframes = INT_MAX;
				tbonus = 1;
				Mix_PlayChannel(-1, dsound, 0);
				return true;
			}
		}
	}
	else if (invframes == 3 && speff == 0){
		spoints += 300 * 20 - lives;
		Circle killcircle(0, getx(), gety(), 0.6);
		for (int i = 0; i < cbullets.size(); i++){
			if (killcircle.overlaps(cbullets[i]) && cbullets[i].isactive()){
				cbullets[i].deactivate();
				cbullets[i].explode();
				cBuck.push(i);
				ptrans.add(Vector(cbullets[i].getx(), cbullets[i].gety(), 0));
				spoints += 20 * (20 - lives);
			}
		}
		for (int i = 0; i < sbullets.size(); i++){
			if (killcircle.overlaps(sbullets[i]) && sbullets[i].isactive()){
				sbullets[i].deactivate();
				sbullets[i].explode();
				sBuck.push(i);
				ptrans.add(Vector(sbullets[i].getx(), sbullets[i].gety(), 0));
				spoints += 20 * (20 - lives);
			}
		}
		for (int i = 0; i < fields.size(); i++){
			if (fields[i].isactive() && killcircle.overlaps(fields[i])){
				fields[i].slowkill();
				spoints += 50 * (20 - lives);
			}
		}
		if (spoints > 15000){
			spoints = 15000;
		}
	}
	if (invframes == 1 && speff != 0){
		speff = 0;
		tbonus = 1;
		spcirc.Reset();
	}
	else if (speff != 0){
		for (int i = 0; i < cbullets.size(); i++){
			if (spcirc.overlaps(cbullets[i]) && cbullets[i].isactive() && cbullets[i].getTier() <= speff){
				cbullets[i].deactivate();
				cbullets[i].explode();
				cBuck.push(i);
				ptrans.add(Vector(cbullets[i].getx(), cbullets[i].gety(), 0));
				addSpScore(10 * cbullets[i].getTier());
			}
		}
		for (int i = 0; i < sbullets.size(); i++){
			if (spcirc.overlaps(sbullets[i]) && sbullets[i].isactive() && sbullets[i].getTier() <= speff){
				sbullets[i].deactivate();
				sbullets[i].explode();
				sBuck.push(i);
				ptrans.add(Vector(sbullets[i].getx(), sbullets[i].gety(), 0));
				addSpScore(10 * sbullets[i].getTier());
			}
		}
		if (speff == 3){
			for (int i = 0; i < fields.size(); i++){
				if (fields[i].isactive() && spcirc.overlaps(fields[i])){
					fields[i].slowkill();
				}
			}
		}
	}
	if (focus && character == 5){
		for (int i = 0; i < cbullets.size(); i++){
			if (pfield.overlaps(cbullets[i]) && cbullets[i].isactive() && cbullets[i].getTier() <= 1){
				cbullets[i].deactivate();
				cbullets[i].explode();
				cBuck.push(i);
				ptrans.add(Vector(cbullets[i].getx(), cbullets[i].gety(), 0));
				addSpScore(10 * cbullets[i].getTier());
			}
		}
		for (int i = 0; i < sbullets.size(); i++){
			if (pfield.overlaps(sbullets[i]) && sbullets[i].isactive() && sbullets[i].getTier() <= 1){
				sbullets[i].deactivate();
				sbullets[i].explode();
				sBuck.push(i);
				ptrans.add(Vector(sbullets[i].getx(), sbullets[i].gety(), 0));
				addSpScore(10 * sbullets[i].getTier());
			}
		}
	}
	if (focus){
		pfield.applyEffect(cbullets, sbullets, cBuck, sBuck);
	}
	return false;
}
void Player::hinput(bool right){
	if (right){
		vel.x = FIXED_TIMESTEP * bulvel;
	}
	else{
		vel.x = -FIXED_TIMESTEP * bulvel;
	}
}
void Player::vinput(bool up){
	if (up){
		vel.y = FIXED_TIMESTEP * bulvel;
	}
	else{
		vel.y = -FIXED_TIMESTEP * bulvel;
	}
}
int Player::getScore(){
	return score;
}
int Player::addScore(int i){
	lastscoreframes = 0;
	tbonus += 0.1;
	totpts += i * tbonus;
	if (speff == 0){
		spoints += i * tbonus;
		if (spoints > 15000){
			spoints = 15000;
		}
	}
	return score;
}
int Player::addSpScore(int i){
	lastscoreframes = 0;
	totpts += i;
	return score;
}
int Player::addSpScore2(int i){
	lastscoreframes = 0;
	totpts += i;
	spoints += i;
	return score;
}
void Player::startinv(){
	invframes = 50;
}
int Player::damage(int i){
	lives -= i;
	return lives;
}
void Player::incScore(PTransit& ptrans){
	score += ceil((totpts - score) / 30);
	expts += ceil((totpts - score) / 30);
	while (expts > exthresh){
		expts -= exthresh;
		ptrans.add(Vector(getx(), gety(), 0), character + 2);
	}
}
GLuint Player::getftex(){
	return ftex;
}
float Player::getfill(){
	return mapValue(spoints, 0, 15000, 0, 3);
}
float Player::getcharge(){
	return mapValue(cpoints, 0, 15000, 0, 3);
}






stringGraphic::stringGraphic(int fontTexture, std::string str, float csize, float cspacing, float re, float gr, float bl, float al, float x, float y)
:Entity(fontTexture, x, y)
{
	text = str;
	size = csize;
	spacing = cspacing;
	r = re;
	g = gr;
	b = bl;
	a = al;
}
void stringGraphic::Draw(){
	if (isactive()){
		glBindTexture(GL_TEXTURE_2D, tex);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPushMatrix();
		Matrix transform;
		transform = buildTMat();
		glMultMatrixf(transform.ml);


		float texture_size = 1.0 / 16.0f;

		vector<float> vertexData;
		vector<float> texCoordData;
		vector<float> colorData;

		for (int i = 0; i < text.size(); i++) {
			float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
			float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
			colorData.insert(colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });

			vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) +
				(-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f
				* size });

			texCoordData.insert(texCoordData.end(), { texture_x, texture_y, texture_x, texture_y + texture_size, texture_x +
				texture_size, texture_y + texture_size, texture_x + texture_size, texture_y });
		}
		glColorPointer(4, GL_FLOAT, 0, colorData.data());
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
		glEnableClientState(GL_VERTEX_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDrawArrays(GL_QUADS, 0, text.size() * 4);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glPopMatrix();
	}
}
void stringGraphic::Update(){
	if (ani == 1 && aniframes < 150){
		aniframes++;
		float mc = mapValue(aniframes, 0, 150, 0, 1);
		float nx = easeOutElastic(1.5, -0.8, mc);
		setPos(nx, gety());
	}
	else if (ani == 2 && aniframes < 3){
		aniframes++;
		float mc = mapValue(aniframes, 0, 3, 0, 1);
		float nx = easeIn(1, 0, mc);
		float ns = easeIn(0.1, 0.12, mc);
		r = nx;
		size = ns;
	}
	else if (ani == 3 && aniframes < 3){
		aniframes++;
		float mc = mapValue(aniframes, 0, 3, 0, 1);
		float nx = easeIn(0, 1, mc);
		float ns = easeIn(0.12, 0.1, mc);
		r = nx;
		size = ns;
	}
	else if (ani == 4 && aniframes < 100){
		aniframes++;
		float mc = mapValue(aniframes, 0, 100, 0, 1);
		float nx = easeInOut(1.5, -0.1, mc);
		setPos(nx, gety());
	}
	else if (ani == 5 && aniframes < 100){
		aniframes++;
		if (aniframes <= 50){
			float mc = mapValue(aniframes, 0, 50, 0, 1);
			float nx = easeOutElastic(-2, -0.8, mc);
			setPos(nx, gety());
		}
		else if (aniframes > 50){
			float mc = mapValue(aniframes, 50, 100, 0, 1);
			float nx = easeIn(0, 1.2, mc);
			setPos(getx(), nx);
		}
	}
	else if (ani == 6 && aniframes < 100){
		aniframes++;
		if (aniframes <= 50){
			float mc = mapValue(aniframes, 0, 50, 0, 1);
			float nx = easeOutElastic(1.5, 0.5, mc);
			setPos(nx, gety());
		}
		else if (aniframes > 50){
			float mc = mapValue(aniframes, 50, 100, 0, 1);
			float nx = easeIn(0, 1.2, mc);
			setPos(getx(), nx);
		}
	}
	else if (ani == 7 && aniframes < 300){
		aniframes++;
		float mc = mapValue(aniframes, 0, 300, 0, 1);
		float nx = easeOutElastic(-2, 0.5, mc);
		setPos(nx, 0.3);
	}
	else if (ani == 8 && aniframes < 300){
		aniframes++;
		float mc = mapValue(aniframes, 0, 300, 0, 1);
		float nx = easeOutElastic(1.5, -0.8, mc);
		setPos(nx, 0.3);
	}
	else{
		ani = 0;
	}
}

void stringGraphic::beginani(int animation){
	ani = animation;
	aniframes = 0;
}

int stringGraphic::getani(){
	return ani;
}

void stringGraphic::setText(string s){
	text = s;
}

bCont::bCont(){

}

void bCont::Update(Player& p1, Player& p2){
	for (int i = 0; i < p1buls.size(); i++){
		p1buls[i].Update();
		if (p1buls[i].isactive() && (p1buls[i].gety() <-1 || p1buls[i].gety() > 1 || p1buls[i].getx() < -1.33 || p1buls[i].getx() > 0)){
			p1buls[i].deactivate();
			p1buck.push(i);
		}
	}
	for (int i = 0; i < p2buls.size(); i++){
		p2buls[i].Update();
		if (p2buls[i].isactive() && (p2buls[i].gety() > 1 || p2buls[i].gety() < -1 || p2buls[i].getx() > 1.33 || p2buls[i].getx() < 0)){
			p2buls[i].deactivate();
			p2buck.push(i);
		}
	}
	for (int i = 0; i < ec1buls.size(); i++){
		int j = ec1buls[i].getTier();
		if (j == 3){
			float b = 0;
		}
		ec1buls[i].Update(p1.getx(), p1.gety());
		if (ec1buls[i].isactive() && (ec1buls[i].gety() <-1 || ec1buls[i].gety() > 1 || ec1buls[i].getx() < -1.33 || ec1buls[i].getx() > 0)){
			ec1buls[i].deactivate();
			ec1buck.push(i);
		}
	}
	for (int i = 0; i < ec2buls.size(); i++){
		ec2buls[i].Update(p2.getx(), p2.gety());
		if (ec2buls[i].isactive() && (ec2buls[i].gety() > 1 || ec2buls[i].gety() < -1 || ec2buls[i].getx() > 1.33 || ec2buls[i].getx() < 0)){
			ec2buls[i].deactivate();
			ec2buck.push(i);
		}
	}
	for (int i = 0; i < es1buls.size(); i++){
		es1buls[i].Update();
		if (es1buls[i].isactive() && (es1buls[i].gety() <-1 || es1buls[i].gety() > 1 || es1buls[i].getx() < -1.33 || es1buls[i].getx() > 0)){
			es1buls[i].deactivate();
			es1buck.push(i);
		}
	}
	for (int i = 0; i < es2buls.size(); i++){
		es2buls[i].Update();
		if (es2buls[i].isactive() && (es2buls[i].gety() > 1 || es2buls[i].gety() < -1 || es2buls[i].getx() > 1.33 || es2buls[i].getx() < 0)){
			es2buls[i].deactivate();
			es2buck.push(i);
		}
	}
}

eCont::eCont(){

}

tBullet::tBullet(float ox, float oy, float dx, float dy, int ti)
:origin(ox, oy, 0), destination(dx, dy, 0), tier(ti), ftime(0), active(true)
{
	position.x = ox;
	position.y = oy;
}

PTransit::PTransit(){
	ssound = Mix_LoadWAV("se/exspawn.wav");
	msound = Mix_LoadWAV("se/apocalypseex.wav");
}
void PTransit::Update(std::vector<GLuint> BTex, std::vector<cBullet>& bullets, std::queue<int>& bucket, vector<GLuint>& fte, vector<cField>& fields, queue<int>& fbucket){
	for (int i = 0; i < parts.size(); i++){
		if (parts[i].ftime < 50){
			parts[i].ftime++;
			float mc = mapValue(parts[i].ftime, 0, 50, 0, 1);
			float nx = easeInOut(parts[i].origin.x, parts[i].destination.x, mc);
			float ny = easeInOut(parts[i].origin.y, parts[i].destination.y, mc);
			parts[i].position.x = nx;
			parts[i].position.y = ny;
			if (parts[i].ftime == 10 && parts[i].tier >= 3){
				Mix_PlayChannel(-1, msound, 0);
			}
		}
		else if(parts[i].active == true && parts[i].tier < 3){
			if ( bucket.size() != 0 && bullets.size() - bucket.size() < 75){
				int index = bucket.front();
				bucket.pop();
				GLuint bt;
				if (parts[i].position.x > 0){
					bt = BTex[3];
				}
				else{
					bt = BTex[4];
				}
				bullets[index].setPos(parts[i].position.x, parts[i].position.y);
				bullets[index].change(bt, parts[i].tier, 4);
				bullets[index].activate();
			}
			else if(bullets.size() < 75){
				GLuint bt;
				if (parts[i].position.x > 0){
					bt = BTex[3];
				}
				else{
					bt = BTex[4];
				}
				cBullet c(bt, parts[i].position.x, parts[i].position.y, parts[i].tier, 4);
				c.activate();
				bullets.push_back(c);
			}
			pbucket.push(i);
			parts[i].active = false;
		}
		else if (parts[i].active == true && parts[i].tier >= 3){
			GLuint ftex = fte[parts[i].tier - 2];
			if (fbucket.size() > 0){
				int index = fbucket.front();
				if (fields.size() > 20 || fields[index].isactive()){
					bool a = true;
				}
				fbucket.pop();
				fields[index].setPos(parts[i].position.x, parts[i].position.y);
				fields[index].Reset(ftex, parts[i].tier - 1);
				if (parts[i].tier == 3){
					fields[index].beginani(2);
				}
				else if (parts[i].tier == 4){
					fields[index].beginani(2);
				}
				else if (parts[i].tier == 5){
					fields[index].beginani(2);
				}
				else if (parts[i].tier == 6){
					fields[index].beginani(2);
				}
				else if (parts[i].tier == 7){
					fields[index].beginani(10);
					if (ftex != 58){
						bool a = true;
					}
				}
				else if (parts[i].tier == 8){
					fields[index].beginani(2);
				}
			}
			else{
				cField c(ftex, parts[i].position.x, parts[i].position.y, 0, parts[i].tier - 1);
				c.Reset();
				if (parts[i].tier == 3 || parts[i].tier == 4 || parts[i].tier == 5 || parts[i].tier == 6 || parts[i].tier == 8){
					c.beginani(2);
				}
				else if (parts[i].tier == 7){
					c.beginani(10);
				}
				fields.push_back(c);
			}
			pbucket.push(i);
			parts[i].active = false;
			Mix_PlayChannel(-1, ssound, 0);
		}
	}
}
void PTransit::Render(){
	std::vector<float> particleVertices;
	std::vector<float> particleColors;
	for (int i = 0; i < parts.size(); i++) {
		if (parts[i].ftime < 50){
			particleVertices.push_back(parts[i].position.x);
			particleVertices.push_back(parts[i].position.y);
			if (parts[i].tier == 8){
				particleColors.push_back(0);
				particleColors.push_back(1);
				particleColors.push_back(1);
				particleColors.push_back(1);
			}
			if (parts[i].tier == 7){
				particleColors.push_back(0.5);
				particleColors.push_back(0.5);
				particleColors.push_back(0.5);
				particleColors.push_back(1);
			}
			if (parts[i].tier == 6){
				particleColors.push_back(0);
				particleColors.push_back(0.5);
				particleColors.push_back(1);
				particleColors.push_back(1);
			}
			if (parts[i].tier == 5){
				particleColors.push_back(0);
				particleColors.push_back(1);
				particleColors.push_back(0);
				particleColors.push_back(1);
			}
			if (parts[i].tier == 4){
				particleColors.push_back(1);
				particleColors.push_back(1);
				particleColors.push_back(0);
				particleColors.push_back(1);
			}
			else if (parts[i].tier == 3){
				particleColors.push_back(1);
				particleColors.push_back(1);
				particleColors.push_back(1);
				particleColors.push_back(1);
			}
			else if (parts[i].tier == 1){
				particleColors.push_back(1);
				particleColors.push_back(0);
				particleColors.push_back(0);
				particleColors.push_back(1);
			}
			else if (parts[i].tier == 2){
				particleColors.push_back(0);
				particleColors.push_back(0);
				particleColors.push_back(1);
				particleColors.push_back(1);
			}
			glPointSize(3.0);
		}
	}
	glColorPointer(4, GL_FLOAT, 0, particleColors.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, particleVertices.data());
	glEnableClientState(GL_VERTEX_ARRAY);

	glDrawArrays(GL_POINTS, 0, particleVertices.size() / 2);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
void PTransit::add(Vector pos){
	Vector p(pos.x, pos.y, 0);
	if (pos.x < 0){
		if (pbucket.size() != 0){
			int index = pbucket.front();
			pbucket.pop();
			float nex = mapValue(rand(), 0, RAND_MAX, 0.2, 1.1);
			float ney = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
			float net = mapValue(rand(), 0, RAND_MAX, 1, 5);
			int t = 1;
			if (net > 4){
				t = 2;
			}
			parts[index] = tBullet(p.x, p.y, nex, ney, t);
			parts[index].active = true;
		}
		else{
			float nex = mapValue(rand(), 0, RAND_MAX, 0.2, 1.1);
			float ney = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
			float net = mapValue(rand(), 0, RAND_MAX, 1, 5);
			int t = 1;
			if (net > 4){
				t = 2;
			}
			parts.push_back(tBullet(p.x, p.y, nex, ney, t));
		}
	}
	else{
		if (pbucket.size() != 0){
			int index = pbucket.front();
			pbucket.pop();
			float nex = mapValue(rand(), 0, RAND_MAX, -1.1, -0.2);
			float ney = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
			float net = mapValue(rand(), 0, RAND_MAX, 1, 5);
			int t = 1;
			if (net > 4){
				t = 2;
			}
			parts[index] = tBullet(p.x, p.y, nex, ney, t);
			parts[index].active = true;
		}
		else{
			float nex = mapValue(rand(), 0, RAND_MAX, -1.1, -0.2);
			float ney = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
			float net = mapValue(rand(), 0, RAND_MAX, 1, 5);
			int t = 1;
			if (net > 4){
				t = 2;
			}
			parts.push_back(tBullet(p.x, p.y, nex, ney, t));

		}
	}
}
void PTransit::add(Vector pos, int tier){
	Vector p(pos.x, pos.y, 0);
	float miny = 0.5;
	if (tier > 2){
		miny = -0.8;
	}
	if (pos.x < 0){
		if (pbucket.size() != 0){
			int index = pbucket.front();
			pbucket.pop();
			float nex = mapValue(rand(), 0, RAND_MAX, 0.2, 1.1);
			float ney = mapValue(rand(), 0, RAND_MAX, miny, 0.9);
			parts[index] = tBullet(p.x, p.y, nex, ney, tier);
			parts[index].active = true;
		}
		else{
			float nex = mapValue(rand(), 0, RAND_MAX, 0.2, 1.1);
			float ney = mapValue(rand(), 0, RAND_MAX, miny, 0.9);
			parts.push_back(tBullet(p.x, p.y, nex, ney, tier));
		}
	}
	else{
		if (pbucket.size() != 0){
			int index = pbucket.front();
			pbucket.pop();
			float nex = mapValue(rand(), 0, RAND_MAX, -1.1, -0.2);
			float ney = mapValue(rand(), 0, RAND_MAX, miny, 0.9);
			parts[index] = tBullet(p.x, p.y, nex, ney, tier);
			parts[index].active = true;
		}
		else{
			float nex = mapValue(rand(), 0, RAND_MAX, -1.1, -0.2);
			float ney = mapValue(rand(), 0, RAND_MAX, miny, 0.9);
			parts.push_back(tBullet(p.x, p.y, nex, ney, tier));

		}
	}
}

