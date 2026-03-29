#include <iostream>
#include <cmath>
#include <sstream>
#include "Cube.h"

uint64_t mask = (~0ULL);
uint64_t mask_upper  = 0xFFFFFF0000000000;
uint64_t mask_right  = 0x0000FFFFFF000000;
uint64_t mask_left   = 0xFF0000000000FFFF;
uint64_t mask_lower  = 0x00000000FFFFFF00;

static uint64_t generic_turn(uint64_t face){
	int mask_shift = 64-16;
	uint64_t temp = face & (mask >> mask_shift);
	face >>= 16;
	temp <<= mask_shift;
	return face | temp;
}

static uint64_t generic_turn_prime(uint64_t face){
	int mask_shift = 64-16;
	uint64_t temp = face & (mask << mask_shift);
	face <<= 16;
	temp >>= mask_shift;
	return face | temp;
}

RubixCube::RubixCube(){
    faces[0] = 0x00;
	faces[1] = 0x0101010101010101;
	faces[2] = 0x0202020202020202;
	faces[3] = 0x0303030303030303;
	faces[4] = 0x0404040404040404;
	faces[5] = 0x0505050505050505;
}

void RubixCube::U(uint64_t num_of_turns){
    for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[0] = generic_turn(faces[0]);
		uint64_t anti_mask = ~mask_upper;
		uint64_t temp = faces[1];
		faces[1] &= anti_mask; faces[1] |= (faces[2] & mask_upper);
		faces[2] &= anti_mask; faces[2] |= (faces[3] & mask_upper);
		faces[3] &= anti_mask; faces[3] |= (faces[4] & mask_upper);
		faces[4] &= anti_mask; faces[4] |= (temp & mask_upper);
	}
}

void RubixCube::D(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[5] = generic_turn(faces[5]);
		uint64_t anti_mask = ~mask_lower;
		uint64_t temp = faces[1];
		faces[1] &= anti_mask; faces[1] |= (faces[4] & mask_lower);
		faces[4] &= anti_mask; faces[4] |= (faces[3] & mask_lower);
		faces[3] &= anti_mask; faces[3] |= (faces[2] & mask_lower);
		faces[2] &= anti_mask; faces[2] |= (temp & mask_lower);
	}
}

void RubixCube::F(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[2] = generic_turn(faces[2]);
		uint64_t anti_mask;
		uint64_t temp = faces[0];
		anti_mask = ~mask_lower;
		faces[0] &= anti_mask; faces[0] |= generic_turn(faces[1] & mask_right);
		anti_mask = ~mask_right;
		faces[1] &= anti_mask; faces[1] |= generic_turn(faces[5] & mask_upper);
		anti_mask = ~mask_upper;
		faces[5] &= anti_mask; faces[5] |= generic_turn(faces[3] & mask_left);
		anti_mask = ~mask_left;
		faces[3] &= anti_mask; faces[3] |= generic_turn(temp & mask_lower);
	}
}

void RubixCube::R(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[3] = generic_turn(faces[3]);
		uint64_t anti_mask;
		uint64_t temp = faces[2];
		anti_mask = ~mask_right;
		faces[2] &= anti_mask; faces[2] |= faces[5] & mask_right;
		anti_mask = ~mask_right;
		faces[5] &= anti_mask; faces[5] |= generic_turn(generic_turn(faces[4] & mask_left));
		anti_mask = ~mask_left;
		faces[4] &= anti_mask; faces[4] |= generic_turn(generic_turn(faces[0] & mask_right));
		anti_mask = ~mask_right;
		faces[0] &= anti_mask; faces[0] |= (temp & mask_right);
	}
}

void RubixCube::L(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[1] = generic_turn(faces[1]);
		uint64_t anti_mask;
		uint64_t temp = faces[2];
		anti_mask = ~mask_left;
		faces[2] &= anti_mask; faces[2] |= faces[0] & mask_left;
		anti_mask = ~mask_left;
		faces[0] &= anti_mask; faces[0] |= generic_turn(generic_turn(faces[4] & mask_right));
		anti_mask = ~mask_right;
		faces[4] &= anti_mask; faces[4] |= generic_turn(generic_turn(faces[5] & mask_left));
		anti_mask = ~mask_left;
		faces[5] &= anti_mask; faces[5] |= (temp & mask_left);
	}
}

void RubixCube::B(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[4] = generic_turn(faces[4]);
		uint64_t anti_mask;
		uint64_t temp = faces[0];
		anti_mask = ~mask_upper;
		faces[0] &= anti_mask; faces[0] |= generic_turn_prime(faces[3] & mask_right);
		anti_mask = ~mask_right;
		faces[3] &= anti_mask; faces[3] |= generic_turn_prime(faces[5] & mask_lower);
		anti_mask = ~mask_lower;
		faces[5] &= anti_mask; faces[5] |= generic_turn_prime(faces[1] & mask_left);
		anti_mask = ~mask_left;
		faces[1] &= anti_mask; faces[1] |= generic_turn_prime(temp & mask_upper);
	}
}

void RubixCube::U_PRIME(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[0] = generic_turn_prime(faces[0]);
		uint64_t anti_mask = ~mask_upper;
		uint64_t temp = faces[4];
		faces[4] &= anti_mask; faces[4] |= (faces[3] & mask_upper);
		faces[3] &= anti_mask; faces[3] |= (faces[2] & mask_upper);
		faces[2] &= anti_mask; faces[2] |= (faces[1] & mask_upper);
		faces[1] &= anti_mask; faces[1] |= (temp & mask_upper);
	}
}

