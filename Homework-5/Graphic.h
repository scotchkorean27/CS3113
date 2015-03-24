#pragma once
#include <string>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_surface.h>;

void DrawLevel(int tex, unsigned int** levelDat, int SCX, int SCY, float TS, int height, int width);

void DrawStr(
	int fontTexture, std::string text, float size, float spacing,
	float r, float g, float b, float a, float x, float y);

GLuint LoadTexture(const char *image_path);