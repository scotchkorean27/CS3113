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
#include <iostream>
using namespace std;

#define FIXED_TIMESTEP 0.033333f
#define MAX_TIMESTEPS 6


Game::Game()
: done(false),
leftovertime(0),
masterBullets()
{
	Init();
	RUN();

}
Game::~Game(){
	SDL_Quit();
}
void Game::Init(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	GLuint VertexArrayID;
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	menu = 1;
	SDL_GL_MakeCurrent(displayWindow, context);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	srand(time(0));
	fonttex = (LoadTexture("font1.png"));
	bgtex.push_back(LoadTexture2("bgi/bg1.png")); //0
	bgtex.push_back(LoadTexture2("bgi/cselect.jpg")); //1
	bgtex.push_back(LoadTexture("bgi/black.png")); //2
	bgtex.push_back(LoadTexture("bgi/cselect2.png")); //3
	bgtex.push_back(LoadTexture("bgi/redicon.png")); //4
	bgtex.push_back(LoadTexture("bgi/blueicon.png")); //5
	bgtex.push_back(LoadTexture2("bgi/corvusStage.jpg"));//6
	bgtex.push_back(LoadTexture2("bgi/arielStage.jpg")); //7
	bgtex.push_back(LoadTexture("bgi/frame.png")); //8
	chartex.push_back(LoadTexture("bgi/black.png")); //0
	chartex.push_back(LoadTexture("char/corvus.png")); //1
	chartex.push_back(LoadTexture("char/ariel.png")); //1
	gamtex.push_back(LoadTexture("bgi/black.png"));
	gamtex.push_back(LoadTexture("sprites/corvus.png"));
	gamtex.push_back(LoadTexture("sprites/ariel.png"));
	bultex.push_back(LoadTexture("bullets/bullet.png"));
	stringGraphic texttest(fonttex, "Scott's Final Project", 0.15, -0.06, 1, 1, 1, 1, 1.5, 0.5);
	stringGraphic texttest2(fonttex, "Play", 0.12, -0.05, 0, 1, 1, 1, 1.5, 0.1);
	stringGraphic texttest3(fonttex, "Quit", 0.1, -0.05, 1, 1, 1, 1, 1.5, -0.1);
	texttest.activate();
	texttest2.activate();
	texttest3.activate();
	texttest.beginani(1);
	texts.push_back(texttest);
	texts.push_back(texttest2);
	texts.push_back(texttest3);

	Square s(bgtex[4], -0.25, 0, 0.03, 0.03);
	icons.push_back(s);
	Square s2(bgtex[5], 0.25, 0, 0.03, 0.03);
	icons.push_back(s2);
	Square s3(bgtex[4], -1.2, 0, 0.03, 0.03);
	icons.push_back(s3);

	ifstream infile("chardata.txt");
	string line;
	while (getline(infile, line)){
		charnames.push_back(line);
		getline(infile, line);
		skill1.push_back(line);
		getline(infile, line);
		skill2.push_back(line); 
	}


	for (int i = 3; i < 20; ++i){
		stringGraphic t(1, "text", 0.1, -0.05, 1, 1, 1, 1, 1.5, 0.1);
		texts.push_back(t);
	}
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	music.push_back(Mix_LoadMUS("bgm/Intro.mp3"));
	music.push_back(Mix_LoadMUS("bgm/corvusBGM.mp3"));
	music.push_back(Mix_LoadMUS("bgm/arielBGM.mp3"));
	cmusic = -1;
}
void Game::Update(){
	for (int i = 0; i < texts.size(); i++){
		texts[i].Update();
	}
	if (state == 0){
		if (texts[0].aniframes == 50){
			texts[1].beginani(4);
		}
		if (texts[0].aniframes == 75){
			texts[2].beginani(4);
		}
		if (texts[0].aniframes == 85 && cmusic != 0){
			Mix_PlayMusic(music[0], -1);
			cmusic = 0;
		}
	}
	if (state == 1){
		texts[0].setText(charnames[menu]);
		texts[1].setText(charnames[menu2]);
		texts[2].setText(skill1[menu]);
		texts[3].setText(skill1[menu2]);
		texts[4].setText(skill2[menu]);
		texts[5].setText(skill2[menu2]);
		icons[0].setPos(icons[0].getx(), 0.5 - (0.1 * menu));
		icons[1].setPos(icons[1].getx(), 0.5 - (0.1 * menu2));
		icons[0].incRot(-5);
		icons[1].incRot(5);
	}
	else if (state == 2){
		icons[2].setPos(icons[2].getx(), 0.3 - (0.1 * menu3));
		icons[2].incRot(5);
	}
	else if (state == 3){

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_UP]){
			p1.vinput(true);
		}
		else if (keys[SDL_SCANCODE_DOWN]){
			p1.vinput(false);
		}
		if (keys[SDL_SCANCODE_LEFT]){
			p1.hinput(false);
		}
		else if (keys[SDL_SCANCODE_RIGHT]){
			p1.hinput(true);
		}
		if (keys[SDL_SCANCODE_KP_8]){
			p2.vinput(true);
		}
		else if (keys[SDL_SCANCODE_KP_5]){
			p2.vinput(false);
		}
		if (keys[SDL_SCANCODE_KP_4]){
			p2.hinput(false);
		}
		else if (keys[SDL_SCANCODE_KP_6]){
			p2.hinput(true);
		}

		if (keys[SDL_SCANCODE_Z]){
			p1.Shoot(bultex[0], masterBullets.p1buls, masterBullets.p1buck);
		}

		p1.Update();
		p2.Update();

		for (int i = 0; i < masterBullets.p1buls.size(); i++){
			masterBullets.p1buls[i].Update();
			if (masterBullets.p1buls[i].isactive() && masterBullets.p1buls[i].gety() > 1){
				masterBullets.p1buls[i].deactivate();
				masterBullets.p1buck.push(i);
			}
		}

		if (texts[0].aniframes == 90){
			cmusic = menu3;
			Mix_PlayMusic(music[cmusic], -1);
		}
	}
}
void Game::Render(){
	glClear(GL_COLOR_BUFFER_BIT);
	if (state == 0){
		Square bg(bgtex[0], 0, 0, 1, 1.33);
		bg.Draw();
		for (int i = 0; i < texts.size(); i++){
			texts[i].Draw();
		}
	}
	else if (state == 1){
		Square bg(bgtex[1], 0, 0, 1, 1.33);
		bg.Draw();
		glScissor(0, 0, 400, 600);
		glEnable(GL_SCISSOR_TEST);
		Square c1(chartex[menu], -0.8, 0, 1, 0.55);
		c1.Draw();
		glDisable(GL_SCISSOR_TEST);
		glScissor(400, 0, 400, 600);
		glEnable(GL_SCISSOR_TEST);
		Square c2(chartex[menu2], 0.8, 0, 1, 0.55);
		c2.Draw();
		glDisable(GL_SCISSOR_TEST);
		Square bg2(bgtex[3], 0, 0, 1, 1.33);
		bg2.Draw();
		Square box1(bgtex[2], 0, 0, 1, 0.3);
		box1.Draw();
		for (int i = 0; i < texts.size(); i++){
			texts[i].Draw();
		}
		icons[0].Draw();
		icons[1].Draw();
	}
	else if (state == 2){
		Square bg(bgtex[1], 0, 0, 1, 1.33);
		bg.Draw();
		glScissor(0, 0, 400, 600);
		glEnable(GL_SCISSOR_TEST);
		Square c1(chartex[menu], -0.8, 0, 1, 0.55);
		c1.Draw();
		glDisable(GL_SCISSOR_TEST);
		glScissor(400, 0, 400, 600);
		glEnable(GL_SCISSOR_TEST);
		Square c2(chartex[menu2], 0.8, 0, 1, 0.55);
		c2.Draw();
		glDisable(GL_SCISSOR_TEST);
		Square bg2(bgtex[3], 0, 0, 1, 1.33);
		bg2.Draw();
		Square box1(bgtex[2], 0, 0, 1, 0.3);
		box1.Draw();
		Square bg3(bgtex[2], 0, 0, 0.5, 1.33);
		bg3.Draw();
		for (int i = 0; i < texts.size(); i++){
			texts[i].Draw();
		}
		icons[2].Draw();
		Square picture(bgtex[menu3 + 5], 0.6, 0, 0.4, 0.6);
		picture.Draw();
	}
	else if (state == 3){
		Square bg(bgtex[menu3 + 5], 0, 0, 1, 1.33);

		bg.Draw();
		for (int i = 0; i < texts.size(); i++){
			texts[i].Draw();
		}

		Square bg2(bgtex[8], 0, 0, 1, 1.33);
		bg2.Draw();
		p1.Draw();
		p2.Draw();
		for (int i = 0; i < masterBullets.p1buls.size(); i++){
			masterBullets.p1buls[i].Draw();
		}

	}
	else if (state == 4){
		Square bg(bgtex[menu3 + 5], 0, 0, 1, 1.33);

		bg.Draw();
		
		Square bg2(bgtex[8], 0, 0, 1, 1.33);
		bg2.Draw();
		for (int i = 0; i < texts.size(); i++){
			texts[i].Draw();
		}
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
					}
					else if (state == 1){
						
					}
					else{
						state = 0;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
					if (state == 3 && texts[0].getani() == 0){
						for (int i = 0; i < texts.size(); i++){
							texts[i].deactivate();
						}
						state = 4;
						menu = 1;
						texts[0] = stringGraphic(fonttex, "Go to title?", 0.12, -0.05, 1, 1, 1, 1, -0.4, 0.6);
						texts[1] = stringGraphic(fonttex, "Yes", 0.12, -0.05, 0, 1, 1, 1, -0.2, 0.5);
						texts[2] = stringGraphic(fonttex, "No", 0.1, -0.05, 1, 1, 1, 1, -0.2, 0.4);
						texts[0].activate();
						texts[1].activate();
						texts[2].activate();
					}
					else if (state == 4){
						state = 3;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_DOWN){
					if (state == 0){
						if (texts[menu].getani() == 0){
							texts[menu].beginani(3);
							menu = menu + 1;
							if (menu == 3){
								menu = 1;
							}
							texts[menu].beginani(2);
						}
					}
					if (state == 1){
						menu = menu + 1;
						if (menu == 3){
							menu = 1;
						}
					}
					if (state == 2){
						menu3 = menu3 + 1;
						if (menu3 == 3){
							menu3 = 1;
						}
					}
					if (state == 4){
						if (texts[menu].getani() == 0){
							texts[menu].beginani(3);
							menu = menu + 1;
							if (menu == 3){
								menu = 1;
							}
							texts[menu].beginani(2);
						}
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_UP){
					if (state == 0){
						if (texts[menu].getani() == 0){
							texts[menu].beginani(3);
							menu = menu - 1;
							if (menu == 0){
								menu = 2;
							}
							texts[menu].beginani(2);
						}
					}
					if (state == 1){
						menu = menu - 1;
						if (menu == 0){
							menu = 2;
						}
					}
					if (state == 2){
						menu3 = menu3 - 1;
						if (menu3 == 0){
							menu3 = 2;
						}
					}
					if (state == 4){
						if (texts[menu].getani() == 0){
							texts[menu].beginani(3);
							menu = menu - 1;
							if (menu == 0){
								menu = 2;
							}
							texts[menu3].beginani(2);
						}
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_KP_5){
					if (state == 0){
					}
					else if (state == 1){
						menu2 = menu2 + 1;
						if (menu2 == 3){
							menu2 = 1;
						}
					}
					else{
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_KP_8){
					if (state == 0){
					}
					else if (state == 1){
						menu2 = menu2 - 1;
						if (menu2 == 0){
							menu2 = 2;
						}
					}
					else{
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_Z){
					if (state == 0 && (texts[0].getani() == 0)){
						if (menu == 1){
							for (int i = 0; i < texts.size(); i++){
								texts[i].deactivate();
							}
							state = 1;
							menu2 = 1;
							texts[0] = stringGraphic(fonttex, "Character1", 0.12, -0.05, 1, 0, 0, 1, -1.1, -0.6);
							texts[1] = stringGraphic(fonttex, "Character2", 0.12, -0.05, 0, 0.75, 1, 1, 0.5, -0.6);
							texts[2] = stringGraphic(fonttex, "Skill11", 0.09, -0.05, 1, 0, 0, 1, -1.1, -0.8);
							texts[3] = stringGraphic(fonttex, "Skill12", 0.09, -0.05, 0, 0.75, 1, 1, 0.5, -0.8);
							texts[4] = stringGraphic(fonttex, "Skill21", 0.09, -0.05, 1, 0, 0, 1, -1.1, -0.9);
							texts[5] = stringGraphic(fonttex, "Skill22", 0.09, -0.05, 0, 0.75, 1, 1, 0.5, -0.9);
							texts[6] = stringGraphic(fonttex, "Skills:", 0.09, -0.05, 1, 0, 0, 1, -1.1, -0.7);
							texts[7] = stringGraphic(fonttex, "Skills:", 0.09, -0.05, 0, 0.75, 1, 1, 0.5, -0.7);
							texts[8] = stringGraphic(fonttex, "Character", 0.12, -0.06, 1, 0, 0, 1, -0.25, 0.8);
							texts[9] = stringGraphic(fonttex, "Select", 0.12, -0.05, 0, 0.5, 1, 1, -0.09, 0.7);
							texts[10] = stringGraphic(fonttex, "Corvus", 0.11, -0.05, 1, 1, 1, 1, -0.14, 0.4);
							texts[11] = stringGraphic(fonttex, "Ariel", 0.11, -0.05, 1, 1, 1, 1, -0.11, 0.3);
							for (int i = 0; i <= 11; i++){
								texts[i].activate();
							}
							icons[0].activate();
							icons[1].activate();
						}
						else if (menu == 2){
							done = true;
						}
					}
					else if (state == 1){
						state = 2;
						texts[12] = stringGraphic(fonttex, "Stage Select", 0.12, -0.05, 1, 1, 1, 1, -1.1, 0.4);
						texts[13] = stringGraphic(fonttex, "Passage of Time", 0.11, -0.05, 1, 1, 1, 1, -1.1, 0.2);
						texts[14] = stringGraphic(fonttex, "Clear Blue Skies", 0.11, -0.05, 1, 1, 1, 1, -1.1, 0.1);
						for (int i = 0; i <= 14; i++){
							texts[i].activate();
						}
						icons[0].deactivate();
						icons[1].deactivate();
						icons[2].activate();
						for (int i = 10; i <= 11; i++){
							texts[i].deactivate();
						}
						menu3 = 1;
					}
					else if (state == 2){
						state = 3;
						for (int i = 0; i < texts.size(); i++){
							texts[i].deactivate();
						}
						masterBullets = bCont();
						texts[0] = stringGraphic(fonttex, "Ready?", 0.12, -0.05, 1, 0, 0, 1, -0.8, 0);
						texts[1] = stringGraphic(fonttex, "Ready?", 0.12, -0.05, 0, 0.75, 1, 1, 0.5, 0);
						texts[2] = stringGraphic(fonttex, "Score1", 0.12, -0.05, 1, 1, 1, 1, -1.2, 0.88);
						texts[3] = stringGraphic(fonttex, "Score2", 0.12, -0.05, 1, 1, 1, 1, 0.1, 0.88);
						for (int i = 0; i <= 1; i++){
							texts[i].activate();
							texts[i].beginani(5);
						}
						for (int i = 2; i <= 3; i++){
							texts[i].activate();
						}
						p1 = Player(gamtex[menu], -0.7, -0.3, 1, menu);
						p2 = Player(gamtex[menu2], 0.7, -0.3, 2, menu2);
						p1.setPos(-0.7, 0.3);
						p2.setPos(0.7, 0.3);
						Mix_HaltMusic();
					}
					else if (state == 4){
						if (menu == 1){
							Mix_HaltMusic();
							state = 0;
							for (int i = 0; i < texts.size(); i++){
								texts[i].deactivate();
							}
							texts[0] = stringGraphic(fonttex, "Scott's Final Project", 0.15, -0.06, 1, 1, 1, 1, 1.5, 0.5);
							texts[1] = stringGraphic(fonttex, "Play", 0.12, -0.05, 0, 1, 1, 1, 1.5, 0.1);
							texts[2] = stringGraphic(fonttex, "Quit", 0.1, -0.05, 1, 1, 1, 1, 1.5, -0.1);
							texts[0].activate();
							texts[1].activate();
							texts[2].activate();
							texts[0].beginani(1);
							menu = 1;
						}
						else if (menu == 2){
							state = 3;
							texts[0].deactivate();
							texts[1].deactivate();
							texts[2] = stringGraphic(fonttex, "Score1", 0.12, -0.05, 1, 1, 1, 1, -1.2, 0.88);
							texts[2].activate();
							texts[3].activate();
						}
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_X){
					if (state == 0 && (texts[0].getani() == 0)){
						if (state == 0){
							if (texts[menu].getani() == 0){
								texts[menu].beginani(3);
								menu = 2;
								texts[menu].beginani(2);
							}
						}
					}
					if (state == 1){
						state = 0;
						for (int i = 0; i < texts.size(); i++){
							texts[i].deactivate();
						}
						texts[0] = stringGraphic(fonttex, "Scott's Final Project", 0.15, -0.06, 1, 1, 1, 1, 1.5, 0.5);
						texts[1] = stringGraphic(fonttex, "Play", 0.12, -0.05, 0, 1, 1, 1, 1.5, 0.1);
						texts[2] = stringGraphic(fonttex, "Quit", 0.1, -0.05, 1, 1, 1, 1, 1.5, -0.1);
						texts[0].activate();
						texts[1].activate();
						texts[2].activate();
						texts[0].beginani(1);
						menu = 1;

					}
					if (state == 2){
						state = 1;
						icons[0].activate();
						icons[1].activate();
						icons[2].deactivate();
						for (int i = 10; i <= 11; i++){
							texts[i].activate();
						}
						for (int i = 12; i < texts.size(); i++){
							texts[i].deactivate();
						}
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