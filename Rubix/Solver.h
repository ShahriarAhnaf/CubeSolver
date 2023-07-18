#pragma once 
#include <string>
#include "Cube.h"


enum {
		halal_cross=1,
		matched_cross,
		first_layer,
		middle_layer,
		final_layer
} Gamestate;

// RubixCube Solved_Cube = RubixCube();
class Solver{
private: 
	RubixCube Cube;

public: 
	Solver();
	~Solver();
	bool is_Solved(RubixCube cube){
		return cube == RubixCube(); // matching a solved cube
	}
	void visualize_state();
	std::string Solve_DFS(RubixCube current_cube, int depth_limit);
	std::string Solve_IDFS();
	RubixCube Apply_string_Moves(std::string moveset);

};