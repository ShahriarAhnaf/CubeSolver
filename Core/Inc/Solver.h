#pragma once
#include "Cube.h"
#include <string>
#include <vector>
#include <functional>

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

    // Re-enable checking for a specific sticker position
    void set_relevant(uint8_t face_index, uint8_t position) {
        SET_COLOR(masks[face_index], position, 0xFFU);
    }

    // Re-enable checking for an entire face
    void set_face_relevant(uint8_t face_index) {
        masks[face_index] = 0xFFFFFFFFFFFFFFFF;
    }

    // Compare a cube with this target state, respecting don't care masks
    bool matches_cube(const RubixCube& cube) const {
        for(int i = 0; i < 6; i++) {
            uint64_t current_face = cube.get_face(i);
            // Compare only the bits that are not masked (not don't care)
            if((current_face & masks[i]) != (faces[i] & masks[i])) {
                return false;
            }
        }
        return true;
    }
};

// RubixCube Solved_Cube = RubixCube();
class Solver{
private: 
	const std::string Moveset[18] = {
      "L", "L'", "L2",
	  "R", "R'", "R2",
      "U", "U'", "U2",
      "D", "D'", "D2",
      "F", "F'", "F2",
      "B", "B'", "B2"
	}; 

    bool matches_target(const RubixCube& cube, const TargetState& target);

	std::string Solve_IDFS(RubixCube given_cube, const TargetState& target_state, int Depth_Limit);
	bool Solve_DFS_fast(RubixCube current_cube, const TargetState& target_state,
	                    std::vector<int>& path, int depth_remaining, int prev_move);

	// One search stage: search for `target` up to depth_limit and append the moves. Returns false only
	// when the target is out of reach; an already-satisfied target is success with no moves.
	bool solve_stage(RubixCube& cube, const TargetState& target, int depth_limit, std::string& all_moves);

	// Speedcuber style: try a short list of known algorithms (each after one of the four D-layer
	// rotations) at most `max_algs` deep until `target` matches. No search tree beyond that.
	bool solve_with_algs(RubixCube& cube, const TargetState& target,
	                           const char* const* algs, int num_algs, int max_algs, std::string& out);
	bool permute_corners(RubixCube& cube, const TargetState& target, std::string& out);
	bool permute_edges(RubixCube& cube, const TargetState& target, std::string& out);

	bool has_white_cross(RubixCube& cube);
	bool has_white_corners(RubixCube& cube);
	bool has_second_layer(RubixCube& cube);
	bool has_bottom_layer(RubixCube& cube);
	std::string solve_white_cross(RubixCube& cube);
	std::string solve_white_corners(RubixCube& cube);
	std::string solve_second_layer(RubixCube& cube);
	std::string solve_bottom_cross(RubixCube& cube);
	std::string solve_bottom_corners(RubixCube& cube);

    // Move index redundancy: groups of 3 (L/L'/L2 = group 0, R/R'/R2 = group 1, ...)
    // Opposite pairs: (0,1)=L/R, (2,3)=U/D, (4,5)=F/B
    bool is_redundant_move_idx(int current_idx, int prev_idx) const {
        if (prev_idx < 0) return false;
        int cur_group = current_idx / 3;
        int prev_group = prev_idx / 3;
        // Same face group: sequences like L followed by L' or L2 are redundant
        if (cur_group == prev_group) return true;
        // Opposite faces commute (L R == R L), so allow only one ordering of the pair:
        // the odd group first (R before L, D before U, B before F). Pruning both
        // orderings would make states that need the pair unreachable.
        if (cur_group % 2 == 0 && prev_group == cur_group + 1) return true;
        return false;
    }

    int dfs_count;

public: 
	//copy for now, might be useful to pass by ref?
	Solver() : dfs_count(0) {}
	// Solver(RubixCube cube){
	// 	Cube = cube;
	// }
	~Solver();
	bool is_Solved(RubixCube cube){
		return cube == RubixCube(); // matching a solved cube
	}
	std::string Solve_Cube(RubixCube &given_cube, int Depth_Limit);
	// Called with the moves of each finished stage while Solve_Cube runs (may be empty).
	std::function<void(const std::string&)> on_stage;
	void visualize_state();
	void scramble();
	RubixCube Apply_Moves(RubixCube &El_Cube, std::string leMoves);
    
    // Getter for DFS count
    int get_dfs_count() const { return dfs_count; }
    
    // Reset DFS count
    void reset_dfs_count() { dfs_count = 0; }
    
    // Public solving methods for individual layers
    std::string Solve_White_Cross(RubixCube& cube, int depth_limit);
    std::string Solve_White_Corners(RubixCube& cube, int depth_limit);
    std::string Solve_Second_Layer(RubixCube& cube, int depth_limit);
    std::string Solve_Bottom_Layer(RubixCube& cube, int depth_limit);
};