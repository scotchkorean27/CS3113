
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

Entity::Entity(int texture, float xp, float yp, float h, float w)
:tex(texture),
x(xp),
y(yp),
height(h),
width(w),
rotation(0)
{
}
Entity::~Entity(){

}
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
void Entity::setPos(float xp, float yp){
	x = xp;
	y = yp;
}
void Entity::move(float xp, float yp){
	x += xp;
	y += yp;
}
float Entity::getxpen(Entity& b){
	return fabs(x - b.x) - width - b.width;
}
float Entity::getypen(Entity& b){
	return fabs(y - b.y) - height - b.height;
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
bool Entity::overlaps(Entity& b){
	Vector vn1(cos(toRads(rotation)), sin(toRads(rotation)), 0);
	Vector vn2(cos(toRads(rotation + 90)), sin(toRads(rotation + 90)), 0);
	vn1.normalize();
	vn2.normalize();

	Vector v(width, height, 0);
	float length = v.length();
	Vector v2(b.width, b.height, 0);
	float length2 = v2.length();

	float a1max;
	float a1min;
	Vector a = Vector(x + length * cos(toRads(v.angle() + rotation)), y + length * sin(toRads(v.angle() + rotation)), 0);
	a1max = vn1.dotprod(a.x, a.y);
	a1min = vn1.dotprod(a.x, a.y);
	a = Vector(x + length * cos(toRads(180 - (v.angle() + rotation))), y + length * sin(toRads(180 - (v.angle() + rotation))), 0);
	if (vn1.dotprod(a.x, a.y) > a1max){
		a1max = vn1.dotprod(a.x, a.y);
	}
	else if (vn1.dotprod(a.x, a.y) < a1min){
		a1min = vn1.dotprod(a.x, a.y);
	}
	a = Vector(x + length * cos(toRads(180 + v.angle() + rotation)), y + length * sin(toRads(180 + v.angle() + rotation)), 0);
	if (vn1.dotprod(a.x, a.y) > a1max){
		a1max = vn1.dotprod(a.x, a.y);
	}
	else if (vn1.dotprod(a.x, a.y) < a1min){
		a1min = vn1.dotprod(a.x, a.y);
	}
	a = Vector(x + length * cos(toRads(360 - (v.angle() + rotation))), y + length * sin(toRads(360 - (v.angle() + rotation))), 0);
	if (vn1.dotprod(a.x, a.y) > a1max){
		a1max = vn1.dotprod(a.x, a.y);
	}
	else if (vn1.dotprod(a.x, a.y) < a1min){
		a1min = vn1.dotprod(a.x, a.y);
	}

	float b1max;
	float b1min;
	a = Vector(b.x + length2 * cos(toRads(v2.angle() + b.rotation)), b.y + length2 * sin(toRads(v2.angle() + b.rotation)), 0);
	b1max = vn1.dotprod(a.x, a.y);
	b1min = vn1.dotprod(a.x, a.y);
	a = Vector(b.x + length2 * cos(toRads(180 - (v2.angle() + b.rotation))), b.y + length2 * sin(180 - (toRads(v2.angle() + b.rotation))), 0);
	if (vn1.dotprod(a.x, a.y) > b1max){
		b1max = vn1.dotprod(a.x, a.y);
	}
	else if (vn1.dotprod(a.x, a.y) < b1min){
		b1min = vn1.dotprod(a.x, a.y);
	}
	a = Vector(b.x + length2 * cos(toRads(180 + (v2.angle() + b.rotation))), b.y + length2 * sin(180 + (toRads(v2.angle() + b.rotation))), 0);
	if (vn1.dotprod(a.x, a.y) > b1max){
		b1max = vn1.dotprod(a.x, a.y);
	}
	else if (vn1.dotprod(a.x, a.y) < b1min){
		b1min = vn1.dotprod(a.x, a.y);
	}
	a = Vector(b.x + length2 * cos(toRads(360 - (v2.angle() + b.rotation))), b.y + length2 * sin(360 - (toRads(v2.angle() + b.rotation))), 0);
	if (vn1.dotprod(a.x, a.y) > b1max){
		b1max = vn1.dotprod(a.x, a.y);
	}
	else if (vn1.dotprod(a.x, a.y) < b1min){
		b1min = vn1.dotprod(a.x, a.y);
	}

	float ac1dp = vn1.dotprod(x, y);
	float bc1dp = vn1.dotprod(b.x, b.y);

	float p = abs(ac1dp - bc1dp) - (a1max - a1min + b1max - b1min) / 2;
	if (p >= 0){
		return false;
	}

	float a2max;
	float a2min;
	a = Vector(x + length * cos(toRads(v.angle() + rotation)), y + length * sin(toRads(v.angle() + rotation)), 0);
	a2max = vn2.dotprod(a.x, a.y);
	a2min = vn2.dotprod(a.x, a.y);
	a = Vector(x + length * cos(toRads(180 - (v.angle() + rotation))), y + length * sin(toRads(180 - (v.angle() + rotation))), 0);
	if (vn2.dotprod(a.x, a.y) > a2max){
		a2max = vn2.dotprod(a.x, a.y);
	}
	else if (vn2.dotprod(a.x, a.y) < a2min){
		a2min = vn2.dotprod(a.x, a.y);
	}
	a = Vector(x + length * cos(toRads(180 + v.angle() + rotation)), y + length * sin(toRads(180 + v.angle() + rotation)), 0);
	if (vn2.dotprod(a.x, a.y) > a2max){
		a2max = vn2.dotprod(a.x, a.y);
	}
	else if (vn2.dotprod(a.x, a.y) < a2min){
		a2min = vn2.dotprod(a.x, a.y);
	}
	a = Vector(x + length * cos(toRads(360 - (v.angle() + rotation))), y + length * sin(toRads(360 - (v.angle() + rotation))), 0);
	if (vn2.dotprod(a.x, a.y) > a2max){
		a2max = vn2.dotprod(a.x, a.y);
	}
	else if (vn2.dotprod(a.x, a.y) < a2min){
		a2min = vn2.dotprod(a.x, a.y);
	}

	float b2max;
	float b2min;
	a = Vector(b.x + length2 * cos(toRads(v2.angle() + b.rotation)), b.y + length2 * sin(toRads(v2.angle() + b.rotation)), 0);
	b2max = vn2.dotprod(a.x, a.y);
	b2min = vn2.dotprod(a.x, a.y);
	a = Vector(b.x + length2 * cos(toRads(180 - (v2.angle() + b.rotation))), b.y + length2 * sin(180 - (toRads(v2.angle() + b.rotation))), 0);
	if (vn2.dotprod(a.x, a.y) > b2max){
		b2max = vn2.dotprod(a.x, a.y);
	}
	else if (vn2.dotprod(a.x, a.y) < b2min){
		b2min = vn2.dotprod(a.x, a.y);
	}
	a = Vector(b.x + length2 * cos(toRads(180 + (v2.angle() + b.rotation))), b.y + length2 * sin(180 + (toRads(v2.angle() + b.rotation))), 0);
	if (vn2.dotprod(a.x, a.y) > b2max){
		b2max = vn2.dotprod(a.x, a.y);
	}
	else if (vn2.dotprod(a.x, a.y) <b2min){
		b2min = vn2.dotprod(a.x, a.y);
	}
	a = Vector(b.x + length2 * cos(toRads(360 - (v2.angle() + b.rotation))), b.y + length2 * sin(360 - (toRads(v2.angle() + b.rotation))), 0);
	if (vn2.dotprod(a.x, a.y) > b2max){
		b2max = vn2.dotprod(a.x, a.y);
	}
	else if (vn2.dotprod(a.x, a.y) < b2min){
		b2min = vn2.dotprod(a.x, a.y);
	}

	float ac2dp = vn2.dotprod(x, y);
	float bc2dp = vn2.dotprod(b.x, b.y);

	p = abs(ac2dp - bc2dp) - (a2max - a2min + b2max - b2min) / 2;
	if (p >= 0){
		return false;
	}

	return true;
}
void Entity::resize(float x, float y){
	width = x;
	height = y;
}

Bullet::Bullet(int tex, float xp, float yp, float h, float w, float gangle) : Entity(tex, xp, yp, h, w){
	Vector d(cos(toRads(gangle))/ 40, sin(toRads(gangle)) / 40, 0);
	dir = d;
	incRot(gangle);
}
Bullet::~Bullet(){

}
void Bullet::Update(){
	if (isactive()){
		move(dir.x, dir.y);
	}
}
void Bullet::Render(){
	if (isactive()){
		Draw();
	}
}
void Bullet::Orient(){
	Vector d(cos(toRads(getRot())) / 40, sin(toRads(getRot())) / 40, 0);
	dir = d;
}



Player::Player(int tex, float xp, float yp)
: Entity(tex, xp, yp, 0.04, 0.06),
lastfire(0)
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
}
Player::Player()
:Entity(0, 0, 0, 0, 0){

	deactivate();
}
void Player::Shoot(int BTex, vector<Bullet>& bullets){
	if (isactive()){
		float cfire = (float)SDL_GetTicks() / 1000.0f;
		if (cfire - lastfire > 0.2){
			Bullet nBull(BTex, getx(), gety(), 0.02, 0.04, getRot() + 90);
			lastfire = (float)SDL_GetTicks() / 1000.0f;
			nBull.activate();
			bullets.push_back(nBull);
		}
	}

}
void Player::bRepo(int ind, vector<Bullet>& bullets){
	if (isactive()){
		float cfire = (float)SDL_GetTicks() / 1000.0f;
		if (cfire - lastfire > 0.2){
			bullets[ind].setPos(getx(), gety());
			bullets[ind].activate();
			bullets[ind].incRot(-bullets[ind].getRot() + getRot() + 90);
			bullets[ind].Orient();
			lastfire = (float)SDL_GetTicks() / 1000.0f;
		}
	}
}
void Player::hinput(bool right){
		if (!right){
			incRot(10);
		}
		else{
			incRot(-10);
		}
}
void Player::Update(){
	if (isactive()){
		vel.x += acc.x;
		vel.y += acc.y;
		acc.x = 0;
		acc.y = 0;
		vel.x = applyfriction(vel.x, 0.7);
		vel.y = applyfriction(vel.y, 0.7);
		move(vel.x, vel.y);
		if (abs(getx()) > 1){
			setPos(-getx(), gety());
		}
		if (abs(gety()) > 1){
			setPos(getx(), -gety());
		}
	}
}
void Player::accel(){
		acc.x = cos(toRads(getRot() + 90)) / 800;
		acc.y = sin(toRads(getRot() + 90)) / 800;
}
void Player::raccel(){
	acc.x = -cos(toRads(getRot() + 90)) / 800;
	acc.y = -sin(toRads(getRot() + 90)) / 800;
}
void Player::Render(){
	if (isactive()){
		Draw();
	}
}
bool Player::hit(vector<Enemy>& bullets){
	if (isactive()){
		for (int i = 0; i < bullets.size(); ++i){
			if ((bullets[i]).isactive() && overlaps((bullets[i])) && bullets[i].overlaps(*this)){
				//Compensates for white space
					bullets[i].setPos(-1.5, -1.5);
					bullets[i].deactivate();
					return true;
			}
		}
	}
	return false;
}

