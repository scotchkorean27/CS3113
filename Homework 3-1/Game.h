#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_surface.h>
#include <vector>
#include <set>
#include "Object.h"
#include "Graphic.h"

class Game{
public:
	Game();
	~Game();
	void Init();
	void Update(float elapsed);
	void Render(float elapsed);
	void RUN();
private:
	bool done;
	float lastFrameTicks;
	SDL_Window* displayWindow;
	SDL_Event event;
	std::vector<Enemy> enemies;
	std::set<int> aliveE;
	std::vector<Bullet> pbullets;
	std::vector<Bullet> ebullets;
	int state;
	bool eright;
	GLuint fonttex;
	GLuint playtex;
	GLuint bultex;
	Player player;
	float lastefire;
	int pbind;
	int ebind;
	bool win;
	int score;
	float lastroundticks;
	int highscore;
};
