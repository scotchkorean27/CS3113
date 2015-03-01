//Scott Lee
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <SDL_surface.h>
#include <vector>
#include <ctime>
#include <sstream>
#include <queue>
#include <set>

using namespace std;

SDL_Window* displayWindow;

enum State {TITLE, GAME, OVER};
enum Direction {LEFT, RIGHT};
int bbulind = 0;
int state;
int dir;
bool bounce;




void DrawStr(int fontTexture, string text, float size, float spacing, float r, float g, float b, float a, float x, float y) {

	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	

	float texture_size = 1.0 / 16.0f;

	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colorData;

	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		colorData.insert(colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });

		vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) +
			(-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f
			* size });

		texCoordData.insert(texCoordData.end(), { texture_x, texture_y, texture_x, texture_y + texture_size, texture_x +
			texture_size, texture_y + texture_size, texture_x + texture_size, texture_y });
	}
	glColorPointer(4, GL_FLOAT, 0, colorData.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, text.size() * 4);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);
	return textureID;
}

void DrawSprite(GLint texture, int index, int SCX, int SCY,
	float x, float y, float rotation, float width, float height) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -width, height, -width, -height, width, -height, width, height };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	float u = (float)(((int)index) % SCX) / (float)SCX;
	float v = (float)(((int)index) / SCY) / (float)SCY;
	float spriteWidth = 1.0 / (float)SCX;
	float spriteHeight = 1.0 / (float)SCY;
	GLfloat quadUVs[] = { u, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight,
		u + spriteWidth, v
	};
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}



class Bullet{
public:
	Bullet(int texture, float xpos, float ypos, bool direction){
		visible = true;
		x = xpos;
		y = ypos;
		dir = direction;
		tex = texture;
		angle = 0;
	};
	void Draw(float elapsed){
		angle += elapsed * 20;
		DrawSprite(tex, 0, 1, 1, x, y, angle, 0.0375, 0.0375);
	};
	void Update(float elapsed){
		if (dir){
			y += elapsed * 0.7;
		}
		else{
			y -= elapsed * 0.7;
		}
	};
	bool isVis(){
		return visible;
	};
	bool toggleVis(){
		visible = !visible;
		return visible;
	};
	void repos(float xpos, float ypos, bool direction){
		x = xpos;
		y = ypos;
		dir = direction;
		visible = true;
	};
	GLfloat lx(){
		return x - 0.01875;
	};
	GLfloat rx(){
		return x + 0.01875;
	};
	GLfloat uy(){
		return y + 0.01875;
	};
	GLfloat dy(){
		return y - 0.01875;
	};
	void setPos(float xp, float yp){
		x = xp;
		y = yp;
	};
private:
	float x;
	float y;
	bool dir;
	bool visible;
	float angle;
	int tex;
};

vector<Bullet> bbullet;
vector<Bullet> gbullet;

