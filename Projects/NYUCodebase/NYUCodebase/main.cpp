//Scott Lee
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_surface.h>
#include <string>

SDL_Window* displayWindow;


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

void DrawSprite(GLint texture, float x, float y, float rotation, float proportion) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -proportion, proportion, -proportion, -proportion, proportion, -proportion, proportion, proportion };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
}

int main(int argc, char *argv[])
{
	//Note:Program assumes all assets are in same folder as executable.  I got these assets from the assets folder.
	//Used LoadTexture and DrawSprite from slides.  Slightly modified DrawSprite (Added an additional parameter).  
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	bool done = false;
	float lastFrameTicks = 0.0f;
	float texAngle = 0.0f;
	SDL_Event event;
	glClearColor(0.4f, 0.2f, 0.4f, 1.0f);

	GLuint test = LoadTexture("ufoBlue.png");
	GLuint test2 = LoadTexture("enemyBlack4.png");
	GLuint test3 = LoadTexture("enemyRed1.png");
	GLfloat triangle[] = { 0.0f, -.7f, -.1f, -0.9f, .1f, -0.9f };
	GLfloat triangleColors[] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
		0.0, 1.0 };

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		texAngle += elapsed * 20;

		glVertexPointer(2, GL_FLOAT, 0, triangle);
		glEnableClientState(GL_VERTEX_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, triangleColors);
		glEnableClientState(GL_COLOR_ARRAY);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		DrawSprite(test2, -0.3f, 0.5f, 0.0f, 0.1);
		DrawSprite(test3, 0.2f, 0.7f, 0.0f, 0.1);
		float xpos = sin(texAngle/ 90) / 1.5;
		DrawSprite(test, xpos, -0.4, texAngle, 0.2);
		glLoadIdentity();
		//
		SDL_GL_SwapWindow(displayWindow);
		
	}

	SDL_Quit();
	return 0;
}