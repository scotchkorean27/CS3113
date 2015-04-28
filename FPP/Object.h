#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <queue>
#include <vector>
#include <SDL_surface.h>
#include <SDL_mixer.h>

float toRads(float deg);



class Matrix {
public:

	Matrix();

	union {
		float m[4][4];
		float ml[16];
	};

	void identity();
	Matrix operator * (const Matrix &m2);
	
};

class Vector {
public:

	Vector();
	Vector(float x, float y, float z);

	float length();
	void normalize();
	float dotprod(float x, float y);
	float x;
	float y;
	float z;
	float angle();
	Vector operator * (const Matrix &m);
};

class Particle {
public:
	Vector position;
	Vector velocity;
	float lifetime;
};

class ParticleEmitter {
public:
	ParticleEmitter(unsigned int particleCount);
	ParticleEmitter(float x, float y, bool imp);
	~ParticleEmitter();
	void setPos(float x, float y);
	void Update(float elapsed);
	void Render();
	Vector position;
	Vector gravity;
	float maxLifetime;
	bool implode;
	std::vector<Particle> particles;
};

class Entity{
public:
	Entity(int texture, float xp, float yp);
	~Entity();
	void setPos(float xp, float yp);
	void move(float xp, float yp);
	bool isactive();
	void activate();
	void deactivate();
	float getx();
	float gety();
	float getRot();
	void incRot(float amt);
	float getxpen(Entity& other);
	float getypen(Entity& other);
	Matrix buildTMat();
	int tex;
private:
	float x;
	float y;
	bool active;
	float rotation;
};

class Square : public Entity{
public:
	Square(int texture, float x, float y, float h, float w);
	std::vector<Vector> getVerts();
	bool overlaps(Square& b);
	void Draw();
private:
	std::vector<Vector> vertices;
	float height;
	float width;
};

class Circle : public Entity{
public:
	Circle(int texture, float x, float y, float radius);
	bool overlaps(Square& b); 
	bool overlaps(Circle& b);
	void Draw();
private:
	float radius;
	float r;
	float g;
	float b;
	float a;
};

class sBullet : public Square{
public:	
	sBullet(int tex, float xp, float yp, int tier, int behavior, float w, float h);
	void Update();
	void Render();
	void Orient(float angle);
private:
	int tier;
	Vector dir;
	int movtype;
};

class cBullet : public Circle{
public:
	cBullet(int tex,float xp, float yp, int tier, int behavior, float rad);
	void Orient(float angle);
	void Update();
	void Render();
	int damage(int i);
private:
	int tier;
	int movtype;
	Vector dir;
	int health;
};

class Enemy : public Circle{
public:
	Enemy(int tex, float xp, float yp, int tier);
	void Update();
	void Render();
	void shoot(std::vector<cBullet>& cbullets, std::vector<sBullet>& sbullets, std::queue<int>& cBuck, std::queue<int>& sBuck);
	bool hit(std::vector<cBullet>& bullets, std::queue<int>& bucket);
	int explode(std::vector<Enemy> ene, std::queue<int> bucket);
	void reinit();
	int damage(int i);
private:
	Vector dir;
	ParticleEmitter pe;
	int pframes;
	int tier;
	int health;
	int framesalive;
};

class Player : public Circle{
public:
	Player(int tex, float xp, float yp, int pnum, int character);
	Player();
	void Shoot(int BTex, std::vector<cBullet>& bullets, std::queue<int>& bucket);
	void Update();
	void Render();
	bool hit(std::vector<cBullet>& cbullets, std::vector<sBullet>& sbullets, std::queue<int>& cBuck, std::queue<int>& sBuck);
	void hinput(bool right);
	void vinput(bool up);
	int damage(int i);
private:
	float lastfire;
	Vector vel;
	int score;
	int lives;
	int wins;
	int pnum;
	int character;
};

class sField : public Square{
	sField(int tex, float xp, float yp, int behavior, float w, float h);
	void applyEffect(std::vector<cBullet>& bull, std::vector<sBullet>& sbul, std::vector<Enemy>& ene, Player& p);
};

class cField : public Circle{
	cField(int tex, float xp, float yp, int behavior, float rad);
	void applyEffect(std::vector<cBullet>& bull, std::vector<sBullet>& sbul, std::vector<Enemy>& ene, Player& p);
};



class stringGraphic : public Entity{
public:
	stringGraphic(int fontTexture, std::string text, float size, float spacing, float r, float g, float b, float a, float x, float y);
	void Draw();
	void Update();
	void beginani(int i);
	int aniframes;
	int getani();
	void setText(std::string s);
private:
	int ani;
	std::string text;
	float size;
	float spacing;
	float r;
	float g;
	float b;
	float a;
};

class eCont{
public:
	eCont();
	std::vector<Enemy> p1ene;
	std::queue<int> p1buck;
	std::vector<Enemy> p2ene;
	std::queue<int> p2buck;
};

class bCont{
public:
	bCont();
	std::vector<cBullet> p1buls;
	std::vector<cBullet> p2buls;
	std::queue<int> p1buck;
	std::queue<int> p2buck;
	std::vector<cBullet> ec1buls;
	std::vector<cBullet> ec2buls;
	std::vector<sBullet> es1buls;
	std::vector<sBullet> es2buls;
	std::queue<int> es1buck;
	std::queue<int> ec1buck;
	std::queue<int> es2buck;
	std::queue<int> ec2buck;
};