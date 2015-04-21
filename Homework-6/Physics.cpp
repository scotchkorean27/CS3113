#include "Physics.h"
#include <utility>

using namespace std;
#define FIXED_TIMESTEP 0.033333f
#define GRAVCONST 3
#define TILE_SIZE 0.075

float applyfriction(float speed, float fric){
	float t = FIXED_TIMESTEP * fric;
	return (1.0 - t)*speed;
}

float applygravity(float speed){
	return speed - (GRAVCONST * FIXED_TIMESTEP);
}

std::pair<int, int> worldToTileCoordinates(float worldX, float worldY) {
	int x = (int)((worldX + (TILE_SIZE * 128 / 2)) / TILE_SIZE);
	int y = (int)((-worldY + (TILE_SIZE * 32 / 2)) / TILE_SIZE);
	std::pair<int, int> ret;
	ret.first = x;
	ret.second = y;
	return ret;
}

float mapValue(float value, float srcMin, float srcMax, float dstMin, float dstMax) {
	float retVal = dstMin + ((value - srcMin) / (srcMax - srcMin) * (dstMax - dstMin));
	if (retVal < dstMin) {
		retVal = dstMin;
	}
	if (retVal > dstMax) {
		retVal = dstMax;
	}
	return retVal;
}