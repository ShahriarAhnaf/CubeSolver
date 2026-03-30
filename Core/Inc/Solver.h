#pragma once
#include "Cube.h"
#include <string>
#include <vector>

struct TargetState {
    uint64_t faces[6];
    uint64_t masks[6];

    TargetState() {
        for(int i = 0; i < 6; i++) {
            faces[i] = 0;
            masks[i] = 0xFFFFFFFFFFFFFFFF;
        }
    }

    TargetState(uint64_t face_values[6]) {
        for(int i = 0; i < 6; i++) {
            faces[i] = face_values[i];
            masks[i] = 0xFFFFFFFFFFFFFFFF;
        }
    }

    void set_face(uint8_t face_index, uint64_t face_value, uint64_t face_mask) {
        faces[face_index] = face_value;
        masks[face_index] = face_mask;
    }

    void set_dont_care(uint8_t face_index, uint8_t position) {
        SET_COLOR(masks[face_index], position, 0x00U);
    }

    void set_face_dont_care(uint8_t face_index) {
        masks[face_index] = 0;
    }

    void set_relevant(uint8_t face_index, uint8_t position) {
        SET_COLOR(masks[face_index], position, 0xFFU);
    }

    void set_face_relevant(uint8_t face_index) {
        masks[face_index] = 0xFFFFFFFFFFFFFFFF;
    }

    bool matches_cube(const RubixCube& cube) const {
        for(int i = 0; i < 6; i++) {
            uint64_t current_face = cube.get_face(i);
            if((current_face & masks[i]) != (faces[i] & masks[i])) {
                return false;
            }
        }
        return true;
    }
};

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

	// Heuristic solve methods
	std::string solve_white_cross(RubixCube& cube);
	std::string solve_white_corners(RubixCube& cube);
	std::string solve_second_layer(RubixCube& cube);
	std::string solve_bottom_cross(RubixCube& cube);
	std::string solve_yellow_face(RubixCube& cube);
	std::string solve_corner_permutation(RubixCube& cube);
	std::string solve_edge_permutation(RubixCube& cube);

    bool is_redundant_move_idx(int current_idx, int prev_idx) const {
        if (prev_idx < 0) return false;
        int cur_group = current_idx / 3;
        int prev_group = prev_idx / 3;
        if (cur_group == prev_group) return true;
        if (cur_group % 2 == 0 && prev_group == cur_group + 1) return true;
        return false;
    }

    int dfs_count;

public:
	Solver() : dfs_count(0) {}
	~Solver();
	bool is_Solved(RubixCube cube){
		return cube == RubixCube();
	}
	std::string Solve_Cube(RubixCube &given_cube, int Depth_Limit);
	RubixCube Apply_Moves(RubixCube &El_Cube, std::string leMoves);

    int get_dfs_count() const { return dfs_count; }
    void reset_dfs_count() { dfs_count = 0; }
};
