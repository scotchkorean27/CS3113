#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_surface.h>
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
	std::queue<int> aliveE;
	std::vector<Bullet> pbullets;
	std::vector<Bullet> ebullets;
	std::vector<Wall> walls;
	int state;
	bool eright;
	GLuint fonttex;
	GLuint playtex;
	GLuint bultex;
	Player player;
	int lastespawn;
	int kcount;
	int pbind;
	int ebind;
	bool win;
	int score;
	int lastroundframes;
	int highscore;
	int flotex;
};