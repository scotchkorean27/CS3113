#include "Physics.h"
using namespace std;
#define FIXED_TIMESTEP 0.033333f
#define GRAVCONST 3

float applyfriction(float speed, float fric){
	float t = FIXED_TIMESTEP * fric;
	return (1.0 - t)*speed;
}

float applygravity(float speed){
	return speed - (GRAVCONST * FIXED_TIMESTEP);
}