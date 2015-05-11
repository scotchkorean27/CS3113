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
	void handleDownKey();
	void handleUpKey();
	void handleSelect();
	void handleBack();
	void handleDownBut();
	void handleUpBut();
	void handlePause();
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
	std::vector<Sphere> icons;
	std::vector<GLuint> gamtex;
	std::vector<GLuint> bultex;
	std::vector<GLuint> enetex;
	std::vector<GLuint> ftex;
	bCont masterBullets;
	eCont puppeteer;
	int lasteneframe;
	int gameframes;
	int enethresh;
	int enetexind;
	int bultexind;
	Vector bgvel;
	Vector bgpos;
	float perlinValue;
	SDL_GameController *controller;
	PTransit p1t;
	PTransit p2t;
	chargeBar p1c;
	chargeBar p2c;
	chargeBar2 apocalypsetimer;
	int winner;
	Mix_Chunk * ssound;
	Mix_Chunk * bsound;
	Mix_Chunk * aesound;
	Mix_Chunk * a2sound;
	Mix_Chunk * dsound;
};