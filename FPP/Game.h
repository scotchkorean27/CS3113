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
	int state;
	int menu;
	int menu2;
	int menu3;
	Player p1;
	Player p2;
	std::vector<stringGraphic> texts;
	std::vector<GLuint> bgtex;
	std::vector<GLuint> chartex;
	std::vector<std::string> charnames;
	std::vector<std::string> skill1;
	std::vector<std::string> skill2;
	GLuint fonttex;
	SDL_Window* displayWindow;
	SDL_Event event;
	std::vector<Mix_Music *> music;
	int cmusic;
	std::vector<Square> icons;
	std::vector<GLuint> gamtex;
	std::vector<GLuint> bultex;
	bCont masterBullets;
};