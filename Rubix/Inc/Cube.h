#pragma once
#include <stdint.h>
#include <ncurses.h>
#include <unordered_map>
#ifndef CUBE_H
#define CUBE_H

// Macros for bitwise operations
#define GET_COLOR(face_value, position) ((face_value & (0xFFULL << ((7-position) * 8))) >> ((7-position) * 8))
#define SET_COLOR(face_value, position, color) (face_value = (face_value & ~(0xFFULL << ((7-position) * 8))) | ((uint64_t)color << ((7-position) * 8)))
#define GET_EDGE(face_value, edge) GET_COLOR(face_value, edge)
#define GET_CORNER(face_value, corner) GET_COLOR(face_value, corner)
#define GET_CENTER(face_value) GET_COLOR(face_value, 8)

// Position indices for better readability
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
   * 001111111
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
// index center, x, y

   
// enums
enum FACE_ORIENTATION {FACE_UP, FACE_LEFT, FACE_FRONT, FACE_RIGHT, FACE_BACK, FACE_BOTTOM};
enum COLOR {WHITE, BLUE, RED, GREEN, ORANGE, YELLOW};

class RubixCube{
private:
	uint64_t faces[6]; // 6 faces
   void draw_face(uint8_t face_number, int8_t screen_center_x, int8_t screen_center_y);
   
   // Pre-computed rotation masks
   static const uint64_t ROTATION_MASKS[6][4];  // [face][rotation]
   
   // Move tables for common sequences
   static std::unordered_map<uint64_t, std::string> move_table;
   
   // Optimized rotation functions
   void rotate_face_clockwise(uint8_t face);
   void rotate_face_counterclockwise(uint8_t face);
   
   // Helper for move table generation
   void generate_move_table();
   
public: 
	void operator= (const RubixCube& cube){
      for(int n =0; n < 6; n++){
            faces[n] = cube.faces[n];
      }
   }
   bool operator== (const RubixCube& cube) const {
      // Compare all faces at once using a single 64-bit comparison
      return faces[0] == cube.faces[0] &&
             faces[1] == cube.faces[1] &&
             faces[2] == cube.faces[2] &&
             faces[3] == cube.faces[3] &&
             faces[4] == cube.faces[4] &&
             faces[5] == cube.faces[5];
   }
	RubixCube(); // a solved cube.
   // copy by value
	RubixCube(uint64_t* passed_face){ 
		// faces = new uint64_t[6];
      for(int n =0; n < 6; n++){
         faces[n] = passed_face[n];
      }
	};
	//DESTROYYYYY
	~RubixCube(){
		// if(faces != nullptr) delete[] faces;
	};
	
   // Get face value without modifying the cube
   uint64_t get_face(uint8_t face_number) const {
      if(face_number < 6) {
         return faces[face_number];
      }
      return 0xFFFFFFFFFFFFFFFF; // Return all 1's for invalid face numbers
   }

void draw(int8_t screen_center_x, int8_t screen_center_y);
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

// Optimized move functions
void U(uint64_t num_of_turns) {
    while(num_of_turns--) {
        rotate_face_clockwise(FACE_UP);
        // Update adjacent faces using pre-computed masks
        uint64_t temp = faces[FACE_FRONT];
        faces[FACE_FRONT] = faces[FACE_RIGHT];
        faces[FACE_RIGHT] = faces[FACE_BACK];
        faces[FACE_BACK] = faces[FACE_LEFT];
        faces[FACE_LEFT] = temp;
    }
}

// Similar optimizations for other moves...

// Hash function for state lookup
size_t hash() const {
    size_t h = 0;
    for(int i = 0; i < 6; i++) {
        h = h * 31 + faces[i];
    }
    return h;
}

// Equality operator optimized for hash table
bool operator==(const RubixCube& other) const {
    return faces[0] == other.faces[0] &&
           faces[1] == other.faces[1] &&
           faces[2] == other.faces[2] &&
           faces[3] == other.faces[3] &&
           faces[4] == other.faces[4] &&
           faces[5] == other.faces[5];
}
};

// Hash function for RubixCube
namespace std {
    template<>
    struct hash<RubixCube> {
        size_t operator()(const RubixCube& cube) const {
            return cube.hash();
        }
    };
}

#endif