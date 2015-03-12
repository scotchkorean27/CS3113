
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <SDL_surface.h>
#include "Physics.h"
#include "Graphic.h"
#include <sstream>
using namespace std;
#include "Object.h"

#define bulvel 0.7
#define adlen 0.03
#define FIXED_TIMESTEP 0.033333f
/*Entity has fields:
float x;
float y;
float height;
float width;
int tex;
int index;
int SCX;
int SCY;
bool active;
*/


Entity::Entity(int texture, float xp, float yp, float h, float w)
:tex(texture),
x(xp),
y(yp),
height(h),
width(w),
SCX(1),
SCY(1),
rotation(0)
{
}
Entity::~Entity(){

}
void Entity::Draw(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -width, height, -width, -height, width, -height, width, height };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	float u = (float)(((int)index) % SCX) / (float)SCX;
	float v = (float)(((int)index) / SCX) / (float)SCY;

	

	float spriteWidth = 1.0 / (float)SCX;
	float spriteHeight = 1.0 / (float)SCY;
	GLfloat quadUVs[] = { u, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight,
		u + spriteWidth, v
	};
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}
void Entity::setPos(float xp, float yp){
	x = xp;
	y = yp;
}
void Entity::move(float xp, float yp){
	x += xp;
	y += yp;
}
bool Entity::overlaps(Entity& b){
	return (dy() < b.uy() && uy() > b.dy() && lx() < b.rx() && rx() > b.lx());
}
float Entity::getxpen(Entity& b){
	return fabs(x - b.x) - width - b.width;
}
float Entity::getypen(Entity& b){
	return fabs(y - b.y) - height - b.height;
}
float Entity::lx(){
	return x - width;
}
float Entity::rx(){
	return x + width;
}
float Entity::uy(){
	return y + height;
}
float Entity::dy(){
	return y - height;
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
void Entity::sheetDets(int scx, int scy){
	SCX = scx;
	SCY = scy;
}
void Entity::updateInd(int newind){
	index = newind;
}
int Entity::getInd(){
	return index;
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

Wall::Wall(int tex, float xp, float yp, float h, float w) : Entity(tex, xp, yp, h, w){
	sheetDets(18, 1);
	updateInd(0);
}

void Wall::Update(){

}

void Wall::Render(){
	Draw();
}

Bullet::Bullet(int tex, float xp, float yp, float h, float w, bool rmov) : Entity(tex, xp, yp, h, w){
	if (rmov){
		hspeed = bulvel;
	}
	else{
		hspeed = -bulvel;
	}
	angle = 0;
	sheetDets(1, 1);
	updateInd(0);
	vspeed = 0;
}
Bullet::~Bullet(){

}
void Bullet::Update(){
	if (isactive()){
		move(hspeed * FIXED_TIMESTEP, vspeed * FIXED_TIMESTEP);
		incRot(FIXED_TIMESTEP * 45);
	}
}
void Bullet::Render(){
	if (isactive()){
		Draw();
	}
}

void Bullet::setDir(bool right){
	if (right){
		hspeed = bulvel;
	}
	else{
		hspeed = -bulvel;
	}
}

Player::Player(int tex, float xp, float yp, float h, float w)
: Entity(tex, xp, yp, h, w),
aniframe(0),
lastfire(0),
anind(0),
Ani({ 39, 40, 41 }),
LAni({15, 16, 17}),
RAni({27, 28, 29}),
hspeed(0),
vspeed(0),
haccel(0.6),
xfric(0.7)
{
	sheetDets(12, 8);
	updateInd(27);
}
Player::Player()
:Entity(0, 0, 0, 0, 0){
	deactivate();
}
void Player::Shoot(int BTex, vector<Bullet>& bullets){
	if (isactive()){
		float cfire = (float)SDL_GetTicks() / 1000.0f;
		if (cfire - lastfire > 0.5){
			Bullet nBull(BTex, getx(), gety(), 0.0375, 0.0375, rface);
			lastfire = (float)SDL_GetTicks() / 1000.0f;
			nBull.activate();
			bullets.push_back(nBull);
		}
	}

}
void Player::bRepo(int ind, vector<Bullet>& bullets){
	if (isactive()){
		float cfire = (float)SDL_GetTicks() / 1000.0f;
		if (cfire - lastfire > 0.5){
			bullets[ind].setPos(getx(), gety());
			bullets[ind].activate();
			bullets[ind].setDir(rface);
			lastfire = (float)SDL_GetTicks() / 1000.0f;
		}
	}
}
void Player::hinput(bool right){
		if (right){
			hspeed += haccel * FIXED_TIMESTEP;
			Ani = RAni;
			rface = true;
		}
		else{
			hspeed -= haccel * FIXED_TIMESTEP;
			Ani = LAni;
			rface = false;
		}
}
void Player::Update(){
	if (isactive()){
		hspeed = applyfriction(hspeed, xfric);
		move(hspeed * FIXED_TIMESTEP, vspeed * FIXED_TIMESTEP);	
	}
}
void Player::Update(vector<Wall>& walls){
	if (isactive()){
		lcol = false;
		rcol = false;
		ucol = false;
		dcol = false;
		vspeed = applygravity(vspeed);
		move(0, vspeed * FIXED_TIMESTEP);
		ywallcols(walls);
		hspeed = applyfriction(hspeed, xfric);
		move(hspeed * FIXED_TIMESTEP, 0);
		xwallcols(walls);
	}
}
void Player::jump(){
	if (dcol){
		vspeed = 2;
	}
}
void Player::Render(){
	if (isactive()){
		aniframe++;
		if(aniframe > 300) {
			anind = (anind + 1) % 3;
			updateInd(Ani[anind]);
			aniframe = 0;
		}
		Draw();
	}
}
bool Player::hit(vector<Bullet>& bullets){
	if (isactive()){
		for (int i = 0; i < bullets.size(); ++i){
			if (overlaps((bullets[i])) && (bullets[i]).isactive()){
				bullets[i].setPos(-1.5, -1.5);
				bullets[i].deactivate();
				return true;
			}
		}
	}
	return false;
}
bool Player::hit(vector<Enemy>& bullets){
	if (isactive()){
		for (int i = 0; i < bullets.size(); ++i){
			if (overlaps((bullets[i])) && (bullets[i]).isactive()){
				//Compensates for white space
				if (getxpen(bullets[i]) < -0.08){
					bullets[i].setPos(-1.5, -1.5);
					bullets[i].deactivate();
					return true;
				}
			}
		}
	}
	return false;
}
void Player::xwallcols(vector<Wall>& walls){
	for (int i = 0; i < walls.size(); ++i){
		if (overlaps(walls[i])){
			float xpen = getxpen(walls[i]);
			//compensates for white space
			if (getx() < walls[i].getx()){
				move(xpen, 0);
				hspeed = 0;
				rcol = true;
			}
			else{
				move(-xpen, 0);
				hspeed = 0;
				lcol = true;
			}
			
		}
	}
}
void Player::ywallcols(vector<Wall>& walls){
	for (int i = 0; i < walls.size(); ++i){
		if (overlaps(walls[i])){
			float ypen = getypen(walls[i]);
			if (gety() < walls[i].gety()){
				move(0, ypen - 0.001);
				vspeed = 0;
				ucol = true;
			}
			else{
				move(0, -ypen + 0.001);
				vspeed = 0;
				dcol = true;
			}

		}
	}
}

Enemy::Enemy(int tex, float xp, float yp, float h, float w)
: Entity(tex, xp, yp, h, w),
aniframe(0),
anind(0),
vspeed(0),
hspeed(0),
haccel(-0.6),
rmov(false),
xfric(0.7)
{
	sheetDets(12, 8);
	Ani = { 9, 10, 11};
	RAni = { 33, 34, 35 };
	LAni = { 21, 22, 23 };
	updateInd(36);
	Ani = LAni;
}
void Enemy::Shoot(int BTex, vector<Bullet>& bullets){
	if (isactive()){
		Bullet nBull(BTex, getx(), gety() + 0.1, 0.0375, 0.0375, false);
		bullets.push_back(nBull);
	}
}
void Enemy::bRepo(int ind, vector<Bullet>& bullets){
	if (isactive()){
		bullets[ind].setPos(getx(), gety());
		bullets[ind].activate();
	}
}
void Enemy::Update(vector<Wall>& walls){
	if (isactive()){
		hspeed += haccel * FIXED_TIMESTEP;
		lcol = false;
		rcol = false;
		ucol = false;
		dcol = false;
		vspeed = applygravity(vspeed);
		move(0, vspeed * FIXED_TIMESTEP);
		ywallcols(walls);
		hspeed = applyfriction(hspeed, xfric);
		move(hspeed * FIXED_TIMESTEP, 0);
		xwallcols(walls);
	}
}
void Enemy::Render(){
	if (isactive()){
		aniframe++;

		if (aniframe > 300) {
			anind = (anind + 1) % 3;
			updateInd(Ani[anind]);
			aniframe = 0;
		}

		Draw();
	}
}
bool Enemy::hit(vector<Bullet>& bullets){
	for (int i = 0; i < bullets.size(); ++i){
		if (overlaps((bullets[i])) && (bullets[i]).isactive() && isactive()){
			//Compensates for white space
			if (getxpen(bullets[i]) < -0.07){
				(bullets[i]).deactivate();
				deactivate();
				return true;
			}
		}
	}
	return false;
}
void Enemy::setacc(float acc){
	haccel = acc;
}

void Enemy::xwallcols(vector<Wall>& walls){
	for (int i = 0; i < walls.size(); ++i){
		if (overlaps(walls[i])){
			float xpen = getxpen(walls[i]);
			rmov = !rmov;
			haccel *= -1;
			if (getx() < walls[i].getx()){
				move(xpen, 0);
				hspeed = 0;
				rcol = true;
				Ani = LAni;
			}
			else{
				move(-xpen, 0);
				hspeed = 0;
				lcol = true;
				Ani = RAni;
			}

		}
	}
}
void Enemy::ywallcols(vector<Wall>& walls){
	for (int i = 0; i < walls.size(); ++i){
		if (overlaps(walls[i])){
			float ypen = getypen(walls[i]);
			if (gety() < walls[i].gety()){
				move(0, ypen - 0.001);
				vspeed = 0;
				ucol = true;
			}
			else{
				move(0, -ypen + 0.001);
				vspeed = 0;
				dcol = true;
			}

		}
	}
}