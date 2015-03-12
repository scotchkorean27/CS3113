
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <SDL_surface.h>
using namespace std;
#include "Object.h"

#define bulvel 0.7;
#define adlen 0.03
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
		float v = (float)(((int)index) / SCY) / (float)SCY;
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

Bullet::Bullet(int tex, float xp, float yp, float h, float w, bool upmov) : Entity(tex, xp, yp, h, w){
	if (upmov){
		vel = bulvel;
	}
	else{
		vel = -bulvel;
	}
	angle = 0;
	sheetDets(1, 1);
	updateInd(0);
}
Bullet::~Bullet(){

}
void Bullet::Update(float elapsed){
	if (isactive()){
		move(0, vel * elapsed);
		incRot(elapsed * 45 );
	}
}
void Bullet::Render(){
	if (isactive()){
		Draw();
	}
}

Player::Player(int tex, float xp, float yp, float h, float w)
: Entity(tex, xp, yp, h, w),
anilapsed(0),
lastfire(0),
anind(0),
Ani({27, 28, 29})
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
			Bullet nBull(BTex, getx(), gety() + 0.1, 0.0375, 0.0375, true);
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
			bullets[ind].setPos(getx(), gety() + 0.1);
			bullets[ind].activate();
			lastfire = (float)SDL_GetTicks() / 1000.0f;
		}
	}
}
void Player::Update(float elapsed, bool right){
	if (isactive()){
	if (right){
		move(0.4 * elapsed, 0);
	}
	else{
		move(-0.4 * elapsed, 0);
	}
	}
}
void Player::Render(float elapsed){
	if (isactive()){
		anilapsed += elapsed;

		if (anilapsed > 0.3) {
			anind = (anind + 1) % 3;
			updateInd(Ani[anind]);
			anilapsed = 0.0;
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
	

Enemy::Enemy(int tex, float xp, float yp, float h, float w)
: Entity(tex, xp, yp, h, w),
anilapsed(0),
anind(0)
{
	sheetDets(12, 8);
	Ani = { 36, 37, 38 };
	updateInd(36); 
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
//Also returns whether a bounce needs to happen
bool Enemy::Update(float elapsed, int liv, bool right){
	if (isactive()){
		if (!right){
			move(-elapsed * 0.1 * (1 + 40 / liv), 0);
		}
		else if (right){
			move(elapsed * 0.1 * (1 + 40 / liv), 0);
		}
		if (getx() + 0.0375 > 1){
			return true;
		}
		else if (getx() - 0.0375 < -1){
			return true;
		}
	}
	return false;
}
void Enemy::advance(){
	if (isactive()){
		move(0, -adlen);
	}
}
void Enemy::Render(float elapsed){
	if (isactive()){
		anilapsed += elapsed;

		if (anilapsed > 0.3) {
			anind = (anind + 1) % 3;
			updateInd(Ani[anind]);
			anilapsed = 0.0;
		}

		Draw();
	}
}
bool Enemy::hit(vector<Bullet>& bullets){
	for (int i = 0; i < bullets.size(); ++i){
		if (overlaps((bullets[i])) && (bullets[i]).isactive() && isactive()){
			(bullets[i]).deactivate();
			deactivate();
			return true;
		}
	}
	return false;
}