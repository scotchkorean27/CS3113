
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

float toRads(float deg){
	return deg * 3.14 / 180;
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
ParticleEmitter::ParticleEmitter(float x, float y, bool imp){
	maxLifetime = 1000;
	position.x = x;
	position.y = y;
	implode = imp;
	int max = 20;
	if (implode){
		max = 60;
	}
	for (int i = 0; i < max; i++){
		float angle = rand() % 360;
		float ax = cos(toRads(angle)) / 80;
		float ay = sin(toRads(angle)) / 80;
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
		if (particles[i].lifetime < 0 && !implode){
			particles[i].lifetime = rand() % 1000;
			particles[i].position.x = position.x;
			particles[i].position.y = position.y;
		}
		else if (particles[i].lifetime < 0 && implode){
			particles[i].lifetime = rand() % 100;
			float angle = rand() % 360;
			float ax = cos(toRads(angle)) / 80;
			float ay = sin(toRads(angle)) / 80;
			Vector pvel(ax, ay, 0);
			particles[i].velocity = pvel;
			particles[i].position.x = position.x - particles[i].velocity.x / elapsed;
			particles[i].position.y = position.y - particles[i].velocity.y / elapsed;
			particles[i].velocity.x = (position.x - particles[i].position.x) / particles[i].lifetime / elapsed;
			particles[i].velocity.y = (position.y-particles[i].position.y) / particles[i].lifetime / elapsed;
		}
		else if (implode){

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
		if (implode){
			particleColors.push_back(1);
			particleColors.push_back(0);
			particleColors.push_back(0);
			particleColors.push_back(1);
		}
		else{
			particleColors.push_back(0);
			particleColors.push_back(1);
			particleColors.push_back(1);
			particleColors.push_back(1);
		}
	}
	glColorPointer(4, GL_FLOAT, 0, particleColors.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, particleVertices.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glPointSize(5.0);
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

/*
void Entity::Draw(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
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

	//glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();
}
*/
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




Square::Square(int texture, float x, float y, float h, float w)
: Entity(texture,x, y)
{
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


Circle::Circle(int texture, float x, float y, float rad)
:Entity(texture, x, y)
{
	radius = rad;
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






sBullet::sBullet(int tex, float xp, float yp, int t, int behavior, float w, float h)
:Square(tex, xp, yp, w, h)
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
}

void sBullet::Update(){
	if (isactive()){
	move(dir.x, dir.y);
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
	

cBullet::cBullet(int tex,float xp, float yp, int t, int behavior, float rad)
:Circle(tex, xp, yp, rad)
{
	health = 1;
	tier = t;
	movtype = behavior;
	Vector d(0, 0, 0);
	if (movtype == 0){
		d.x = 0;
		d.y = FIXED_TIMESTEP * bulvel;
	}
	dir = d;
}
void cBullet::Orient(float angle){
	incRot(angle - getRot());
}
void cBullet::Update(){
	if (isactive()){
		move(dir.x, dir.y);
	}
}
void cBullet::Render(){
	if (isactive()){
		Circle::Draw();
	}
	if (health < 1){
		deactivate();
	}
}
int cBullet::damage(int i){
	health = -i;
	return health;
}


Enemy::Enemy(int tex, float xp, float yp, int t)
:Circle(tex, xp, yp, 0.3), pe(0, 0, true)
{
	tier = t;
	framesalive = 0;
}
void Enemy::Update(){
	framesalive += 1;
	move(-0.01, 0);
}
void Enemy::Render(){
	Circle::Draw();
}
void Enemy::shoot(vector<cBullet>& cbullets, vector<sBullet>& sbullets, queue<int>& cBuck, queue<int>& sBuck){
	//temporary
	cBullet c(1, getx(), gety(), 1, 0, 0.05);
	c.activate();
	cbullets.push_back(c);
}
bool Enemy::hit(vector<cBullet>& bullets, queue<int>& bucket){
	return false;
}
int Enemy::explode(vector<Enemy> ene, queue<int> bucket){
	return 0;
}
void Enemy::reinit(){

}
int Enemy::damage(int i){
	health -= i;
	return health;
}


Player::Player()
:Circle(0, 0, 0, 0)
{

}
Player::Player(int tex, float xp, float yp, int pnum1, int chara)
:Circle(tex, xp, yp, 0.04)
{
	lastfire = 0;
	vel = Vector(0, 0, 0);
	score = 0;
	lives = 3;
	wins = 0;
	pnum = pnum1;
	character = chara;
}
void Player::Shoot(int BTex, vector<cBullet>& bullets, queue<int>& bucket){
	float cfire = (float)SDL_GetTicks() / 1000.0f;
	if (cfire - lastfire > 0.1){
		if (bucket.size() != 0){
			int index = bucket.front();
			bucket.pop();
			bullets[index].setPos(getx(), gety());
			bullets[index].activate();
		}
		else{
			cBullet c(BTex, getx(), gety(), 1, 0, 0.02);
			c.activate();
			bullets.push_back(c);
		}
		lastfire = cfire;
	}
	
}
void Player::Update(){
	move(vel.x, vel.y);
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
}
void Player::Render(){
	Draw();
}
bool Player::hit(vector<cBullet>& cbullets, vector<sBullet>& sbullets, queue<int>& cBuck, queue<int>& sBuck){
	for (int i = 0; i < cbullets.size(); i++){
		if (overlaps(cbullets[i])){
			cbullets[i].deactivate();
			cBuck.push(i);
			return true;
		}
	}
	for (int i = 0; i < sbullets.size(); i++){
		if (overlaps(sbullets[i])){
			sbullets[i].deactivate();
			sBuck.push(i);
			return true;
		}
	}
	
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


sField::sField(int tex, float xp, float yp, int behavior, float h, float w)
:Square(tex, xp, yp, h, w)
{

}
void sField::applyEffect(vector<cBullet>& bull, vector<sBullet>& sbul, vector<Enemy>& ene, Player& p){

}

cField::cField(int tex, float xp, float yp, int behavior, float rad)
:Circle(tex, xp, yp, rad)
{

}
void cField::applyEffect(vector<cBullet>& bull, vector<sBullet>& sbul, vector<Enemy>& ene, Player& p){

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
		float mc = mapValue(aniframes, 0, 100, 0, 1);
		float nx = easeIn(0, 1.2, mc);
		setPos(getx(), nx);
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

eCont::eCont(){

}