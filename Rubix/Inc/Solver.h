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
	std::string Moveset[18] = {
      "L", "LPRIME", "L2",
	  "R", "RPRIME", "R2",
      "U", "UPRIME", "U2",
      "D", "DPRIME", "D2",
      "F","FPRIME", "F2",
      "B", "BPRIME", "B2"
	}; 
	std::string Solve_DFS(RubixCube current_cube, std::string Moves, int depth_limit);
	std::string Solve_IDFS();
	
public: 
	//copy for now, might be useful to pass by ref?
	Solver(){}
	// Solver(RubixCube cube){
	// 	Cube = cube;
	// }
	~Solver();
	bool is_Solved(RubixCube cube){
		return cube == RubixCube(); // matching a solved cube
	}
	void Solve_Cube(RubixCube given_cube);
	void visualize_state();
	void scramble();
	RubixCube Apply_Moves(RubixCube &El_Cube, std::string leMoves);
};