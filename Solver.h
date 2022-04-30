#pragma once 
#include "Cube.h"


class GameState{
private: 
	RubixCube Cube;
	bool closer;
public: 
	GameState();
	~GameState();
};