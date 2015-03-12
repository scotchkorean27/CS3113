
//Scott Lee
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <ctime>

SDL_Window* displayWindow;

class Rect{
public:
	Rect(GLfloat lx, GLfloat ly, float lh, GLfloat lw){
		x = lx;
		y = ly;
		h = lh;
		w = lw;
	};

	void Update(GLfloat dx, GLfloat dy){
		x += dx;
		y += dy;
	};

	void Draw(){

		GLfloat rect[] = { lx(), uy(), lx(), dy(), rx(), dy(), rx(), uy() };
		glVertexPointer(2, GL_FLOAT, 0, rect);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
	};

	bool overlaps(Rect other){
		return (dy() < other.uy() && uy() > other.dy() && lx() < other.rx() && rx() > other.lx());
	};


	GLfloat lx(){
		return x - w / 2;
	};
	GLfloat rx(){
		return x + w / 2;
	};
	GLfloat uy(){
		return y + h / 2;
	};
	GLfloat dy(){
		return y - h / 2;
	};
	GLfloat x;
	GLfloat y;
	GLfloat w;
	GLfloat h;
};

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, SDL_WINDOW_OPENGL);
	glViewport(0, 0, 1200, 600);
	glOrtho(-1.66, 1.66, -1, 1, -1, 1);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	bool done = false;
	float lastFrameTicks = 0.0f;
	float texAngle = 0.0f;
	SDL_Event event;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	srand(time(0));

	Rect p1(-.9, 0, 0.6, 0.05);
	Rect p2(0.9, 0.0, 0.6, 0.05);
	Rect puck(0.0, 0.0, 0.1, 0.06);
	GLfloat p2vel = 0.4;

	float angle = rand() % 360 - 180;
	GLfloat pxvel = cos(angle) / 1.5;
	GLfloat pyvel = sin(angle)/1.5;
	

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		GLfloat p1vel = 0;
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_UP] && p1.uy() < 1){
			p1vel = 0.4;
		}
		else if (keys[SDL_SCANCODE_DOWN] && p1.dy() > -1){
			p1vel = -0.4;
		}
		glClear(GL_COLOR_BUFFER_BIT);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;



		

		if (puck.uy() >= 1 || puck.dy() <= -1){
			pyvel *= -1;
		}
		if (puck.rx() >= 1 || puck.lx() <= -1){
			puck.x = 0;
			puck.y = 0;
		}

		if (puck.uy() > p2.y){
			p2vel = 0.3;
		}
		if (puck.dy() < p2.y){
			p2vel = -0.3;
		}
		if ( (p2.uy() >= 1 && p2vel > 0) || (p2.dy() <= -1 && p2vel < 0)){
			p2vel = 0;
		}

		if (puck.overlaps(p1)){
			GLfloat dis = sqrt(pow(puck.x - p1.x, 2) + pow(puck.y - p1.y, 2));
			pxvel = (puck.x - p1.x) / (dis * 1.5);
			pyvel = (puck.y - p1.y) / (dis * 1.5);
		}
		else if (puck.overlaps(p2)){
			GLfloat dis = sqrt(pow(puck.x - p2.x, 2) + pow(puck.y - p2.y, 2));
			pxvel = (puck.x - p2.x) / (dis * 2);
			pyvel = (puck.y - p2.y) / (dis * 2);
		}

		p1.Draw();
		glLoadIdentity();
		p2.Draw();
		p1.Update(0, p1vel * elapsed);
		p2.Update(0, p2vel * elapsed);
		puck.Draw();
		puck.Update(pxvel * elapsed, pyvel * elapsed);
		


		//
		SDL_GL_SwapWindow(displayWindow);

	}

	SDL_Quit();
	return 0;
}

