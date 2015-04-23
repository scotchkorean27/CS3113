#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_surface.h>
#include <SDL_mixer.h>
#include <vector>
#include <queue>
#include "Object.h"
#include "Graphic.h"

class Game{
public:
	Game();
	~Game();
	void Init();
	void Update();
	void Render();
	void RUN();
private:
	bool done;
	float lastFrameTicks;
	float leftovertime;
	SDL_Window* displayWindow;
	SDL_Event event;
	std::vector<Enemy> enemies;
	std::queue<int> bucket;
	std::vector<Bullet> pbullets;
	int state;
	GLuint fonttex;
	GLuint playtex;
	GLuint bultex;
	GLuint astex;
	Player player;
	int lastespawn;
	int pbind;
	int ebind;
	int score;
	int highscore;
	int parttimer;
	ParticleEmitter spawnpe;
	Vector spawnpoint;
	int shaking;
	int introani;
	Mix_Music *music;
	float perlinValue;
};