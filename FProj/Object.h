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
void setPerspective(float fov, float aspect, float zNear, float zFar);


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
	ParticleEmitter(float x, float y, int behavior);
	~ParticleEmitter();
	void setPos(float x, float y);
	void Update(float elapsed);
	void Render();
	Vector position;
	Vector gravity;
	float maxLifetime;
	int behavior;
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
	void setRot(float amt);
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

class Sphere : public Entity
{
public:
	Sphere(GLuint tex, float rad, float x, float y);
	void Update();
	void RenderCube();
	void RenderSphere();
	Matrix buildYaw();
	void setRad(float i);
private:
	float radius;
};
class Square : public Entity{
public:
	Square(int texture, float x, float y, float h, float w);
	std::vector<Vector> getVerts();
	bool overlaps(Square& b);
	void Draw();
	void reinit(int texture, float x, float y, float h, float w);
	void resize(float h, float w);
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
	void reinit(int texture, float x, float y, float radius);
	void setRad(float r);
	float getRad();
private:
	float radius;
};

class sBullet : public Square{
public:	
	sBullet(int tex, float xp, float yp, int tier, int behavior, float w, float h);
	void Update();
	void Render();
	void Orient(float angle);
	int getTier();
	void explode();
	void applyEffect(int i);
private:
	int tier;
	Vector dir;
	int movtype;
	int pframes;
	ParticleEmitter pe;
	float mult;
};

class cBullet : public Circle{
public:
	cBullet(int tex, float xp, float yp, int tier, int behavior, Vector vel);
	cBullet(int tex, float xp, float yp, int tier, int behavior);
	void Orient(float angle);
	void Update();
	void Update(float tx, float ty);
	void Render();
	int damage(int i);
	void change(GLuint tex, int tier, int behavior);
	int getHealth();
	void explode();
	int getTier();
	void applyEffect(int i);
	float tSize(int tier);
	Vector tmove(int type);
	void setDir(Vector dir);
private:
	int tier;
	int movtype;
	Vector dir;
	int health;
	int pframes;
	ParticleEmitter pe;
	float mult;
	bool stagged;
	int aframes;
};

class cField : public Circle{
public:
	cField(int texture, float x, float y, float radius, int effect);
	void applyEffect(std::vector<cBullet>& cbullets, std::vector<sBullet>& sbullets, std::queue<int>& cBuck, std::queue<int>& sBuck, std::vector<GLuint> btex, Vector target);
	void applyEffect(std::vector<cBullet>& cbullets, std::vector<sBullet>& sbullets, std::queue<int>& cBuck, std::queue<int>& sBuck);
	bool Update(std::queue<int>& buk, int myind);
	bool Update();
	void beginani(int i);
	void Render();
	void Reset();
	void Reset(GLuint i, int effnum);
	void slowkill();
	int getEff();
private:
	int aniframes;
	int framesactive;
	int effect;
	int ani;
	Sphere crystal;
	Mix_Chunk * csound;
	Mix_Chunk * fsound;
};

class sField : public Square{

};

class tBullet : public Particle{
public:
	tBullet(float ox, float oy, float dx, float dy, int tier);
	Vector origin;
	Vector destination;
	int tier;
	int ftime;
	bool active;
};

class PTransit{
public:
	PTransit();
	void Update(std::vector<GLuint> BTex, std::vector<cBullet>& bullets, std::queue<int>& bucket, std::vector<GLuint>& ftex, std::vector<cField>& fields, std::queue<int>& fbuck);
	void Render();
	void add(Vector pos);
	void add(Vector pos, int i);
	std::vector<tBullet> parts;
	std::queue<int> pbucket;
private:
	Mix_Chunk *msound;
	Mix_Chunk* ssound;
};

class Enemy : public Circle{
public:
	Enemy(int tex, float xp, float yp, int tier);
	void Update();
	void Render();
	void shoot(std::vector<cBullet>& cbullets, std::vector<sBullet>& sbullets, std::queue<int>& cBuck, std::queue<int>& sBuck, std::vector<GLuint> tex, Vector target, PTransit& pt);
	int hit(std::vector<cBullet>& bullets, PTransit& ptrans);
	int explode(std::vector<cBullet>& cbullets, std::vector<sBullet>& sbullets, std::queue<int>& cBuck, std::queue<int>& sBuck, PTransit& trans);
	void reinit(int tex, float xp, float yp, int tier);
	int damage(int i);
	int getHealth();
	int getTier();
private:
	Vector dir;
	int tier;
	int health;
	int framesalive;
	int lastfireframe;
	int firethresh;
	int lastfireframe2;
	int pnum;
	std::queue<int> bulletcham;
};

class chargeBar:public Entity{
public:
	chargeBar(GLuint bg, GLuint frame, GLuint col, GLuint u, float x, float y);
	void Render();
	void Update(float i, float j);
private:
	Square back;
	Square f;
	Square c;
	Square pfil;
};
class chargeBar2 :public Entity{
public:
	chargeBar2(GLuint bg, GLuint frame, GLuint col, GLuint u, float x, float y);
	void Render();
	void Update(float i, float j);
private:
	Square back;
	Square f;
	Square c;
	Square pfil;
};

class Player : public Circle{
public:
	Player(int tex, float xp, float yp, int pnum, int character, std::vector<GLuint>& ftex);
	Player();
	void Shoot(std::vector<GLuint> BTex, std::vector<cBullet>& bullets, std::queue<int>& bucket);
	void Update(PTransit& ptrans, bool Foc, bool charging);
	void Render();
	bool hit(std::vector<cBullet>& cbullets, std::vector<sBullet>& sbullets, std::queue<int>& cBuck, std::queue<int>& sBuck, PTransit& ptrans, std::vector<cField>& fields, std::queue<int>& fbuck);
	void hinput(bool right);
	void vinput(bool up);
	int damage(int i);
	int getScore();
	int addScore(int i);
	void startinv();
	void incScore(PTransit& ptrans);
	GLuint getftex();
	float getfill();
	float getcharge();
	int addSpScore(int i);
	int addSpScore2(int i);
private:
	float lastfire;
	Vector vel;
	int score;
	int lives;
	int wins;
	int pnum;
	int character;
	int invframes;
	bool focus;
	GLuint ftex;
	int exthresh;
	int expts;
	float tbonus;
	int lastscoreframes;
	int totpts;
	cField pfield;
	Circle hitbox;
	int spoints;
	int cpoints;
	int speff;
	cField spcirc;
	int spframes;
	int angle;
	Mix_Chunk *dsound;
	Mix_Chunk *ssound;
	Mix_Chunk *ecsound;
	Mix_Chunk *spsound;
	Mix_Chunk *susound;
	Mix_Chunk *esound;
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
	std::vector<cField> pc1fields;
	std::vector<cField> pc2fields;
	std::vector<sField> ps1fields;
	std::vector<sField> ps2fields;
	std::queue<int> fs1buck;
	std::queue<int> fc1buck;
	std::queue<int> fs2buck;
	std::queue<int> fc2buck;
	void Update(Player& p, Player& p2);
};

class stage : public Square{
public:
	stage(GLuint background, GLuint mparts, int snum);
	void Update();
	void Render();
	std::vector<Circle> stuff;
	int snum;
};
