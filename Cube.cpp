#include <iostream>
#include <cmath>
#include "Cube.h"
#include <ncurses.h>


void RubixCube::draw(){
	initscr();

    // moving cursor, x = 20, y = 10
    mvprintw(10, 20, "Hello World!");

    move(11, 10);
    printw("Now i am here");

    refresh();
    getch();
    endwin();
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

