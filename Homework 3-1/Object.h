#pragma once


#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <SDL_surface.h>

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
	float lx();
	float rx();
	float uy();
	float dy();
	void sheetDets(int scx, int scy);
	void updateInd(int newind);
	int getInd();
	float getx();
	float gety();
	void incRot(float amt);
private:
	float x;
	float y;
	float height;
	float width;
	int tex;
	int index;
	int SCX;
	int SCY;
	bool active;
	float rotation;
};

class Bullet : public Entity{
public:
	Bullet(int tex, float xp, float yp, float h, float w, bool upmov);
	~Bullet();
	void Update(float elapsed);
	void Render();
private:
	float vel;
	float angle;
};

class Player : public Entity{
public:
	Player(int tex, float xp, float yp, float h, float w);
	Player();
	void Shoot(int BTex, std::vector<Bullet>& bullets);
	void Update(float elapsed, bool right);
	void Render(float elapsed);
	bool hit(std::vector<Bullet>& bullets);
	void bRepo(int ind, std::vector<Bullet>& bullets);
private:
	float anilapsed;
	float lastfire;
	std::vector<int> Ani;
	int anind;
};

class Enemy : public Entity{
public:
	Enemy(int tex, float xp, float yp, float h, float w);
	void Shoot(int BTex, std::vector<Bullet>& bullets);
	bool Update(float elapsed, int liv, bool right);
	void Render(float elapsed);
	bool hit(std::vector<Bullet>& bullets);
	void bRepo(int ind, std::vector<Bullet>& bullets);
	void advance();
private:
	float anilapsed;
	std::vector<int> Ani;
	int anind;
};
