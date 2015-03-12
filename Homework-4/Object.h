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
	float getxpen(Entity& other);
	float getypen(Entity& other);
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
	void Update();
	void Render();
	void setDir(bool right);
private:
	float vspeed;
	float angle;
	float hspeed;
};

class Wall : public Entity{
public:
	Wall(int tex, float xp, float yp, float h, float w);
	void Update();
	void Render();
};

class Enemy : public Entity{
public:
	Enemy(int tex, float xp, float yp, float h, float w);
	void Shoot(int BTex, std::vector<Bullet>& bullets);
	void Update(std::vector<Wall>& walls);
	void Render();
	bool hit(std::vector<Bullet>& bullets);
	void bRepo(int ind, std::vector<Bullet>& bullets);
	void setacc(float acc);
	void xwallcols(std::vector<Wall>& walls);
	void ywallcols(std::vector<Wall>& walls);
private:
	int aniframe;
	std::vector<int> LAni;
	std::vector<int> RAni;
	std::vector<int> Ani;
	int anind;
	float hspeed;
	float vspeed;
	float haccel;
	float xfric;
	bool rmov;
	bool dcol;
	bool ucol;
	bool lcol;
	bool rcol;
};


class Player : public Entity{
public:
	Player(int tex, float xp, float yp, float h, float w);
	Player();
	void Shoot(int BTex, std::vector<Bullet>& bullets);
	void Update();
	void Update(std::vector<Wall>& walls);
	void Render();
	bool hit(std::vector<Bullet>& bullets);
	bool hit(std::vector<Enemy>& enemies);
	void bRepo(int ind, std::vector<Bullet>& bullets);
	void jump();
	void hinput(bool right);
	void xwallcols(std::vector<Wall>& walls);
	void ywallcols(std::vector<Wall>& walls);
private:
	int aniframe;
	float lastfire;
	std::vector<int> LAni;
	std::vector<int> RAni;
	std::vector<int> Ani;
	int anind;
	float hspeed;
	float vspeed;
	float haccel;
	float xfric;
	bool dcol;
	bool ucol;
	bool lcol;
	bool rcol;
	bool rface;
};


