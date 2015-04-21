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
	ParticleEmitter(float x, float y);
	~ParticleEmitter();

	void setPos(float x, float y);
	void Update(float elapsed);
	void Render();
	Vector position;
	Vector gravity;
	float maxLifetime;
	Color startColor;
	Color endColor;
	std::vector<Particle> particles;
};


class Entity{
public:
	Entity(int texture, float xp, float yp, float h, float w);
	~Entity();
	void Draw();
	void setPos(float xp, float yp);
	void move(float xp, float yp);
	bool overlaps(Entity& other);
	bool isactive();
	void activate();
	void deactivate();
	float getx();
	float gety();
	void incRot(float amt);
	float getxpen(Entity& other);
	float getypen(Entity& other);
	float getRot();
	void resize(float x, float y);
	int tex;
private:
	float x;
	float y;
	float height;
	float width;
	
	bool active;
	float rotation;
};

class Bullet : public Entity{
public:
	Bullet(int tex, float xp, float yp, float h, float w, float angle);
	~Bullet();
	void Update();
	void Render();
	void Orient();
private:
	Vector dir;
};


class Enemy : public Entity{
public:
	Enemy(int tex, float xp, float yp, int fraction);
	void Update();
	void Render();
	bool hit(std::vector<Bullet>& bullets);
	int explode(std::vector<Enemy>& enemies);
	void reinit();
private:
	Vector dir;
	int fraction;
	ParticleEmitter pe;
	int pframes;
};



class Player : public Entity{
public:
	Player(int tex, float xp, float yp);
	Player();
	void Shoot(int BTex, std::vector<Bullet>& bullets);
	void Update();
	void Render();
	bool hit(std::vector<Enemy>& enemies);
	void bRepo(int ind, std::vector<Bullet>& bullets);
	void hinput(bool right);
	void accel();
	void raccel();
private:
	float lastfire;
	Vector dir;
	Vector acc;
	Vector vel;
	bool dcol;
	bool ucol;
	bool lcol;
	bool rcol;
	bool rface;
	Mix_Chunk *jSound;
	Mix_Chunk *sSound;
};


