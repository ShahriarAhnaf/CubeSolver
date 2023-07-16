#pragma once
#include <stdint.h>
#include <ncurses.h>
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
   *  B B B   R R R   G G G   O O O
   *  B B B   R R R   G G G   O O O
   *  B B B   R R R   G G G   O O O
   *
   *          Y Y Y
   *          Y Y Y
   *          Y Y Y
   *
   * Index wise:
   * 001111111
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
// index center, x, y
enum goal{
	cross=1,
	first_layer,
	middle_layer,
	final_layer
};
   
   // enums
enum FACE  {UP, LEFT, FRONT, RIGHT, BACK, DOWN};
enum COLOR {WHITE, BLUE, RED, GREEN, ORANGE, YELLOW};
enum MOVE{
      L, LPRIME, 
	   R, RPRIME, 
      U, UPRIME, 
      D, DPRIME, 
      F, FPRIME, 
      B, BPRIME 	
	}; 
class RubixCube{
private:
	uint64_t* faces; // 6 faces
   void draw_face(uint8_t face_number);
   void shift_upper_();
public: 
	
	RubixCube(); // never a random cube creation.
	RubixCube(uint64_t* passed_face){
		faces = passed_face;
	};
	//DESTROYYYYY
	~RubixCube(){
		
	};
	
void draw();
void U(uint64_t num_of_turns);
void D(uint64_t num_of_turns);
void F(uint64_t num_of_turns);
void R(uint64_t num_of_turns);
void L(uint64_t num_of_turns);
void B(uint64_t num_of_turns);
void U_PRIME(uint64_t num_of_turns);
void D_PRIME(uint64_t num_of_turns);
void F_PRIME(uint64_t num_of_turns);
void R_PRIME(uint64_t num_of_turns);
void L_PRIME(uint64_t num_of_turns);
void B_PRIME(uint64_t num_of_turns);
};

#endif