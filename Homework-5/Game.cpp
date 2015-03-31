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
#include <iostream>
#include <fstream>
#include <SDL_mixer.h>
using namespace std;

#define FIXED_TIMESTEP 0.033333f
#define MAX_TIMESTEPS 6


bool Game::readHeader(std::ifstream &stream) {
	string line;
	mapWidth = -
		1;
	mapHeight = -
		1;
	while
		(getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height"){
			mapHeight = atoi(value.c_str());
		}
	}
	if
		(mapWidth == -
		1 || mapHeight == -
		1) {
		return false;
	}
	else { // allocate our map data
		levelData = new unsigned int*[mapHeight];
		for
			(int i =
			0; i < mapHeight; ++i) {
			levelData[i] = new unsigned int[mapWidth];
		}
		return true;
	}
}
bool Game::readLayerData(std::ifstream &stream) {
	string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				istringstream lineStream(line);
				string tile;
				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, tile, ',');
					unsigned int val = (unsigned int)atoi(tile.c_str());
					if (val > 0) {
						// be careful, the tiles in this format are indexed from 1 not 0
						levelData[y][x] = val - 1;
					}
					else {
						levelData[y][x] = 0;
					}
				}
			}
		}
	}
	return true;
}
void Game::placeEntity(string type, int x, int y){
	if (type == "Player"){
		Player p(playtex, x , y );
		player = p;
		
	}
}
bool Game::readEntityData(std::ifstream &stream) {
	string line;
	string type;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "type") {
			type = value;
		}
		else if (key == "location") {
			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');
				float placeX = atoi(xPosition.c_str()) / 16 * TILE_SIZE;
			float placeY = atoi(yPosition.c_str()) / 16 * -TILE_SIZE;
			placeEntity(type, placeX, placeY);
		}
	}
	return true;
}
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
TILE_SIZE(16),
player(){
	Init();
	RUN();

}
Game::~Game(){
	Mix_FreeMusic(music);
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
	flotex = LoadTexture("dirt-tiles.png");

	ifstream infile("hw5map.txt");
	string line;
	while (getline(infile, line)) {
		if (line == "[header]") {
			if (!readHeader(infile)) {
				return;
			}
		}
		else if (line == "[layer]") {
			readLayerData(infile);
		}
		else if (line == "[POLayer]") {
			readEntityData(infile);
		}
	}
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	//The music is from an RPG called Devil of Decline (good game, pretty decent soundtrack)
	music = Mix_LoadMUS("DoDWBGM.mp3");

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
		player.Update(levelData);

		for (int i = 0; i < pbullets.size(); ++i){
			pbullets[i].Update(levelData);
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
		//I chose to do the scrolling a slightly different way.
		//Instead of having a single translate, I sent in the "camera position" to the object render functions, and had them scroll themselves.
		//I just kind of like it that way.  
		//When I did it the other way, it just chose to not render every second bullet, and glRotatef wreaked havoc on everything
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		DrawScrollingLevel(flotex, levelData, 24, 16, 0.075, mapHeight, mapWidth, -player.getx(), -player.gety());
		
		
		player.Render();
		for (int i = 0; i < enemies.size(); ++i){
			enemies[i].Render();
		}
		for (int i = 0; i < pbullets.size(); ++i){
			pbullets[i].Render(player.getx(), player.gety());
		}
		glPopMatrix();
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
						player.setPos(-4, 0);
						player.activate();
						score = 0;
						lastroundframes = 1;
						kcount = 0;
						Mix_PlayMusic(music, -1);
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
