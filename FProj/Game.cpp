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
#define PS3_RIGHT SDL_CONTROLLER_BUTTON_DPAD_RIGHT
#define PS3_LEFT SDL_GameControllerButton(12)
#define PS3_DOWN SDL_GameControllerButton(13)
#define PS3_UP SDL_CONTROLLER_BUTTON_DPAD_UP
#define PS3_X SDL_CONTROLLER_BUTTON_A
#define PS3_O SDL_CONTROLLER_BUTTON_B
#define PS3_L1 SDL_GameControllerButton(9)
#define PS3_R1 SDL_GameControllerButton(10)
#define PS3_START SDL_GameControllerButton(6)
#define CHAR_COUNT 6
#define STAGE_COUNT 7

Game::Game()
: done(false),
leftovertime(0),
masterBullets(),
puppeteer(),
bgpos(0, 0, 0),
bgvel(0, 0, 0),
p1c(0, 0, 0, 0, 0, 0),
p2c(0, 0, 0, 0, 0, 0),
apocalypsetimer(0, 0, 0, 0, 0, 0)
{
	Init();
	RUN();

}
Game::~Game(){
	SDL_Quit();
}
void Game::Init(){
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	GLuint VertexArrayID;
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	menu = 1;
	enethresh = 60;
	lasteneframe = 0;
	SDL_GL_MakeCurrent(displayWindow, context);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	srand(time(0));
	perlinValue = 0.0;
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
	bgtex.push_back(LoadTexture("bgi/chargebar.png")); //9
	bgtex.push_back(LoadTexture("bgi/chargeback.png")); //10
	bgtex.push_back(LoadTexture("bgi/chargefill.png")); //11
	bgtex.push_back(LoadTexture("bgi/usefill.png")); //12
	bgtex.push_back(LoadTexture2("bgi/floraStage.jpg"));//13
	bgtex.push_back(LoadTexture("bgi/thorStage.png")); //14
	bgtex.push_back(LoadTexture2("bgi/crystalStage1.jpg")); //15
	bgtex.push_back(LoadTexture2("bgi/kephriStage.jpg"));
	bgtex.push_back(LoadTexture2("bgi/apocalypse.jpg"));
	chartex.push_back(LoadTexture("bgi/black.png")); //0
	chartex.push_back(LoadTexture("char/corvus.png")); //1
	chartex.push_back(LoadTexture("char/ariel.png")); //1
	chartex.push_back(LoadTexture("char/flora.png"));
	chartex.push_back(LoadTexture("char/thor.png"));
	chartex.push_back(LoadTexture("char/StGermain.png"));
	chartex.push_back(LoadTexture("char/kephri.png"));
	gamtex.push_back(LoadTexture("bgi/black.png"));
	gamtex.push_back(LoadTexture("sprites/corvus.png"));
	gamtex.push_back(LoadTexture("sprites/ariel.png"));
	gamtex.push_back(LoadTexture("sprites/flora.png"));
	gamtex.push_back(LoadTexture("sprites/thor.png"));
	gamtex.push_back(LoadTexture("sprites/crystal.png"));
	gamtex.push_back(LoadTexture("sprites/kephri.png"));
	bultex.push_back(LoadTexture("bullets/bullet.png"));
	bultex.push_back(LoadTexture("bullets/corvusbul.png"));
	bultex.push_back(LoadTexture("bullets/arielbul.png"));
	bultex.push_back(LoadTexture("bullets/enebul1.png"));
	bultex.push_back(LoadTexture("bullets/enebul2.png"));
	bultex.push_back(LoadTexture("bullets/florabul.png"));
	bultex.push_back(LoadTexture("bullets/thorbul.png"));
	bultex.push_back(LoadTexture("bullets/crystalbul.png"));
	bultex.push_back(LoadTexture("bullets/kephribul.png"));
	bultex.push_back(LoadTexture("bullets/kephriEX.png"));
	bultex.push_back(LoadTexture("bullets/kephriEX2.png"));
	enetex.push_back(LoadTexture("bgi/black.png"));
	enetex.push_back(LoadTexture("enemies/corvusE1.png"));
	enetex.push_back(LoadTexture("enemies/arielE1.png"));
	enetex.push_back(LoadTexture("enemies/floraE1.png"));
	enetex.push_back(LoadTexture("enemies/thorE1.png"));
	enetex.push_back(LoadTexture("enemies/crystalE1.png"));
	enetex.push_back(LoadTexture("enemies/kephriE1.png"));
	enetex.push_back(LoadTexture("enemies/apocalypseE1.png"));
	enetex.push_back(LoadTexture("enemies/apocalypseE2.png"));
	ftex.push_back(LoadTexture("fields/spcirc.png"));
	ftex.push_back(LoadTexture("fields/corvuspfield.png"));
	ftex.push_back(LoadTexture("fields/arielfield.png"));
	ftex.push_back(LoadTexture("fields/florafield.png"));
	ftex.push_back(LoadTexture("fields/thorfield.png"));
	ftex.push_back(LoadTexture("fields/crystalfield.png"));
	ftex.push_back(LoadTexture("fields/kephrifield.png"));
	stringGraphic texttest(fonttex, "Creative Title", 0.15, -0.06, 1, 1, 1, 1, 1.5, 0.5);
	stringGraphic texttest2(fonttex, "Play", 0.12, -0.05, 0, 1, 1, 1, 1.5, 0.1);
	stringGraphic texttest3(fonttex, "Quit", 0.1, -0.05, 1, 1, 1, 1, 1.5, -0.1);
	texttest.activate();
	texttest2.activate();
	texttest3.activate();
	texttest.beginani(1);
	texts.push_back(texttest);
	texts.push_back(texttest2);
	texts.push_back(texttest3);

	Sphere s(bgtex[4], 0.03, -0.25, 0);
	icons.push_back(s);
	Sphere s2(bgtex[5], 0.03, 0.25, 0);
	icons.push_back(s2);
	Sphere s3(bgtex[4], 0.03, -1.2, 0);
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
	p1c = chargeBar(bgtex[10], bgtex[9], bgtex[11], bgtex[12], -0.67, -0.95);
	p2c = chargeBar(bgtex[10], bgtex[9], bgtex[11], bgtex[12], 0.67, -0.95);
	apocalypsetimer = chargeBar2(bgtex[10], bgtex[9], bgtex[11], bgtex[12], 0, 0.975);
	for (int i = 3; i < 20; ++i){
		stringGraphic t(1, "text", 0.1, -0.05, 1, 1, 1, 1, 1.5, 0.1);
		texts.push_back(t);
	}
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	music.push_back(Mix_LoadMUS("bgm/Intro.mp3"));
	music.push_back(Mix_LoadMUS("bgm/corvusBGM.mp3"));
	music.push_back(Mix_LoadMUS("bgm/arielBGM.wav"));
	music.push_back(Mix_LoadMUS("bgm/floraBGM.wav"));
	music.push_back(Mix_LoadMUS("bgm/thorBGM.wav"));
	music.push_back(Mix_LoadMUS("bgm/crystalBGM.wav"));
	music.push_back(Mix_LoadMUS("bgm/kephriBGM.wav"));
	music.push_back(Mix_LoadMUS("bgm/apocalypseBGM1.wav"));
	music.push_back(Mix_LoadMUS("bgm/apocalypseBGM2.wav"));
	ssound = Mix_LoadWAV("se/sel.wav");
	bsound = Mix_LoadWAV("se/back.wav");
	aesound = Mix_LoadWAV("se/endofapocalypse.wav");
	a2sound = Mix_LoadWAV("se/spellcard.wav");
	dsound = Mix_LoadWAV("se/edam.wav");
	cmusic = -1;
	if (SDL_NumJoysticks() != 0 && SDL_IsGameController(0)){
		controller = SDL_GameControllerOpen(0);
	}
	//SDL_SetWindowFullscreen(displayWindow, SDL_WINDOW_FULLSCREEN);
	SDL_ShowCursor(0);
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
		if (winner == 0){
			gameframes++;
		}
		if (winner != 0){
			for (int i = 0; i < masterBullets.ec1buls.size(); i++){
				if (masterBullets.ec1buls[i].isactive()){
					masterBullets.ec1buls[i].explode();
					masterBullets.ec1buls[i].deactivate();
				}
			}
			for (int i = 0; i < masterBullets.ec2buls.size(); i++){
				if (masterBullets.ec2buls[i].isactive()){
					masterBullets.ec2buls[i].explode();
					masterBullets.ec2buls[i].deactivate();
				}
			}
			for (int i = 0; i < puppeteer.p1ene.size(); i++){
				if (puppeteer.p1ene[i].isactive()){
					puppeteer.p1ene[i].deactivate();
				}
			}
			for (int i = 0; i < puppeteer.p2ene.size(); i++){
				if (puppeteer.p2ene[i].isactive()){
					puppeteer.p2ene[i].deactivate();
				}
			}
		}

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
		if (keys[SDL_SCANCODE_KP_8] || SDL_GameControllerGetButton(controller, PS3_UP)){
			p2.vinput(true);
		}
		else if (keys[SDL_SCANCODE_KP_5] || SDL_GameControllerGetButton(controller, PS3_DOWN)){
			p2.vinput(false);
		}
		if (keys[SDL_SCANCODE_KP_4] || SDL_GameControllerGetButton(controller, PS3_LEFT)){
			p2.hinput(false);
		}
		else if (keys[SDL_SCANCODE_KP_6] || SDL_GameControllerGetButton(controller, PS3_RIGHT)){
			p2.hinput(true);
		}

		if (keys[SDL_SCANCODE_Z]){
			p1.Shoot(bultex, masterBullets.p1buls, masterBullets.p1buck);
		}
		if (keys[SDL_SCANCODE_KP_2] || SDL_GameControllerGetButton(controller, PS3_X)){
			p2.Shoot(bultex, masterBullets.p2buls, masterBullets.p2buck);
		}
		

		p1.Update(p1t, keys[SDL_SCANCODE_LSHIFT], keys[SDL_SCANCODE_X]);
		p2.Update(p2t, SDL_GameControllerGetButton(controller, PS3_R1), SDL_GameControllerGetButton(controller,PS3_L1));
		lasteneframe++;

		if (lasteneframe > enethresh - gameframes / 90 && winner == 0 && menu3 != 7){
			if (puppeteer.p1ene.size() - puppeteer.p1buck.size() < 30){
				if (puppeteer.p1buck.size() == 0){
					float nex = mapValue(rand(), 0, RAND_MAX, -1.1, -0.2);
					float ney = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
					Enemy nene(enetex[enetexind], nex, ney, 0);
					nene.activate();
					puppeteer.p1ene.push_back(nene);
				}
				else{
					float nex = mapValue(rand(), 0, RAND_MAX, -1.1, -0.2);
					float ney = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
					int eneind = puppeteer.p1buck.front();
					puppeteer.p1buck.pop();
					puppeteer.p1ene[eneind].reinit(enetex[enetexind], nex, ney, 0);
					puppeteer.p1ene[eneind].activate();
				}
			}
			if (puppeteer.p2ene.size() - puppeteer.p2buck.size() < 30){
				if (puppeteer.p2buck.size() == 0){
					float nex = mapValue(rand(), 0, RAND_MAX, 0.2, 1.1);
					float ney2 = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
					Enemy nene2(enetex[enetexind], nex, ney2, 0);
					nene2.activate();
					puppeteer.p2ene.push_back(nene2);
				}
				else{
					float nex = mapValue(rand(), 0, RAND_MAX, 0.2, 1.1);
					float ney = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
					int eneind = puppeteer.p2buck.front();
					puppeteer.p2buck.pop();
					puppeteer.p2ene[eneind].reinit(enetex[enetexind], nex, ney, 0);
					puppeteer.p2ene[eneind].activate();
				}
			}

			lasteneframe = 0;
		}
		else if (menu3 == 7 && lasteneframe == 50 && puppeteer.p1ene.size() == 0){
				Mix_PlayChannel(-1, a2sound, 0);
				Enemy hinode1(enetex[7], -0.65, 1, 1);
				Enemy hinode2(enetex[7], 0.65, 1, 1);
				hinode1.activate();
				hinode2.activate();
				puppeteer.p1ene.push_back(hinode1);
				puppeteer.p2ene.push_back(hinode2);
				lasteneframe = 0;
			
		}
		else if (lasteneframe > 60 && winner == 0 && menu3 == 7){
			if (gameframes < 5400){
				int count = gameframes / 1800 + 1;
				for (int i = 0; i < count; i++){
					if (puppeteer.p1ene.size() - puppeteer.p1buck.size() < 30){
						int enetexind = mapValue(rand(), 0, RAND_MAX, 1, 6.9);
						if (puppeteer.p1buck.size() == 0){
							Enemy nene(enetex[enetexind], puppeteer.p1ene[0].getx(), puppeteer.p1ene[0].gety(), 0);
							nene.activate();
							puppeteer.p1ene.push_back(nene);
						}
						else{
							int eneind = puppeteer.p1buck.front();
							puppeteer.p1buck.pop();
							puppeteer.p1ene[eneind].reinit(enetex[enetexind], puppeteer.p1ene[0].getx(), puppeteer.p1ene[0].gety(), 0);
							puppeteer.p1ene[eneind].activate();
						}
					}
				}
				for (int i = 0; i < count; i++){
					if (puppeteer.p2ene.size() - puppeteer.p2buck.size() < 30){
						int enetexind = mapValue(rand(), 0, RAND_MAX, 1, 6.9);
						if (puppeteer.p2buck.size() == 0){
							Enemy nene(enetex[enetexind], puppeteer.p2ene[0].getx(), puppeteer.p2ene[0].gety(), 0);
							nene.activate();
							puppeteer.p2ene.push_back(nene);
						}
						else{
							int eneind = puppeteer.p2buck.front();
							puppeteer.p2buck.pop();
							puppeteer.p2ene[eneind].reinit(enetex[enetexind], puppeteer.p2ene[0].getx(), puppeteer.p2ene[0].gety(), 0);
							puppeteer.p2ene[eneind].activate();
						}
					}
				}

				lasteneframe = 0;
			}
			else if (gameframes > 5650){
				for (int i = 0; i < 3; i++){
					int enetexind = mapValue(rand(), 0, RAND_MAX, 1, 6.9);
					if (puppeteer.p1ene.size() - puppeteer.p1buck.size() < 30){
						if (puppeteer.p1buck.size() == 0){
							float nex = mapValue(rand(), 0, RAND_MAX, -1.1, -0.2);
							float ney = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
							Enemy nene(enetex[enetexind], nex, ney, 0);
							nene.activate();
							puppeteer.p1ene.push_back(nene);
						}
						else{
							float nex = mapValue(rand(), 0, RAND_MAX, -1.1, -0.2);
							float ney = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
							int eneind = puppeteer.p1buck.front();
							puppeteer.p1buck.pop();
							puppeteer.p1ene[eneind].reinit(enetex[enetexind], nex, ney, 0);
							puppeteer.p1ene[eneind].activate();
						}
					}
					if (puppeteer.p2ene.size() - puppeteer.p2buck.size() < 30){
						if (puppeteer.p2buck.size() == 0){
							float nex = mapValue(rand(), 0, RAND_MAX, 0.2, 1.1);
							float ney2 = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
							Enemy nene2(enetex[enetexind], nex, ney2, 0);
							nene2.activate();
							puppeteer.p2ene.push_back(nene2);
						}
						else{
							float nex = mapValue(rand(), 0, RAND_MAX, 0.2, 1.1);
							float ney = mapValue(rand(), 0, RAND_MAX, 0.5, 0.9);
							int eneind = puppeteer.p2buck.front();
							puppeteer.p2buck.pop();
							puppeteer.p2ene[eneind].reinit(enetex[enetexind], nex, ney, 0);
							puppeteer.p2ene[eneind].activate();
						}
					}
				}
				lasteneframe = 0;
			}
		}
		if (menu3 == 7){
			float i = mapValue(gameframes, 0, 5400, 0, 1);
			float j = mapValue(gameframes, 5500, 10900, 0, 1);

			apocalypsetimer.Update(i, j);
		}
		if (gameframes == 5400 && menu3 == 7){
			Mix_PlayChannel(-1, aesound, 0);
			for (int i = 0; i < masterBullets.ec1buls.size(); i++){
				if (masterBullets.ec1buls[i].isactive()){
					masterBullets.ec1buls[i].explode();
					masterBullets.ec1buls[i].deactivate();
					masterBullets.ec1buck.push(i);
				}
			}
			for (int i = 0; i < masterBullets.ec2buls.size(); i++){
				if (masterBullets.ec2buls[i].isactive()){
					masterBullets.ec2buls[i].explode();
					masterBullets.ec2buls[i].deactivate();
					masterBullets.ec2buck.push(i);
				}
			}
			for (int i = 0; i < puppeteer.p1ene.size(); i++){
				if (puppeteer.p1ene[i].isactive()){
					puppeteer.p1ene[i].deactivate();
					puppeteer.p1buck.push(i);
					
				}
			}
			for (int i = 0; i < puppeteer.p2ene.size(); i++){
				if (puppeteer.p2ene[i].isactive()){
					puppeteer.p2ene[i].deactivate();
					puppeteer.p2buck.push(i);
					
				}
			}
			puppeteer.p1buck.pop();
			puppeteer.p2buck.pop();
			Mix_HaltMusic();	
		}
		else if (gameframes == 5500 && menu3 == 7){
			puppeteer = eCont();
			Mix_PlayMusic(music[8], -1);
			Mix_PlayChannel(-1, a2sound, 0);
			Enemy hinode1(enetex[8], -0.65, 1, 2);
			Enemy hinode2(enetex[8], 0.65, 1, 2);
			hinode1.activate();
			hinode2.activate();
			puppeteer.p1ene.push_back(hinode1);
			puppeteer.p2ene.push_back(hinode2);
			lasteneframe = 0;
		}


		masterBullets.Update(p1, p2);
		int a = puppeteer.p1ene.size();
		int b = puppeteer.p2ene.size();
		for (int i = 0; i < a; i++){
			puppeteer.p1ene[i].Update();
			puppeteer.p1ene[i].shoot(masterBullets.ec1buls, masterBullets.es1buls, masterBullets.ec1buck, masterBullets.ec1buck, bultex, Vector(p1.getx(), p1.gety(), 0),p1t);
			int bulindex = puppeteer.p1ene[i].hit(masterBullets.p1buls, p1t);
			if (bulindex != -1){
				if (puppeteer.p1ene[i].getHealth() < 1){
					p1.addScore(puppeteer.p1ene[i].explode(masterBullets.ec1buls, masterBullets.es1buls, masterBullets.ec1buck, masterBullets.es1buck, p1t));
					Mix_PlayChannel(-1, dsound, 0);
					puppeteer.p1buck.push(i);
				}
				if (masterBullets.p1buls[bulindex].getHealth() < 1){
					masterBullets.p1buls[bulindex].deactivate();
					masterBullets.p1buls[bulindex].explode();
					masterBullets.p1buck.push(bulindex);
				}
				if (menu3 == 7 && puppeteer.p1ene[i].getTier() == 1){
					p1.addSpScore2(5);
				}
				if (menu3 == 7 && puppeteer.p1ene[i].getTier() == 2){
					p1.addSpScore2(20);
				}
			}
		}

		for (int i = 0; i < b; i++){
			puppeteer.p2ene[i].Update();
			puppeteer.p2ene[i].shoot(masterBullets.ec2buls, masterBullets.es2buls, masterBullets.ec2buck, masterBullets.ec2buck, bultex, Vector(p2.getx(), p2.gety(), 0), p2t);
			int bulindex = puppeteer.p2ene[i].hit(masterBullets.p2buls, p2t);
			if (bulindex != -1){
				if (puppeteer.p2ene[i].getHealth() < 1){
					p2.addScore(puppeteer.p2ene[i].explode(masterBullets.ec2buls, masterBullets.es2buls, masterBullets.ec2buck, masterBullets.es2buck, p2t));
					Mix_PlayChannel(-1, dsound, 0);
					puppeteer.p2buck.push(i);
				}
				if (masterBullets.p2buls[bulindex].getHealth() < 1){
					masterBullets.p2buls[bulindex].deactivate();
					masterBullets.p2buls[bulindex].explode();
					masterBullets.p2buck.push(bulindex);
				}
				if (menu3 == 7 && puppeteer.p2ene[i].getTier() == 1){
					p2.addSpScore2(5);
				}
				if (menu3 == 7 && puppeteer.p2ene[i].getTier() == 2){
					p2.addSpScore2(20);
				}
			}
		}
		p1t.Update(bultex, masterBullets.ec2buls, masterBullets.ec2buck, ftex, masterBullets.pc2fields, masterBullets.fc2buck);
		p2t.Update(bultex, masterBullets.ec1buls, masterBullets.ec1buck, ftex, masterBullets.pc1fields, masterBullets.fc1buck);
		if (p1.hit(masterBullets.ec1buls, masterBullets.es1buls, masterBullets.ec1buck, masterBullets.es1buck, p1t, masterBullets.pc1fields, masterBullets.fc1buck)){
			int rh = p1.damage(1);
			p1.startinv();
			ostringstream intconv;
			intconv << rh;
			texts[4].setText(intconv.str());
			intconv.flush();
			if (rh == 0){
				winner = 2;
				Mix_PlayChannel(-1, aesound, 0);
				texts[7].beginani(8);
				texts[6].beginani(7);
			}
		}
		if (p2.hit(masterBullets.ec2buls, masterBullets.es2buls, masterBullets.ec2buck, masterBullets.es2buck, p2t, masterBullets.pc2fields, masterBullets.fc2buck)){
			int rh = p2.damage(1);
			p2.startinv();
			ostringstream intconv;
			intconv << rh;
			texts[5].setText(intconv.str());
			intconv.flush();
			if (rh == 0){
				winner = 1;
				Mix_PlayChannel(-1, aesound, 0);
				texts[6].beginani(8);
				texts[7].beginani(7);
			}
		}
		if (texts[0].aniframes == 90){
			cmusic = menu3;
			Mix_PlayMusic(music[cmusic], -1);
		}

		for (int i = 0; i < masterBullets.pc1fields.size(); ++i){
			if (masterBullets.pc1fields[i].Update(masterBullets.fc1buck, i)){
				masterBullets.pc1fields[i].applyEffect(masterBullets.ec1buls, masterBullets.es1buls, masterBullets.ec1buck, masterBullets.es1buck, bultex, Vector(p1.getx(), p1.gety(), 0));
			}
		}
		for (int i = 0; i < masterBullets.pc2fields.size(); ++i){
			if (masterBullets.pc2fields[i].Update(masterBullets.fc2buck, i)){
				masterBullets.pc2fields[i].applyEffect(masterBullets.ec2buls, masterBullets.es2buls, masterBullets.ec2buck, masterBullets.es2buck, bultex, Vector(p2.getx(), p2.gety(), 0));
			}
		}

		ostringstream intconv;
		intconv << p1.getScore();
		texts[2].setText(intconv.str());
		ostringstream intconv2;
		intconv.flush();
		intconv2 << p2.getScore();
		texts[3].setText(intconv2.str());
		intconv2.flush();


		p1c.Update(p1.getfill(), p1.getcharge());
		p2c.Update(p2.getfill(), p2.getcharge());
	}
}
void Game::Render(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glDepthMask(GL_FALSE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDepthMask(GL_TRUE);
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
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		icons[0].RenderCube();
		icons[1].RenderCube();
		glDisable(GL_DEPTH_TEST);
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
		
		int stageind;
		if (menu3 == 1){
			stageind = 6;
		}
		else if (menu3 == 2){
			stageind = 7;
		}
		else if (menu3 == 3){
			stageind = 13;
		}
		else if (menu3 == 4){
			stageind = 14;
		}
		else if (menu3 == 5){
			stageind = 15;
		}
		else if (menu3 == 6){
			stageind = 16;
		}
		else if (menu3 == 7){
			stageind = 17;
		}
		Square picture(bgtex[stageind], 0.6, 0, 0.4, 0.6);
		picture.Draw();
		enetexind = menu3;
		bultexind = menu3;
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		icons[2].RenderCube();
		glDisable(GL_DEPTH_TEST);
	}
	else if (state == 3){
		perlinValue += FIXED_TIMESTEP / 300;
		
		float coord[2] = { perlinValue, 0.0 };
		bgpos.x = noise2(coord);
		coord[1] = 0.5f;
		bgpos.y = noise2(coord);

		
		int stageind;
		if (menu3 == 1){
			stageind = 6;
		}
		else if (menu3 == 2){
			stageind = 7;
		}
		else if (menu3 == 3){
			stageind = 13;
		}
		else if (menu3 == 4){
			stageind = 14;
		}
		else if (menu3 == 5){
			stageind = 15;
		}
		else if (menu3 == 6){
			stageind = 16;
		}
		else if (menu3 == 7){
			stageind = 17;
		}
		Square bg(bgtex[stageind], bgpos.x, bgpos.y, 1.5, 2);
		bg.Draw();
		
		

		
		
		glPushMatrix();
		glScissor(0, 0, 400, 600);
		glEnable(GL_SCISSOR_TEST);
		for (int i = 0; i < masterBullets.pc1fields.size(); i++){
			if (masterBullets.pc1fields[i].getEff() != 6){
				masterBullets.pc1fields[i].Render();
			}
		}
		p1.Render();
		for (int i = 0; i < masterBullets.p1buls.size(); i++){
			masterBullets.p1buls[i].Render();
		}
		for (int i = 0; i < masterBullets.ec1buls.size(); i++){
			masterBullets.ec1buls[i].Render();
		}

		for (int i = 0; i < masterBullets.es1buls.size(); i++){
			masterBullets.es1buls[i].Render();
		}
		
		for (int i = 0; i < puppeteer.p1ene.size(); i++){
			puppeteer.p1ene[i].Render();
		}
		for (int i = 0; i < masterBullets.pc1fields.size(); i++){
			if (masterBullets.pc1fields[i].getEff() == 6){
				masterBullets.pc1fields[i].Render();
			}
		}
		glDisable(GL_SCISSOR_TEST);
		glPopMatrix();

		glPushMatrix();
		glScissor(400, 0, 400, 600);
		glEnable(GL_SCISSOR_TEST);
		for (int i = 0; i < masterBullets.pc2fields.size(); i++){
			if (masterBullets.pc2fields[i].getEff() != 6){
				masterBullets.pc2fields[i].Render();
			}
		}
		p2.Render();
		for (int i = 0; i < masterBullets.es2buls.size(); i++){
			masterBullets.es2buls[i].Render();
		}
		for (int i = 0; i < masterBullets.ec2buls.size(); i++){
			masterBullets.ec2buls[i].Render();
		}
		for (int i = 0; i < masterBullets.p2buls.size(); i++){
			masterBullets.p2buls[i].Render();
		}
		for (int i = 0; i < puppeteer.p2ene.size(); i++){
			puppeteer.p2ene[i].Render();
		}
		for (int i = 0; i < masterBullets.pc2fields.size(); i++){
			if (masterBullets.pc2fields[i].getEff() == 6){
				masterBullets.pc2fields[i].Render();
			}
		}
		glDisable(GL_SCISSOR_TEST);
		glPopMatrix();
		
		glPushMatrix();
		for (int i = 0; i < texts.size(); i++){
			texts[i].Draw();
		}


		Square bg2(bgtex[8], 0, 0, 1, 1.33);
		bg2.Draw();
		

		p1t.Render();
		p2t.Render();

		p1c.Render();
		p2c.Render();
		if (menu3 == 7){
			apocalypsetimer.Render();
		}
		glPopMatrix();
	}
	else if (state == 4){
		int stageind;
		if (menu3 == 1){
			stageind = 6;
		}
		else if (menu3 == 2){
			stageind = 7;
		}
		else if (menu3 == 3){
			stageind = 13;
		}
		else if (menu3 == 4){
			stageind = 14;
		}
		else if (menu3 == 5){
			stageind = 15;
		}
		else if (menu3 == 6){
			stageind = 16;
		}
		else if (menu3 == 7){
			stageind = 17;
		}
		Square bg(bgtex[stageind], 0, 0, 1, 1.33);

		bg.Draw();
		
		Square bg2(bgtex[8], 0, 0, 1, 1.33);
		bg2.Draw();
		for (int i = 0; i < texts.size(); i++){
			texts[i].Draw();
		}
	}

	


	SDL_GL_SwapWindow(displayWindow);
}

