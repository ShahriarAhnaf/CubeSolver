#pragma once
#include "Cube.h"
#include <string>
#include <vector>

enum {
		halal_cross=1,
		matched_cross,
		first_layer,
		middle_layer,
		final_layer
} Gamestate;

// Struct to represent a target state with masks for specifying which stickers are relevant
struct TargetState {
    uint64_t faces[6];        // The target face values
    uint64_t masks[6];        // Masks for each face (1 = relevant, 0 = don't care)
    
    // Constructor to create a target state with all stickers relevant
    TargetState() {
        for(int i = 0; i < 6; i++) {
            faces[i] = 0;
            masks[i] = 0xFFFFFFFFFFFFFFFF; // All stickers relevant by default
        }
    }
    
    // Constructor to create a target state from face values
    TargetState(uint64_t face_values[6]) {
        for(int i = 0; i < 6; i++) {
            faces[i] = face_values[i];
            masks[i] = 0xFFFFFFFFFFFFFFFF; // All stickers relevant by default
        }
    }
    
    // Helper method to set a face with a mask
    void set_face(uint8_t face_index, uint64_t face_value, uint64_t face_mask) {
        faces[face_index] = face_value;
        masks[face_index] = face_mask;
    }
    
    // Helper method to set a specific sticker as don't care
    void set_dont_care(uint8_t face_index, uint8_t position) {
		SET_COLOR(masks[face_index], position, 0x00U);
    }
    
    // Helper method to set an entire face as don't care
    void set_face_dont_care(uint8_t face_index) {
        masks[face_index] = 0;
    }
};

// RubixCube Solved_Cube = RubixCube();
class Solver{
private: 
	const std::string Moveset[18] = {
      "L", "LPRIME", "L2",
	  "R", "RPRIME", "R2",
      "U", "UPRIME", "U2",
      "D", "DPRIME", "D2",
      "F", "FPRIME", "F2",
      "B", "BPRIME", "B2"
	}; 

    // Helper function to check if a cube matches a target state
    bool matches_target(const RubixCube& cube, const TargetState& target);

	std::string Solve_DFS(RubixCube current_cube, TargetState target_state, std::string Moves, int depth_remaining);
	std::string Solve_IDFS(RubixCube given_cube, TargetState target_state, int Depth_Limit);
	bool has_white_cross(RubixCube& cube);
	bool has_white_corners(RubixCube& cube);
	bool has_second_layer(RubixCube& cube);
	bool has_bottom_layer(RubixCube& cube);
	std::string solve_white_cross(RubixCube& cube);
	std::string solve_white_corners(RubixCube& cube);
	std::string solve_second_layer(RubixCube& cube);
	std::string solve_bottom_cross(RubixCube& cube);
	std::string solve_bottom_corners(RubixCube& cube);
	
    // Helper function to check if a move would undo the previous move
    bool is_redundant_move(const std::string& current_move, const std::string& previous_move) const {
        if (previous_move.empty()) return false;
        
        // Get the base move (without prime or 2)
        char current_base = current_move[0];
        char prev_base = previous_move[0];
        
        // Same face moves are redundant
        if (current_base == prev_base) return true;
        
        // Check if current move would undo previous move
        if (current_base == prev_base) {
            // If previous was prime and current is not, or vice versa
            if ((previous_move.find("PRIME") != std::string::npos && current_move.find("PRIME") == std::string::npos) ||
                (previous_move.find("PRIME") == std::string::npos && current_move.find("PRIME") != std::string::npos)) {
                return true;
            }
        }
        
        return false;
    }

public: 
	//copy for now, might be useful to pass by ref?
	Solver(){}
	// Solver(RubixCube cube){
	// 	Cube = cube;
	// }
	~Solver();
	bool is_Solved(RubixCube cube){
		return cube == RubixCube(); // matching a solved cube
	}
	void Solve_Cube(RubixCube &given_cube, int Depth_Limit);
	void visualize_state();
	void scramble();
	RubixCube Apply_Moves(RubixCube &El_Cube, std::string leMoves);
};