class Entity{
public:
	Entity(bool play, int texture, float xpos, float ypos){
		if (play){
			Ani = { 27, 28, 29 };
		}
		else{
			Ani = { 36, 37, 38 };
		}
		x = xpos;
		y = ypos;
		tex = texture;
		isPlayer = play;
		index = Ani[0];
		anilapsed = 0;
		fps = 30;
		visible = true;
		gbulind = 0;
	};
	void Draw(float elapsed){
		anilapsed += elapsed;

		if (anilapsed > 10.0 / fps) {
			index++;
			anilapsed = 0.0;
			if (index > 2) {
				index = 0;
			}
		}

		DrawSprite(tex, Ani[index], 12, 8, x, y, 0, 0.075, 0.075);

	};
	void setPos(float xp, float yp){
		x = xp;
		y = yp;
	};
	bool isVis(){
		return visible;
	};
	bool toggleVis(){
		visible = !visible;
		return visible;
	};
	void Update(float elapsed, bool right, int alive){
		if (isPlayer && right && x + 0.0375 < 1){
			x += elapsed * 0.4;
		}
		else if (isPlayer && !right && x - 0.0375 > -1){
			x -= elapsed * 0.4;
		}
		else if (!isPlayer){
			if (x + 0.0375 > 1){
				dir = LEFT;
				bounce = true;
			}
			else if (x - 0.0375 < -1){
				dir = RIGHT;
				bounce = true;
			}
			if (dir == LEFT){
				x -= elapsed * 0.1 * (1 + 40/alive);
			}
			else if (dir == RIGHT){
				x += elapsed * 0.1 * (1 + 40 / alive);
			}
		}
	};
	void moveDown(){
		y -= 0.03;
		if (y < -0.8 && !isPlayer){
			state = OVER;
		}
	};
	void pshoot(int btex){
		if (gbullet.size() <= 10){
			gbullet.push_back(Bullet(btex, x, y, isPlayer));
		}
		else{
			gbullet[gbulind].repos(x, y, isPlayer);
			gbulind = (gbulind + 1) % 10;
		}
	};
	void eshoot(int btex){
		if (bbullet.size() <= 50){
			bbullet.push_back(Bullet(btex, x, y, isPlayer));
		}
		else{
			bbullet[bbulind].repos(x, y, isPlayer);
			if (!bbullet[bbulind].isVis()){
				bbullet[bbulind].toggleVis();
			}
			bbulind = (bbulind + 1) % 50;
		}
	};
	GLfloat lx(){
		return x - 0.0375;
	};
	GLfloat rx(){
		return x + 0.0375;
	};
	GLfloat uy(){
		return y + 0.0375;
	};
	GLfloat dy(){
		return y - 0.0375;
	};
	bool intersects(Bullet b){
		return (dy() < b.uy() && uy() > b.dy() && lx() < b.rx() && rx() > b.lx());
	};
	bool intersects(Entity b){
		return (dy() < b.uy() && uy() > b.dy() && lx() < b.rx() && rx() > b.lx());
	};
	bool checked(){
		for (int i = 0; i < gbullet.size(); ++i){
			if (gbullet[i].isVis() && intersects(gbullet[i])){
				gbullet[i].toggleVis();
				toggleVis();
				return true;
			}
		}
		return false;
	};
	bool checkpd(){
		for (int i = 0; i < bbullet.size(); ++i){
			if (bbullet[i].isVis() && intersects(bbullet[i])){
				return true;
			}
		}
		return false;
	};
private:
	vector<int> Ani;
	bool isPlayer;
	float x;
	float y;
	int tex;
	int index;
	float anilapsed;
	float fps;
	bool visible;
	int gbulind;
};

vector<Entity> enemies;


