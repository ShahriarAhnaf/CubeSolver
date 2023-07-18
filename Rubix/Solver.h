#pragma once 
#include <string>
#include <vector>
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
	std::string Moveset[18] = {
      "L", "LPRIME", "L2",
	  "R", "RPRIME", "R2",
      "U", "UPRIME", "U2",
      "D", "DPRIME", "D2",
      "F","FPRIME", "F2",
      "B", "BPRIME", "B2"
	}; 
public: 
	Solver();
	~Solver();
	bool is_Solved(RubixCube cube){
		return cube == RubixCube(); // matching a solved cube
	}
	void visualize_state();
	std::string Solve_DFS(RubixCube current_cube, std::string Moves, int depth_limit);
	std::string Solve_IDFS();
	RubixCube Apply_Moves(RubixCube El_Cube, char* leMoves);
};