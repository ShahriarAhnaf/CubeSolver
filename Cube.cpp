#include <iostream>
#include <cmath>
#include "Cube.h"

int8_t location_center[6][2] = {{18,1},{9,5}, {18,5}, {27,5}, {36,5}, {18, 10}};
int8_t print_relative_mapping[9][2] = {{-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}};

//masks
uint64_t mask = (~0); //max
uint64_t mask_upper = 0xFFFFFF000000000; 
uint64_t mask_cube = 0xFF00000000000000;

void print_bytes(uint64_t n){
	int a[64],i;
	for(i=0; n>0; i++)    
	{    
	a[i]=n%2;    
	n= n/2;  
	}        
	for(i=i-1 ;i>=0 ;i--)    
	{    
		std::cout<<a[i];    
	}  
	std::cout << std::endl;
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
    faces[0] = 0x0;
	faces[1] = 0x0101010101010101;
	faces[2] = 0x0202020202020202;
	faces[3] = 0x0303030303030303;
	faces[4] = 0x0404040404040404;
	faces[5] = 0x0505050505050505;
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