Enemy::Enemy(int tex, float xp, float yp, int frac)
: Entity(tex, xp, yp, 0.15 / frac, 0.15 / frac), fraction(frac), pe(0, 0, false), pframes(0)
{
	float vx = rand() % 360 - 180;
	float vy = rand() % 360 - 180;
	Vector d(vx, vy, 0);
	d.normalize();
	d.x = d.x / 120 + (frac / 360);
	d.y = d.y / 120 + (frac / 360);
	dir = d;
}
void Enemy::Update(){
	if (isactive()){
		move(dir.x, dir.y); 
		if (abs(getx()) > 1){
			setPos(-getx(), gety());
		}
		if (abs(gety()) > 1){
			setPos(getx(), -gety());
		}
		incRot(2);
	}
	else if (pframes < 100){
		
	}
}
void Enemy::Render(){
	if (isactive()){
		Draw();
	}
	else if (pframes < 1000){
		pe.Update(FIXED_TIMESTEP);
		pe.Render();
		pframes++;
	}
}
bool Enemy::hit(vector<Bullet>& bullets){
	for (int i = 0; i < bullets.size(); ++i){
		if (isactive() && (bullets[i]).isactive() && overlaps((bullets[i])) && bullets[i].overlaps(*this)){
			//Compensates for white space
				(bullets[i]).deactivate();
				deactivate();
				return true;
		}
	}
	return false;
}
int Enemy::explode(std::vector<Enemy>& enemies){
	pe.setPos(getx(), gety());
	int score = fraction * 50;
	if (fraction < 5){
		Enemy e(tex, getx(), gety(), fraction + 1);
		e.activate();
		Enemy e2(tex, getx(), gety(), fraction + 1);
		e2.activate();
		enemies.push_back(e);
		enemies.push_back(e2);
		e2.reinit();
	}
	return score;
}
void Enemy::reinit(){
	float vx = rand() % 360;
	float vy = rand() % 360;
	Vector d(vx, vy, 0);
	d.normalize();
	d.x = d.x / 120 + (fraction / 360);
	d.y = d.y / 120 + (fraction / 360);
	dir = d;
}