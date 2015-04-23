#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <SDL_surface.h>
#include "Game.h"
#include "Graphic.h"
#include "Object.h"
#include "Physics.h"
#include <ctime>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <SDL_mixer.h>
#include "PerlinNoise.h"
using namespace std;

#define FIXED_TIMESTEP 0.033333f
#define MAX_TIMESTEPS 6


Game::Game()
: done(false),
leftovertime(0),
state(0),
pbind(0),
ebind(0),
highscore(0),
lastespawn(0),
parttimer(0),
player(),
spawnpe(0, 0, true),
spawnpoint(0, 0, 0){
	Init();
	RUN();

}
Game::~Game(){
	Mix_FreeMusic(music);
	SDL_Quit();
}
void Game::Init(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	GLuint VertexArrayID;
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1, 1, -1, 1, -1, 1);
	
	SDL_GL_MakeCurrent(displayWindow, context);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	srand(time(0));

	fonttex = (LoadTexture("font1.png"));
	playtex = (LoadTexture("ship.png"));
 	bultex = (LoadTexture("laserBlue02.png"));
	astex = LoadTexture("asteroid.png");


	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	//Music is Stage 5 boss theme from Mystic Square.  I dunno, it just feels really..."asteroids"
	music = Mix_LoadMUS("MSs5.mp3");
	Player p(playtex, 0, 0);
	player = p;
}
void Game::Update(){
	if (state == 1){
		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_UP]){
			player.accel();
		}
		else if (keys[SDL_SCANCODE_DOWN]){
			player.raccel();
		}
		if (keys[SDL_SCANCODE_LEFT]){
			player.hinput(false);
		}
		else if (keys[SDL_SCANCODE_RIGHT]){
			player.hinput(true);
		}
		
		player.Update();

		for (int i = 0; i < pbullets.size(); ++i){
			pbullets[i].Update();
		}
		for (int i = 0; i < enemies.size(); ++i){
			enemies[i].Update();
		}
		
		lastespawn++;
		

		if (parttimer < 1){
			//Spawn enemy
			float ang = rand() % 360;
			Vector v(cos(toRads(ang)), sin(toRads(ang)), 0);
			spawnpoint = v;
			ParticleEmitter pe2(spawnpoint.x, spawnpoint.y, true);
			spawnpe = pe2;
			parttimer = 200 - (score / 4000) * (rand() / RAND_MAX / 2 + 0.5);
			lastespawn = 0;
		}

		if (parttimer > 0){
			parttimer--;
			spawnpe.Update(FIXED_TIMESTEP);
			if (parttimer == 0){
				Enemy e(astex, spawnpoint.x, spawnpoint.y, 1);
				e.activate();
				enemies.push_back(e);
				
			}
		}
		
		for (int i = 0; i < enemies.size(); ++i){
			//Check for enemy kills
			if (enemies[i].hit(pbullets)){
				score += enemies[i].explode(enemies);
				shaking = 300;
			}
			
		}

		if (player.hit(enemies)){
			//check for player death
			player.setPos(0, 0);
			Mix_HaltMusic();
			state = 2;
		}

	}
	else if (state == 2){
		for (int i = 0; i < pbullets.size(); i++){
			pbullets[i].deactivate();
		}
		for (int i = 0; i < enemies.size(); i++){
			enemies[i].deactivate();
		}
		player.deactivate();
		if (score > highscore){
			highscore = score;
		}
	}
}
void Game::Render(){
	glClear(GL_COLOR_BUFFER_BIT);
	if (state == 0){
		DrawStr(fonttex, "Asteroids!", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.3);
		DrawStr(fonttex, "Wasn't very creative here.", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.2);
		ostringstream intconv;
		intconv << highscore;
		DrawStr(fonttex, "Current High Score: " + intconv.str(), 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.1);
		intconv.flush();
		DrawStr(fonttex, "Press SPACE to start.", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0);
	}
	else if (state == 1){
		glLoadIdentity();
		player.Render();
		ostringstream intconv;
		for (int i = 0; i < pbullets.size(); ++i){
			pbullets[i].Render();
		}
		for (int i = 0; i < enemies.size(); ++i){
			enemies[i].Render();
		}
		if (parttimer > 0){
			spawnpe.Render();
		}

		
		perlinValue += FIXED_TIMESTEP;
		float val = 0;
		float val2 = 0;
		
		if (shaking > 0){
			shaking--;
			float coord[2] = { perlinValue, 0.0 };
			val = noise2(coord) * score / 20000;

			coord[1] = 0.5f;
			val2 = noise2(coord) * score / 20000;
		}
		
		intconv << score;
		DrawStr(fonttex, "Score: " + intconv.str(), 0.1, -0.06, 1, 1, 1, 1, -0.95 + val, 0.95 + val2);
		intconv.flush();
		if (introani < 3000){
			float mval = mapValue(introani, 0, 3000, 0, 1);
			DrawStr(fonttex, "SPACE to shoot!", 0.1, -0.06, 1, 1, 1, 1, easeOutElastic(1, -0.3, mval), 0);
			introani++;
		}
		else if (introani < 5000){
			float mval = mapValue(introani, 3000, 5000, 0, 1);
			DrawStr(fonttex, "SPACE to shoot!", 0.1, -0.06, 1, 1, 1, 1, easeIn(-0.3, -2, mval), 0);
			introani++;
		}
	}
	else if (state == 2){
		ostringstream intconv;
		intconv << score;
		DrawStr(fonttex, "Your Score: " + intconv.str(), 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.1);
		intconv.flush();
		ostringstream intconv2;
		intconv2 << highscore;
		DrawStr(fonttex, "Current High Score: " + intconv2.str(), 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.0);
		intconv2.flush();
		DrawStr(fonttex, "Press SPACE to return to title.", 0.1, -0.06, 1, 1, 1, 1, -0.45, -0.1);
	}
	SDL_GL_SwapWindow(displayWindow);
}
void Game::RUN(){
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			else if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
					if (state == 0){
						state = 1;
						player.setPos(0, 0);
						player.activate();
						score = 0;
						enemies = {};
						pbullets = {};
						parttimer = 0;
						lastespawn = 0;
						Mix_PlayMusic(music, -1);
						introani = 0;
					}
					else if (state == 1){
						if (pbullets.size() < 30){
							player.Shoot(bultex, pbullets);
						}
						else{
							player.bRepo(pbind, pbullets);
							pbind = (pbind + 1) % 30;
						}
						
					}
					else{
						state = 0;
					}
				}
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		float fixedelapsed = elapsed + leftovertime;
		if (fixedelapsed > FIXED_TIMESTEP * MAX_TIMESTEPS){
			fixedelapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
		}
		while (fixedelapsed >= FIXED_TIMESTEP){
			fixedelapsed -= FIXED_TIMESTEP;
			Update();
		}
		leftovertime = fixedelapsed;
		Render();

	}

}