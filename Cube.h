#pragma once
#include "Solver.h"
#include <stdint.h>
#ifndef CUBE_H
#define CUBE_H

	/*
   * The cube is laid out as follows.
   *
   * The sides:
   *
   *    U
   *  L F R B
   *    D
   *
   * Color wise:
   *
   *          W W W
   *          W W W
   *          W W W
   *
   *  G G G   R R R   B B B   O O O
   *  G G G   R R R   B B B   O O O
   *  G G G   R R R   B B B   O O O
   *
   *          Y Y Y
   *          Y Y Y
   *          Y Y Y
   *
   * Index wise:
   *
   *
   *              0  1  2
   *              7     3
   *              6  5  4
   *
   *   8  9 10   16 17 18   24 25 26   32 33 34
   *  15    11   23    19   31    27   39    35
   *  14 13 12   22 21 20   30 29 28   38 37 36
   *
   *             40 41 42
   *             47    43
   *             46 45 44
   *
   * Centers:
   *
   *    0
   *  1 2 3 4
   *    5
   */

enum goal{
	cross=1,
	first_layer,
	middle_layer,
	final_layer
};

// inherit from the homies
class RubixCube{
private:
	uint64_t* faces; // 6 faces
public: 
	enum class FACE   : uint8_t {UP, LEFT, FRONT, RIGHT, BACK, DOWN};
    enum class COLOR  : uint8_t {WHITE, GREEN, RED, BLUE, ORANGE, YELLOW};
    enum class MOVE   : uint8_t{
      L, LPRIME, L2,
      R, RPRIME, R2,
      U, UPRIME, U2,
      D, DPRIME, D2,
      F, FPRIME, F2,
      B, BPRIME, B2
	};
	RubixCube(){
		faces = new u_int64_t[6];
	};
	RubixCube(){
		
	};
	//DESTROYYYYY
	~RubixCube(){
		
	};
	
};




class GameState{
private: 
	RubixCube Cube;
	bool closer;
public: 
	GameState();
	~GameState();
};
#endif