void Game::handleDownKey(){
	if (state == 0){
		if (texts[menu].getani() == 0){
			Mix_PlayChannel(-1, ssound, 0);
			texts[menu].beginani(3);
			menu = menu + 1;
			if (menu == 3){
				menu = 1;
			}
			texts[menu].beginani(2);
		}
	}
	if (state == 1){
		Mix_PlayChannel(-1, ssound, 0);
		menu = menu + 1;
		if (menu == CHAR_COUNT + 1){
			menu = 1;
		}
	}
	if (state == 2){
		Mix_PlayChannel(-1, ssound, 0);
		menu3 = menu3 + 1;
		if (menu3 == STAGE_COUNT + 1){
			menu3 = 1;
		}
	}
	if (state == 4){
		if (texts[menu].getani() == 0){
			Mix_PlayChannel(-1, ssound, 0);
			texts[menu].beginani(3);
			menu = menu + 1;
			if (menu == 3){
				menu = 1;
			}
			texts[menu].beginani(2);
		}
	}
}
void Game::handleUpKey(){
	if (state == 0){
		if (texts[menu].getani() == 0){
			Mix_PlayChannel(-1, ssound, 0);
			texts[menu].beginani(3);
			menu = menu - 1;
			if (menu == 0){
				menu = 2;
			}
			texts[menu].beginani(2);
		}
	}
	if (state == 1){
		Mix_PlayChannel(-1, ssound, 0);
		menu = menu - 1;
		if (menu == 0){
			menu = CHAR_COUNT;
		}
	}
	if (state == 2){
		Mix_PlayChannel(-1, ssound, 0);
		menu3 = menu3 - 1;
		if (menu3 == 0){
			menu3 = STAGE_COUNT;
		}
	}
	if (state == 4){
		if (texts[menu].getani() == 0){
			Mix_PlayChannel(-1, ssound, 0);
			texts[menu].beginani(3);
			menu = menu - 1;
			if (menu == 0){
				menu = 2;
			}
			texts[menu].beginani(2);
		}
	}
}
void Game::handleSelect(){
	if (state == 0 && (texts[0].getani() == 0)){
		Mix_PlayChannel(-1, ssound, 0);
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
			texts[12] = stringGraphic(fonttex, "Flora", 0.11, -0.05, 1, 1, 1, 1, -0.11, 0.2);
			texts[13] = stringGraphic(fonttex, "Thor", 0.11, -0.05, 1, 1, 1, 1, -0.11, 0.1);
			texts[14] = stringGraphic(fonttex, "Crystal", 0.11, -0.05, 1, 1, 1, 1, -0.165, 0.0);
			texts[15] = stringGraphic(fonttex, "Kephri", 0.11, -0.05, 1, 1, 1, 1, -0.14, -0.1);
			for (int i = 0; i <= 15; i++){
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
		Mix_PlayChannel(-1, ssound, 0);
		state = 2;
		texts[12] = stringGraphic(fonttex, "Stage Select", 0.12, -0.05, 1, 1, 1, 1, -1.1, 0.4);
		texts[13] = stringGraphic(fonttex, "Grandfather Clock", 0.11, -0.05, 1, 1, 1, 1, -1.1, 0.2);
		texts[14] = stringGraphic(fonttex, "Sky Stratum", 0.11, -0.05, 1, 1, 1, 1, -1.1, 0.1);
		texts[15] = stringGraphic(fonttex, "Botanical Garden", 0.11, -0.05, 1, 1, 1, 1, -1.1, 0.0);
		texts[16] = stringGraphic(fonttex, "Niflheim", 0.11, -0.05, 1, 1, 1, 1, -1.1, -0.1);
		texts[17] = stringGraphic(fonttex, "City of Lights", 0.11, -0.05, 1, 1, 1, 1, -1.1, -0.2);
		texts[18] = stringGraphic(fonttex, "Schwarzwald", 0.11, -0.05, 1, 1, 1, 1, -1.1, -0.3);
		texts[19] = stringGraphic(fonttex, "Apocalypse", 0.11, -0.05, 1, 0, 1, 1, -1.1, -0.4);
		for (int i = 0; i <= 19; i++){
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
		Mix_PlayChannel(-1, ssound, 0);
		state = 3;
		for (int i = 0; i < texts.size(); i++){
			texts[i].deactivate();
		}
		masterBullets = bCont();
		puppeteer = eCont();
		bgpos = Vector(0, 0, 0);
		bgvel = Vector(0, 0, 0);
		p1t = PTransit();
		p2t = PTransit();
		gameframes = 0;
		winner = 0;
		p1 = Player(gamtex[menu], -0.7, -0.7, 1, menu, ftex);
		p2 = Player(gamtex[menu2], 0.7, -0.7, 2, menu2, ftex);
		string i("Ready?");
		string health("5");
		if (menu3 == 7){
			i = "WARNING!";
			p1.damage(-15);
			p2.damage(-15);
			health = "20";
		}
		texts[0] = stringGraphic(fonttex, i, 0.12, -0.05, 1, 0, 0, 1, -0.8, 0);
		texts[1] = stringGraphic(fonttex, i, 0.12, -0.05, 0, 0.75, 1, 1, 0.5, 0);
		texts[2] = stringGraphic(fonttex, "Score1", 0.12, -0.05, 1, 1, 1, 1, -1.2, 0.88);
		texts[3] = stringGraphic(fonttex, "Score2", 0.12, -0.05, 1, 1, 1, 1, 0.15, 0.88);
		texts[4] = stringGraphic(fonttex, health, 0.12, -0.05, 1, 1, 1, 1, -0.2, 0.88);
		texts[5] = stringGraphic(fonttex, health, 0.12, -0.05, 1, 1, 1, 1, 1.15, 0.88);
		texts[6] = stringGraphic(fonttex, "Winner!", 0.12, -0.05, 1, 1, 1, 1, 5, 5);
		texts[7] = stringGraphic(fonttex, "Loser!", 0.12, -0.05, 1, 1, 1, 1, 5, 5);
		for (int i = 0; i <= 7; i++){
			texts[i].activate();
		}
		texts[0].beginani(5);
		texts[1].beginani(6);
		
		lasteneframe = 0;
		p1.setPos(-0.7, -0.7);
		p2.setPos(0.7, -0.7);
		Mix_HaltMusic();
	}
	else if (state == 3 && winner != 0 && texts[6].aniframes > 250){
		Mix_PlayChannel(-1, ssound, 0);
		Mix_HaltMusic();
		state = 0;
		for (int i = 0; i < texts.size(); i++){
			texts[i].deactivate();
		}
		texts[0] = stringGraphic(fonttex, "Creative Title", 0.15, -0.06, 1, 1, 1, 1, 1.5, 0.5);
		texts[1] = stringGraphic(fonttex, "Play", 0.12, -0.05, 0, 1, 1, 1, 1.5, 0.1);
		texts[2] = stringGraphic(fonttex, "Quit", 0.1, -0.05, 1, 1, 1, 1, 1.5, -0.1);
		texts[0].activate();
		texts[1].activate();
		texts[2].activate();
		texts[0].beginani(1);
		menu = 1;
	}
	else if (state == 4){
		Mix_PlayChannel(-1, ssound, 0);
		if (menu == 1){
			Mix_HaltMusic();
			state = 0;
			for (int i = 0; i < texts.size(); i++){
				texts[i].deactivate();
			}
			texts[0] = stringGraphic(fonttex, "Creative Title", 0.15, -0.06, 1, 1, 1, 1, 1.5, 0.5);
			texts[1] = stringGraphic(fonttex, "Play", 0.12, -0.05, 0, 1, 1, 1, 1.5, 0.1);
			texts[2] = stringGraphic(fonttex, "Quit", 0.1, -0.05, 1, 1, 1, 1, 1.5, -0.1);
			texts[0].activate();
			texts[1].activate();
			texts[2].activate();
			texts[0].beginani(1);
			menu = 1;
		}
		else if (menu == 2){
			Mix_PlayChannel(-1, ssound, 0);
			state = 3;
			texts[0].deactivate();
			texts[1].deactivate();
			texts[2] = stringGraphic(fonttex, "Score1", 0.12, -0.05, 1, 1, 1, 1, -1.2, 0.88);
			texts[2].activate();
			texts[3].activate();
			texts[4].activate();
			texts[5].activate();
		}
	}
}
void Game::handleBack(){
	if (state == 0 && (texts[0].getani() == 0)){
		if (state == 0){
			if (texts[menu].getani() == 0){
				Mix_PlayChannel(-1, ssound, 0);
				texts[menu].beginani(3);
				menu = 2;
				texts[menu].beginani(2);
			}
		}
	}
	if (state == 1){
		Mix_PlayChannel(-1, ssound, 0);
		state = 0;
		for (int i = 0; i < texts.size(); i++){
			texts[i].deactivate();
		}
		texts[0] = stringGraphic(fonttex, "Creative Title", 0.15, -0.06, 1, 1, 1, 1, 1.5, 0.5);
		texts[1] = stringGraphic(fonttex, "Play", 0.12, -0.05, 0, 1, 1, 1, 1.5, 0.1);
		texts[2] = stringGraphic(fonttex, "Quit", 0.1, -0.05, 1, 1, 1, 1, 1.5, -0.1);
		texts[0].activate();
		texts[1].activate();
		texts[2].activate();
		texts[0].beginani(1);
		menu = 1;

	}
	if (state == 2){
		Mix_PlayChannel(-1, ssound, 0);
		state = 1;
		icons[0].activate();
		icons[1].activate();
		icons[2].deactivate();
		texts[12] = stringGraphic(fonttex, "Flora", 0.11, -0.05, 1, 1, 1, 1, -0.11, 0.2);
		texts[13] = stringGraphic(fonttex, "Thor", 0.11, -0.05, 1, 1, 1, 1, -0.11, 0.1);
		texts[14] = stringGraphic(fonttex, "Crystal", 0.11, -0.05, 1, 1, 1, 1, -0.165, 0.0);
		texts[15] = stringGraphic(fonttex, "Kephri", 0.11, -0.05, 1, 1, 1, 1, -0.14, -0.1);
		for (int i = 10; i <= 15; i++){
			texts[i].activate();
		}
		for (int i = 16; i < texts.size(); i++){
			texts[i].deactivate();
		}
	}
	if (state == 4){
		Mix_PlayChannel(-1, ssound, 0);
		state = 3;
		texts[0].deactivate();
		texts[1].deactivate();
		texts[2] = stringGraphic(fonttex, "Score1", 0.12, -0.05, 1, 1, 1, 1, -1.15, 0.88);
		texts[2].activate();
		texts[3].activate();
		texts[4].activate();
		texts[5].activate();
	}
}
void Game::handleDownBut(){
	if (state == 0){
		if (texts[menu].getani() == 0){
			Mix_PlayChannel(-1, ssound, 0);
			texts[menu].beginani(3);
			menu = menu + 1;
			if (menu == 3){
				menu = 1;
			}
			texts[menu].beginani(2);
		}
	}
	else if (state == 1){
		Mix_PlayChannel(-1, ssound, 0);
		menu2 = menu2 + 1;
		if (menu2 == CHAR_COUNT + 1){
			menu2 = 1;
		}
	}
	if (state == 2){
		Mix_PlayChannel(-1, ssound, 0);
		menu3 = menu3 + 1;
		if (menu3 == STAGE_COUNT + 1){
			menu3 = 1;
		}
	}
	if (state == 4){
		if (texts[menu].getani() == 0){
			Mix_PlayChannel(-1, ssound, 0);
			texts[menu].beginani(3);
			menu = menu + 1;
			if (menu == 3){
				menu = 1;
			}
			texts[menu].beginani(2);
		}
	}
}
void Game::handleUpBut(){
	if (state == 0){
		if (texts[menu].getani() == 0){
			Mix_PlayChannel(-1, ssound, 0);
			texts[menu].beginani(3);
			menu = menu - 1;
			if (menu == 0){
				menu = 2;
			}
			texts[menu].beginani(2);
		}
	}
	else if (state == 1){
		Mix_PlayChannel(-1, ssound, 0);
		menu2 = menu2 - 1;
		if (menu2 == 0){
			menu2 = CHAR_COUNT;
		}
	}
	if (state == 2){
		Mix_PlayChannel(-1, ssound, 0);
		menu3 = menu3 - 1;
		if (menu3 == 0){
			menu3 = STAGE_COUNT;
		}
	}
	if (state == 4){
		if (texts[menu].getani() == 0){
			Mix_PlayChannel(-1, ssound, 0);
			texts[menu].beginani(3);
			menu = menu - 1;
			if (menu == 0){
				menu = 2;
			}
			texts[menu].beginani(2);
		}
	}
}
void Game::handlePause(){
	if (state == 3 && texts[0].getani() == 0){
		Mix_PlayChannel(-1, ssound, 0);
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
		Mix_PlayChannel(-1, ssound, 0);
		state = 3;
		texts[0].deactivate();
		texts[1].deactivate();
		texts[2] = stringGraphic(fonttex, "Score1", 0.12, -0.05, 1, 1, 1, 1, -1.2, 0.88);
		texts[2].activate();
		texts[3].activate();
		texts[4].activate();
		texts[5].activate();
	}
}


void Game::RUN(){
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			if (event.type == SDL_CONTROLLERBUTTONDOWN){
				if (event.cbutton.button == PS3_DOWN){
					handleDownBut();
				}
				if (event.cbutton.button == PS3_UP){
					handleUpBut();
				}
				if(event.cbutton.button ==PS3_X){
					handleSelect();
				}
				if (event.cbutton.button == PS3_O){
					handleBack();
				}
				if (event.cbutton.button == PS3_START){
					handlePause();
				}
				else{
					cout << "Else";
				}
			}
			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
					handlePause();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_DOWN){
					handleDownKey();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_UP){
					handleUpKey();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_KP_5){
					handleDownBut();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_KP_8){
					handleUpBut();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_Z){
					handleSelect();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_X){
					handleBack();
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