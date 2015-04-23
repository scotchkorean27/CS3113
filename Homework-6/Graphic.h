#pragma once
#include <string>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_surface.h>
#include <vector>

float easeIn(float from, float to, float time);

float easeOutElastic(float from, float to, float time);

float easeInOut(float from, float to, float time);

void DrawStr(
	int fontTexture, std::string text, float size, float spacing,
	float r, float g, float b, float a, float x, float y);

GLuint LoadTexture(const char *image_path);