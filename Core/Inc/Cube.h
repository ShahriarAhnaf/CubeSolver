#pragma once
#include <stdint.h>
#include <string>
#ifndef CUBE_H
#define CUBE_H

#define GET_COLOR(face_value, position) ((face_value & (0xFFULL << ((7-position) * 8))) >> ((7-position) * 8))
#define SET_COLOR(face_value, position, color) (face_value = (face_value & ~(0xFFULL << ((7-position) * 8))) | ((uint64_t)color << ((7-position) * 8)))
#define GET_EDGE(face_value, edge) GET_COLOR(face_value, edge)
#define GET_CORNER(face_value, corner) GET_COLOR(face_value, corner)
#define GET_CENTER(face_value) GET_COLOR(face_value, 8)

#define TOP_LEFT 0
#define TOP 1
#define TOP_RIGHT 2
#define RIGHT 3
#define BOTTOM_RIGHT 4
#define BOTTOM 5
#define BOTTOM_LEFT 6
#define LEFT 7
#define CENTER 8

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
   *
   *             0  1  2
   *             7     3
   *             6  5  4
   *
   *   0  1  2   0  1  2   0 1 2   0  1  2
   *   7     3   7     3   7   3   7     3
   *   6  5  4   6  5  4   6 5 4   6  5  4
   *
   *             0  1  2
   *             7     3
   *             6  5  4
   *
   * Centers:
   *
   *    0
   *  1 2 3 4
   *    5
   */

enum FACE_ORIENTATION {FACE_UP, FACE_LEFT, FACE_FRONT, FACE_RIGHT, FACE_BACK, FACE_BOTTOM};
enum COLOR {WHITE, BLUE, RED, GREEN, ORANGE, YELLOW};

class RubixCube{
private:
	uint64_t faces[6];
public:
	void operator= (const RubixCube& cube){
      for(int n =0; n < 6; n++){
            faces[n] = cube.faces[n];
      }
   }
   bool operator== (const RubixCube& cube) const {
      return faces[0] == cube.faces[0] &&
             faces[1] == cube.faces[1] &&
             faces[2] == cube.faces[2] &&
             faces[3] == cube.faces[3] &&
             faces[4] == cube.faces[4] &&
             faces[5] == cube.faces[5];
   }
	RubixCube();
	RubixCube(uint64_t* passed_face){
      for(int n =0; n < 6; n++){
         faces[n] = passed_face[n];
      }
	};
	~RubixCube(){};

   uint64_t get_face(uint8_t face_number) const {
      if(face_number < 6) {
         return faces[face_number];
      }
      return 0xFFFFFFFFFFFFFFFF;
   }

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
void apply_moves(std::string moves);
void apply_move_index(int move_index);
};

#endif /* CUBE_H */