//I have no idea why, but if you lose by bullet collision, youll automatically lose the next time you play, but not the time after that.  Just hit space a few times.
int main(int argc, char *argv[])
{
	bool win = false;
	set<int> aliveE;
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	glViewport(0, 0, 800, 800);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1, 1, -1, 1, -1, 1);
	SDL_GL_MakeCurrent(displayWindow, context);
	bool done = false;
	float lastFrameTicks = 0.0f;
	SDL_Event event;
	int score(0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	dir = LEFT;
	state = TITLE;
	srand(time(0));

	GLint fonttex = LoadTexture("font1.png");
	GLint playtex = LoadTexture("characters_1.png");
	GLint bultex = LoadTexture("bullet.png");

	Entity player(true, playtex, 0, -0.9f);
	
	for (int i = 0; i < 40; ++i){
		aliveE.insert(i);
		Entity ene(false, playtex, (i % 8) * 0.2 - 0.5, ((int) i / 8) * 0.125 + 0.35);
		enemies.push_back(ene);
		
	}
	float lastfire = 0.0f;
	float lastefire = 0.0f;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			else if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
					if (state == TITLE){
						state = GAME;
						score = 0;
						win = false;
						for (int i = 0; i < enemies.size(); ++i){
							if (!enemies[i].isVis()){
								enemies[i].toggleVis();
								aliveE.insert(i);
							}
							enemies[i].setPos((i % 8) * 0.2 - 0.5, ((int)i / 8) * 0.125 + 0.35);
						}
					}
					else if (state == GAME){
						float cfire = (float)SDL_GetTicks() / 1000.0f;
						if (cfire - lastfire > 0.5){
							player.pshoot(bultex);
							lastfire = (float)SDL_GetTicks() / 1000.0f;
						}
					}
					else{
						state = TITLE;
					}
				}
			}
		}
		if (state == TITLE){
			glClear(GL_COLOR_BUFFER_BIT);
			DrawStr(fonttex, "This my homework.", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.2);
			DrawStr(fonttex, "I don't know what it is either.", 0.1, -0.06, 1, 1, 1, 1, -0.55, 0.1);
			DrawStr(fonttex, "Press SPACE to start.", 0.1, -0.06, 1, 1, 1, 1, -0.45, 0);
		}
		if (state == GAME){
			const Uint8 *keys = SDL_GetKeyboardState(NULL);
			glClear(GL_COLOR_BUFFER_BIT);

			float ticks = (float)SDL_GetTicks() / 1000.0f;
			float elapsed = ticks - lastFrameTicks;
			lastFrameTicks = ticks;

			if (keys[SDL_SCANCODE_LEFT]){
				player.Update(elapsed, false, aliveE.size());
			}
			else if (keys[SDL_SCANCODE_RIGHT]){
				player.Update(elapsed, true, aliveE.size());
			}

			player.Draw(elapsed);
			for (int i = 0; i < enemies.size(); ++i){
				if (enemies[i].isVis()){
					enemies[i].Draw(elapsed);
				}
			}
			bounce = false;
			for (int i = 0; i < enemies.size(); ++i){
				if (enemies[i].isVis()){
					enemies[i].Update(elapsed, false, aliveE.size());
				}
			}
			if (bounce){
				for (int i = 0; i < enemies.size(); ++i){
					if (enemies[i].isVis()){
						enemies[i].moveDown();
					}
				}
			}
			for (int i = 0; i < gbullet.size(); ++i){
				if (gbullet[i].isVis()){
					gbullet[i].Draw(elapsed);
					gbullet[i].Update(elapsed);
				}
			}
			for (int i = 0; i < bbullet.size(); ++i){
				if (bbullet[i].isVis()){
					bbullet[i].Draw(elapsed);
					bbullet[i].Update(elapsed);
				}
			}
			float cefire = (float)SDL_GetTicks() / 1000.0f;
			if (cefire - lastefire > 1.25 - (41 - aliveE.size())/40 ){
				set<int>::iterator aitr(aliveE.begin());
				advance(aitr, rand() % aliveE.size());
				enemies[*aitr].eshoot(bultex);
				lastefire = cefire;
			}
			for (int i = 0; i < enemies.size(); ++i){
				if (aliveE.count(i) > 0 && enemies[i].checked()){
					aliveE.erase(i);
					score += 100;
				}
				if (aliveE.size() == 0){
					win = true;
					score += 2000;
					state = OVER;
				}
			}
			if (player.checkpd()){
				for (int i = 0; i < bbullet.size(); i++){
					bbullet[i].setPos(-1, -1);
				}
				player.setPos(0, -0.9f);
				win = false;
				state = OVER;
			}

			ostringstream intconv;
			intconv << score;
			DrawStr(fonttex, "Score: " + intconv.str(), 0.1, -0.06, 1, 1, 1, 1, -0.95, 0.95);
			intconv.flush();
		}
		else if (state == OVER){
			for (int i = 0; i < gbullet.size(); i++){
				if (gbullet[i].isVis()){
					gbullet[i].toggleVis();
				}
			}
			for (int i = 0; i < bbullet.size(); i++){
				if (bbullet[i].isVis()){
					bbullet[i].toggleVis();
				}
			}
			for (int i = 0; i < enemies.size(); i++){
				if (enemies[i].isVis()){
					enemies[i].toggleVis();
				}
			}
			glClear(GL_COLOR_BUFFER_BIT);
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
			DrawStr(fonttex, "Press SPACE to return to title.", 0.1, -0.06, 1, 1, 1, 1, -0.45, 0);
		}
		SDL_GL_SwapWindow(displayWindow);

	}

	SDL_Quit();
	return 0;
}
