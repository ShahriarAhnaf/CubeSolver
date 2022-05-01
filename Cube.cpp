#include <iostream>
#include <cmath>
#include "Cube.h"

int8_t location_center[6][2] = {{18,1},{9,5}, {18,5}, {27,5}, {36,5}, {18, 10}};
int8_t print_relative_mapping[9][2] = {{-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}};

//masks
uint64_t mask = (~0); //max 64 uint
// 				MASKS
//                       0 1 2 3 4 5 6 7
uint64_t mask_upper  = 0xFFFFFF0000000000; // index 0,1,2
uint64_t mask_right  = 0x0000FFFFFF000000; // index 2,3,4
uint64_t mask_left   = 0xFF0000000000FFFF; // index 0,7,6
uint64_t mask_lower  = 0x00000000FFFFFF00; // index 6,5,4
uint64_t mask_cube   = 0xFF00000000000000;
// helpers
void print_bytes(uint64_t n, int location, int location_y){
	int a[64],i;
	for(i=0; n>0; i++)    
	{    
	a[i]=n%2;    
	n= n/2;  
	}        
	for(i=i-1 ;i>=0 ;i--)    
	{    
		move(location_y, location-i);
		if(a[i]){
		printw("1");    
		}
		else{
		printw("0");
		}
	}
}
void RubixCube::draw_face(uint8_t face_number){
	for(int x=0; x<8;x++) {
	char pp;
	int mask_shift = 8*x;
	// account for the eight bytes that you need which means you only need 56 shift max.
	int eight_byte_shift = 56 - mask_shift;
	//std::cout << "mask shift and eight shift = " << mask_shift<< " " << eight_byte_shift<<"\nmask cube used"; print_bytes(mask_cube >> mask_shift);
	uint64_t masked_bytes = faces[face_number] & (mask_cube >> mask_shift);
	uint64_t eight_bytes = masked_bytes >> (eight_byte_shift);
	//std::cout << "face " << (int) face_number << " cube num: "  << x << " : ";
	 
	//  print_bytes(faces[face_number]);
	//  std::cout << "masked -> ";
	//  print_bytes(masked_bytes) ;
	//  std::cout << "shifted eight -> ";
	//  print_bytes(eight_bytes);



	switch(eight_bytes){
		case WHITE:
			mvprintw(location_center[face_number][1]+ print_relative_mapping[x][1], location_center[face_number][0] + print_relative_mapping[x][0], "W");
			break;
		case BLUE:
			mvprintw(location_center[face_number][1]+ print_relative_mapping[x][1], location_center[face_number][0] + print_relative_mapping[x][0],"B");
			break;
		case RED:
			mvprintw(location_center[face_number][1]+ print_relative_mapping[x][1], location_center[face_number][0] + print_relative_mapping[x][0], "R");
			break;
		case GREEN:
			mvprintw(location_center[face_number][1]+ print_relative_mapping[x][1], location_center[face_number][0] + print_relative_mapping[x][0], "G");
			break;
		case ORANGE:
			mvprintw(location_center[face_number][1]+ print_relative_mapping[x][1], location_center[face_number][0] + print_relative_mapping[x][0], "O");
			break;
		case YELLOW:
			mvprintw(location_center[face_number][1]+ print_relative_mapping[x][1], location_center[face_number][0] + print_relative_mapping[x][0],"Y");
			break;
		default:
			mvprintw(location_center[face_number][1]+ print_relative_mapping[x][1], location_center[face_number][0] + print_relative_mapping[x][0], "X");
			break;
	}
	}
}

void RubixCube::draw(){
    // printing each face
    for(int n=0; n < 6; n++){draw_face(n);}
}

// moves the upper face clockwise

RubixCube::RubixCube(){
	faces = new u_int64_t[6]{};
    faces[0] = 0x05;
	faces[1] = 0x0101010101010101;
	faces[2] = 0x0202020202020202;
	faces[3] = 0x0303030303030303;
	faces[4] = 0x0404040404040404;
	faces[5] = 0x0505050505050505;
}


void RubixCube::U(uint64_t num_of_turns){
	// face itself
	// mask the end bits so that they can wrap around
	int mask_shift = 64-16*num_of_turns;
	uint64_t temp = faces[0] & (mask >> mask_shift);
	//print_bytes(temp, 80,22);
	//print_bytes(faces[0], 80,20);
	faces[0] >>= 16*num_of_turns;
	//moves the bits back all the way to the left.
	temp <<= mask_shift;
	//print_bytes(temp, 80,24);
	faces[0] |= temp;
	//print_bytes(faces[0], 90,21);
	// other consequences 

	uint64_t anti_mask = ~mask_upper;

	// face 1
	temp = faces[1];
	//to find the othes bydet op of the face and preserve them
	faces[1] &= anti_mask; // get rid of the upper layer in the face
	faces[1] |= ((faces[2] & mask_upper));
	
	// front face affected
	// face 2
	//print_bytes(anti_mask, 40,40);
	faces[2] &= anti_mask; // get rid of the upper layer in the face
	faces[2] |= ((faces[3] & mask_upper));
	//print_bytes(faces[2], 90, 30);
	// face 3
	//to find the othes bydet op of the face and preserve them

	faces[3] &= anti_mask; // get rid of the upper layer in the face
	faces[3] |= ((faces[4] & mask_upper));

	//to find the othes bydet op of the face and preserve them
	faces[4] &= anti_mask; // get rid of the upper layer in the face
	faces[4] |= ((temp & mask_upper));

}
void RubixCube::D(uint64_t num_of_turns){}
void RubixCube::F(uint64_t num_of_turns){}
void RubixCube::R(uint64_t num_of_turns){}
void RubixCube::L(uint64_t num_of_turns){}
void RubixCube::B(uint64_t num_of_turns){}
void RubixCube::U_PRIME(uint64_t num_of_turns){
	// face itself
	// mask the first bits so that they can wrap around
	int mask_shift = 64-16*num_of_turns;
	uint64_t temp = faces[0] & (mask << mask_shift);
	//print_bytes(temp, 80,22);
	//print_bytes(faces[0], 80,20);
	faces[0] <<= 16*num_of_turns;
	//moves the bits back all the way to the left.
	temp >>= mask_shift;
	//print_bytes(temp, 80,24);
	faces[0] |= temp;
	//print_bytes(faces[0], 90,21);
	// other consequences 

	uint64_t anti_mask = ~mask_upper;

	// face 1
	temp = faces[4];
	
	
//to find the othes bydet op of the face and preserve them
	faces[4] &= anti_mask; // get rid of the upper layer in the face
	faces[4] |= (faces[3] & mask_upper);

	faces[3] &= anti_mask; // get rid of the upper layer in the face
	faces[3] |= ((faces[2] & mask_upper));

	faces[2] &= anti_mask; // get rid of the upper layer in the face
	faces[2] |= ((faces[1] & mask_upper));
	//to find the othes bydet op of the face and preserve them
	faces[1] &= anti_mask; // get rid of the upper layer in the face
	faces[1] |= ((temp & mask_upper));
	
	// face 3
	//to find the othes bydet op of the face and preserve them



}
void RubixCube::D_PRIME(uint64_t num_of_turns){}
void RubixCube::F_PRIME(uint64_t num_of_turns){}
void RubixCube::R_PRIME(uint64_t num_of_turns){}
void RubixCube::L_PRIME(uint64_t num_of_turns){}
void RubixCube::B_PRIME(uint64_t num_of_turns){}

