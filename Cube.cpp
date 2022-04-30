#include <iostream>
#include <cmath>
#include "Cube.h"

int8_t location_center[6][2] = {{18,1},{9,5}, {18,5}, {27,5}, {36,5}, {18, 10}};
int8_t print_relative_mapping[9][2] = {{-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}};

void draw_face(uint8_t face_number){
	for(int x=0; x<8;x++){
	mvprintw(location_center[face_number][1]+ print_relative_mapping[x][1], location_center[face_number][0] + print_relative_mapping[x][0],"C");
	}
}

void RubixCube::draw(){
    // printing each face
    for(int n=0; n < 6; n++){draw_face(n);}
    refresh();
}

uint64_t mask = (~0); //max
uint64_t mask_upper = 0xFFF00000; 
// moves the upper face clockwise

RubixCube::RubixCube(){
	faces = new u_int64_t[6]{};
    faces[0] = 0x0;
	faces[1] = 0x1010101010101010;
	faces[2] = 0x2020202020202020;
	faces[3] = 0x3030303030303030;
	faces[4] = 0x4040404040404040;
	faces[5] = 0x5050505050505050;
}


void RubixCube::U(uint64_t num_of_turns){
	// face itself
	uint64_t temp = faces[0] & (mask >> (16*num_of_turns));
	faces[0] >>= 16*num_of_turns;
	temp <<= 64-16*num_of_turns;
	faces[0] &= temp;

	// other consequences 
	// front face affected
	temp = faces[2];
	// l
	faces[2] &= ((faces[3] & mask_upper) | 0x000FFFFF); 
}
void RubixCube::D(uint64_t num_of_turns){}
void RubixCube::F(uint64_t num_of_turns){}
void RubixCube::R(uint64_t num_of_turns){}
void RubixCube::L(uint64_t num_of_turns){}
void RubixCube::B(uint64_t num_of_turns){}
void RubixCube::U_PRIME(uint64_t num_of_turns){}
void RubixCube::D_PRIME(uint64_t num_of_turns){}
void RubixCube::F_PRIME(uint64_t num_of_turns){}
void RubixCube::R_PRIME(uint64_t num_of_turns){}
void RubixCube::L_PRIME(uint64_t num_of_turns){}
void RubixCube::B_PRIME(uint64_t num_of_turns){}

