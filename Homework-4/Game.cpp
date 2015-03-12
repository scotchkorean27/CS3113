#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <SDL_surface.h>
#include "Game.h"
#include "Graphic.h"
#include "Object.h"
#include <ctime>
#include <sstream>
#include <vector>
using namespace std;

#define FIXED_TIMESTEP 0.033333f
#define MAX_TIMESTEPS 6

Game::Game()
: done(false),
lastroundframes(0),
leftovertime(0),
state(0),
pbind(0),
ebind(0),
eright(false),
highscore(0),
lastespawn(0),
player(){
	Init();
	RUN();

}
Game::~Game(){
	SDL_Quit();
}
void Game::Init(){
	SDL_Init(SDL_INIT_VIDEO);
	glMatrixMode(GL_PROJECTION);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	glViewport(0, 0, 800, 800);
	glOrtho(-1, 1, -1, 1, -1, 1);
	
	SDL_GL_MakeCurrent(displayWindow, context);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	srand(time(0));

	fonttex = (LoadTexture("font1.png"));
	playtex = (LoadTexture("characters_1.png"));
	bultex = (LoadTexture("bullet.png"));
	flotex = LoadTexture("JnRTiles.png");


	
	Player p(playtex, 0, 0, 0.075, 0.075);
	player = p;
	Wall w(flotex, 0, -0.9, 0.05, 1);
	Wall w2(flotex, -0.95, 0.05, 2, 0.05);
	Wall w3(flotex, 0.95, 0.05, 2, 0.05);
	Wall w4(flotex, 0.6, -0.4, 0.05, 0.4);
	Wall w5(flotex, -0.6, -0, 0.05, 0.4);
	Wall w6(flotex, 0.6, 0.4, 0.05, 0.4);
	walls.push_back(w);
	walls.push_back(w2);
	walls.push_back(w3);
	walls.push_back(w4);
	walls.push_back(w5);
	walls.push_back(w6);
}
void Game::Update(){
	if (state == 1){
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		lastroundframes++;


		if (keys[SDL_SCANCODE_LEFT]){
			player.hinput(false);
		}
		else if (keys[SDL_SCANCODE_RIGHT]){
			player.hinput(true);
		}
		player.Update(walls);

		for (int i = 0; i < pbullets.size(); ++i){
			pbullets[i].Update();
		}
		for (int i = 0; i < enemies.size(); ++i){
			enemies[i].Update(walls);
		}
		
		lastespawn++;

		if (lastespawn > 160 - kcount * (rand() / RAND_MAX / 2 + 0.5)){
			if (aliveE.size() > 0){
				double x = (double)rand() / RAND_MAX - 0.5;
				int ind = aliveE.front();
				enemies[ind].setPos(x, 1);
				enemies[ind].activate();
				aliveE.pop();
			}
			else{
				double x = (double)rand() / RAND_MAX - 0.5;
				Enemy e(playtex, x, 1, 0.075, 0.075);
				e.activate();
				enemies.push_back(e);
			}
			lastespawn = 0;
		}
		
		for (int i = 0; i < enemies.size(); ++i){
			if (enemies[i].hit(pbullets)){
				aliveE.push(i);
				score += 50 * (kcount * 100) / lastroundframes;
				kcount++;
			}
			
		}

		if (player.hit(enemies)){
			player.setPos(0, 0);
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
		DrawStr(fonttex, "I'm not creative", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.3);
		DrawStr(fonttex, "I hope that's obvious", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.2);
		ostringstream intconv;
		intconv << highscore;
		DrawStr(fonttex, "Current High Score: " + intconv.str(), 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.1);
		intconv.flush();
		DrawStr(fonttex, "Press SPACE to start.", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0);
	}
	else if (state == 1){
		player.Render();
		for (int i = 0; i < enemies.size(); ++i){
			enemies[i].Render();
		}
		for (int i = 0; i < pbullets.size(); ++i){
			pbullets[i].Render();
		}
		for (int i = 0; i < walls.size(); ++i){
			walls[i].Render();
		}
		ostringstream intconv;
		intconv << score;
		DrawStr(fonttex, "Score: " + intconv.str(), 0.1, -0.06, 1, 1, 1, 1, -0.95, 0.95);
		intconv.flush();
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
						lastroundframes = 1;
						kcount = 0;
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
				else if (event.key.keysym.scancode == SDL_SCANCODE_UP){
					if (state == 1){
						player.jump();
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