#include "stdint.h"
#include <iostream>

uint64_t mask_cube   = 0xFF00000000000000;

uint64_t make_left_uniform(uint64_t masked_left){
	uint64_t temp = masked_left & mask_cube;
	masked_left <<= 8;
    std::cout << masked_left << std::endl;
	return (masked_left | (temp >> 56)) << 8;
}

// the bits must be given in 0xFFFFFF form
uint64_t make_masked_left(uint64_t masked_bits){
	uint64_t temp = masked_bits & 0xFF; // gets last bits
	masked_bits >>= 8; // shift over the bits for the left
	temp <<= 56; // move the bits to the index of 0
	return masked_bits | temp;
}

uint64_t swap(uint64_t swapo){
    return (swapo << 8) | (swapo >> 8);
}

int main(){
    uint64_t test = 0x5334;
    std::cout << (test) <<std::endl;
    test = swap(test);
    std::cout << test << std::endl;
    return 0;
} 