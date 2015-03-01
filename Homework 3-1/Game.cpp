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
using namespace std;



Game::Game()
: done(false),
lastFrameTicks(0),
state(0),
pbind(0),
ebind(0),
eright(false),
highscore(0),
lastefire(0),
player(){
	Init();
	RUN();

}
Game::~Game(){
	SDL_Quit();
}
void Game::Init(){
	SDL_Init(SDL_INIT_VIDEO);
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

	
	for (int i = 0; i < 40; ++i){
		Enemy en(playtex, 0, 0, 0.075, 0.075);
		en.deactivate();
		enemies.push_back(en);
	}
	Player p(playtex, 0, -0.9, 0.075, 0.075);
	player = p;
}
void Game::Update(float elapsed){
	if (state == 1){
		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		

		if (keys[SDL_SCANCODE_LEFT]){
			player.Update(elapsed, false);
		}
		else if (keys[SDL_SCANCODE_RIGHT]){
			player.Update(elapsed, true);
		}

		bool bounce = false;
		for (int i = 0; i < enemies.size(); ++i){
			if (enemies[i].Update(elapsed, aliveE.size(), eright)){
				bounce = true;
			}
		}
		
		if (bounce){
			for (int i = 0; i < enemies.size(); ++i){
				if (eright){
					enemies[i].move(-0.01, 0);
				}
				else{
					enemies[i].move(0.01, 0);
				}
				enemies[i].advance();
				if (enemies[i].gety() < -0.8){
					state = 2;
					win = false;
				}
			}
			eright = !eright;
		}

		
		
		for (int i = 0; i < pbullets.size(); ++i){
			pbullets[i].Update(elapsed);
		}
		for (int i = 0; i < ebullets.size(); ++i){
			ebullets[i].Update(elapsed);
		}
		float cefire = (float)SDL_GetTicks() / 1000.0f;
		if (cefire - lastefire > 1.25 - (41 - aliveE.size()) / 40){
			set<int>::iterator aitr(aliveE.begin());
			advance(aitr, rand() % aliveE.size());
			if (ebullets.size() < 50){
				enemies[*aitr].Shoot(bultex, ebullets);
			}
			else{
				enemies[*aitr].bRepo(ebind, ebullets);
				ebind = (ebind + 1) % 50;
			}
			lastefire = cefire;
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f - lastroundticks;
		for (int i = 0; i < enemies.size(); ++i){
			if (aliveE.count(i) > 0 && enemies[i].hit(pbullets)){
				aliveE.erase(i);
				score += 5000 / (aliveE.size()+ 1) / ticks;
			}
			if (aliveE.size() == 0){
				win = true;
				score += 2000;
				state = 2;
			}
		}
		if (player.hit(ebullets)){
			player.setPos(0, -0.9f);
			win = false;
 			state = 2;
		}

		
	}
	else if (state == 2){
		for (int i = 0; i < ebullets.size(); i++){
			ebullets[i].deactivate();
		}
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
void Game::Render(float elapsed){
	glClear(GL_COLOR_BUFFER_BIT);
	if (state == 0){
		DrawStr(fonttex, "This my homework.", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.2);
		ostringstream intconv;
		intconv << highscore;
		DrawStr(fonttex, "Current High Score: " + intconv.str(), 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.1);
		intconv.flush();
		DrawStr(fonttex, "Press SPACE to start.", 0.1, -0.06, 1, 1, 1, 1, -0.45, 0);
	}
	else if (state == 1){
		player.Render(elapsed);
		for (int i = 0; i < enemies.size(); ++i){
			enemies[i].Render(elapsed);
		}
		for (int i = 0; i < pbullets.size(); ++i){
			pbullets[i].Render();
		}
		for (int i = 0; i < ebullets.size(); ++i){
			ebullets[i].Render();
		}
		ostringstream intconv;
		intconv << score;
		DrawStr(fonttex, "Score: " + intconv.str(), 0.1, -0.06, 1, 1, 1, 1, -0.95, 0.95);
		intconv.flush();
	}
	else if (state == 2){
		ostringstream intconv;
		intconv << score;
		if (win){
			DrawStr(fonttex, "Congratulations! You Win!", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.2);
		}
		else{
			DrawStr(fonttex, "Oh no! You Lost!", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.2);
		}
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
						for (int i = 0; i < enemies.size(); ++i){
							aliveE.insert(i);
							enemies[i].setPos((i % 8) * 0.2 - 0.5, ((int)i / 8) * 0.125 + 0.35);
							enemies[i].activate();
						}
						player.setPos(0, -0.9);
						player.activate();
						score = 0;
						lastroundticks = (float)SDL_GetTicks() / 1000.0f;
					}
					else if (state == 1){
						if (pbullets.size() < 30){
							player.Shoot(bultex, pbullets);
						}
						else{
							player.bRepo(pbind,pbullets);
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
		Update(elapsed);
		Render(elapsed);

	}

}