void RubixCube::D_PRIME(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[5] = generic_turn_prime(faces[5]);
		uint64_t anti_mask = ~mask_lower;
		uint64_t temp = faces[1];
		faces[1] &= anti_mask; faces[1] |= (faces[2] & mask_lower);
		faces[2] &= anti_mask; faces[2] |= (faces[3] & mask_lower);
		faces[3] &= anti_mask; faces[3] |= (faces[4] & mask_lower);
		faces[4] &= anti_mask; faces[4] |= (temp & mask_lower);
	}
}

void RubixCube::F_PRIME(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[2] = generic_turn_prime(faces[2]);
		uint64_t anti_mask;
		uint64_t temp = faces[0];
		anti_mask = ~mask_lower;
		faces[0] &= anti_mask; faces[0] |= generic_turn_prime(faces[3] & mask_left);
		anti_mask = ~mask_left;
		faces[3] &= anti_mask; faces[3] |= generic_turn_prime(faces[5] & mask_upper);
		anti_mask = ~mask_upper;
		faces[5] &= anti_mask; faces[5] |= generic_turn_prime(faces[1] & mask_right);
		anti_mask = ~mask_right;
		faces[1] &= anti_mask; faces[1] |= generic_turn_prime(temp & mask_lower);
	}
}

void RubixCube::R_PRIME(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[3] = generic_turn_prime(faces[3]);
		uint64_t anti_mask;
		uint64_t temp = faces[2];
		anti_mask = ~mask_right;
		faces[2] &= anti_mask; faces[2] |= faces[0] & mask_right;
		anti_mask = ~mask_right;
		faces[0] &= anti_mask; faces[0] |= generic_turn(generic_turn(faces[4] & mask_left));
		anti_mask = ~mask_left;
		faces[4] &= anti_mask; faces[4] |= generic_turn(generic_turn(faces[5] & mask_right));
		anti_mask = ~mask_right;
		faces[5] &= anti_mask; faces[5] |= (temp & mask_right);
	}
}

void RubixCube::L_PRIME(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[1] = generic_turn_prime(faces[1]);
		uint64_t anti_mask;
		uint64_t temp = faces[2];
		anti_mask = ~mask_left;
		faces[2] &= anti_mask; faces[2] |= faces[5] & mask_left;
		anti_mask = ~mask_left;
		faces[5] &= anti_mask; faces[5] |= generic_turn(generic_turn(faces[4] & mask_right));
		anti_mask = ~mask_right;
		faces[4] &= anti_mask; faces[4] |= generic_turn(generic_turn(faces[0] & mask_left));
		anti_mask = ~mask_left;
		faces[0] &= anti_mask; faces[0] |= (temp & mask_left);
	}
}

void RubixCube::B_PRIME(uint64_t num_of_turns){
	for(uint64_t turns = 0; turns < num_of_turns; turns++){
		faces[4] = generic_turn_prime(faces[4]);
		uint64_t anti_mask;
		uint64_t temp = faces[0];
		anti_mask = ~mask_upper;
		faces[0] &= anti_mask; faces[0] |= generic_turn(faces[1] & mask_left);
		anti_mask = ~mask_left;
		faces[1] &= anti_mask; faces[1] |= generic_turn(faces[5] & mask_lower);
		anti_mask = ~mask_lower;
		faces[5] &= anti_mask; faces[5] |= generic_turn(faces[3] & mask_right);
		anti_mask = ~mask_right;
		faces[3] &= anti_mask; faces[3] |= generic_turn(temp & mask_upper);
	}
}

void RubixCube::apply_move_index(int move_index) {
    switch(move_index) {
        case 0:  L(1); break;      case 1:  L_PRIME(1); break;  case 2:  L(2); break;
        case 3:  R(1); break;      case 4:  R_PRIME(1); break;  case 5:  R(2); break;
        case 6:  U(1); break;      case 7:  U_PRIME(1); break;  case 8:  U(2); break;
        case 9:  D(1); break;      case 10: D_PRIME(1); break;  case 11: D(2); break;
        case 12: F(1); break;      case 13: F_PRIME(1); break;  case 14: F(2); break;
        case 15: B(1); break;      case 16: B_PRIME(1); break;  case 17: B(2); break;
    }
}

void RubixCube::apply_moves(std::string moves) {
    std::istringstream iss(moves);
    std::string move;
    while (iss >> move) {
        if (move == "R") R(1);       else if (move == "R'") R_PRIME(1);  else if (move == "R2") R(2);
        else if (move == "L") L(1);  else if (move == "L'") L_PRIME(1);  else if (move == "L2") L(2);
        else if (move == "U") U(1);  else if (move == "U'") U_PRIME(1);  else if (move == "U2") U(2);
        else if (move == "D") D(1);  else if (move == "D'") D_PRIME(1);  else if (move == "D2") D(2);
        else if (move == "F") F(1);  else if (move == "F'") F_PRIME(1);  else if (move == "F2") F(2);
        else if (move == "B") B(1);  else if (move == "B'") B_PRIME(1);  else if (move == "B2") B(2);
    }
}
