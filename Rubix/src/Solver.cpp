#include "Solver.h"
#include <iostream>
#include <sstream>
#include <iterator>

#include <assert.h>
#include <chrono>
#include <thread>


static long int DFS_count = 0;
// static long int IDFS_count = 0;

// Helper function to check if a cube matches a target state
bool Solver::matches_target(const RubixCube& cube, const TargetState& target) {
    for(int i = 0; i < 6; i++) {
        uint64_t current_face = cube.get_face(i);
        uint64_t target_face = target.faces[i];
        uint64_t mask = target.masks[i];
        
        // Compare only the bits that are not masked (not don't care)
        if((current_face & mask) != (target_face & mask)) {
            return false;
        }
    }
    return true;
}

// void Solver::Solve_Cube(RubixCube &given_cube, int Depth_Limit){
		
//         std::string IDFS_solved = Solve_IDFS(given_cube, Depth_Limit);
//         if(IDFS_solved == "") mvprintw(10,1,"We aint making it out the hood");
//          else {
//             mvprintw(10,1, IDFS_solved.c_str());
//             mvprintw(15,5, "Total iterative Cube State Checks = %ld", DFS_count);   
            
//          }
//         //  if (given_cube == RubixCube()) mvprintw(1,1, "Already Solved!");
//         // string solved = Solve_DFS(given_cube, "", Depth_Limit);

//         // if(solved == "") mvprintw(1,1,"We aint making it out the hood");
//         // else {
//         //     mvprintw(1,1, solved.c_str());
//         //     mvprintw(5,5, "Total Cube State Checks = %ld", DFS_count);   
//         // }
// }
Solver::~Solver(){}

// the current cube is implicitly a reference
std::string Solver::Solve_DFS(RubixCube current_cube, TargetState target_state, std::string Moves, int depth_remaining) {
    if(depth_remaining == 0) {
        if(matches_target(current_cube, target_state)) {
            return Moves;
        }
        return "";
    }
    
    for(const std::string& move : this->Moveset) {
        #ifdef STEP_THROUGH_DEBUGGER
        mvprintw(1,100, "depth %d calling move: %s from %s\n\r", depth_remaining, move.data(), Moves.data());
        mvprintw(2,100, "saved Cube, should match given_Cube");
        mvprintw(20,100, "given_Cube");
        cube_copy.draw(100,5);
        #endif
        
        RubixCube cube_copy = current_cube;
        cube_copy = Apply_Moves(cube_copy, move);
        std::string result = Solve_DFS(cube_copy, target_state, Moves + move + " ", depth_remaining - 1);
        if(result != "") {
            return result;
        }
    }
    return "";
}

//returns "" if answer not found
std::string Solver::Solve_IDFS(RubixCube given_cube, TargetState target_state, int Depth_Limit) {
    // reset counter 
    DFS_count = 0;
    std::string nice = "";
    for(int n = 1; n < Depth_Limit; n++) {
        nice = Solve_DFS(given_cube, target_state, "", n);
        if(nice != "") break; // exit loop when answer is found
    }
    return nice;
}

//applys a series of moves
RubixCube Solver::Apply_Moves(RubixCube &El_cube, std::string leMoves){
    RubixCube Local_Copy_Cube; // stack solved cube 
    Local_Copy_Cube = El_cube; // supposed to copy over dem values
    std::vector<std::string> tokens;
 
    std::istringstream iss(leMoves);
    std::string s;
    while (iss >> s) {
        tokens.push_back(s);
    }
    // for each move in the string
    for(const std::string &Move : tokens){
            if (Move == "L"){
                Local_Copy_Cube.L(1);
                }
            else if (Move == "LPRIME"){
                Local_Copy_Cube.L_PRIME(1);
                }
            else if (Move == "L2"){
                Local_Copy_Cube.L_PRIME(2);
                }
	        else if (Move == "R"){
                Local_Copy_Cube.R(1);
                }
            else if (Move == "RPRIME"){
                Local_Copy_Cube.R_PRIME(1);
                } 
            else if (Move == "R2"){
                Local_Copy_Cube.R_PRIME(2);
                } 
            else if (Move == "U"){
                Local_Copy_Cube.U(1);
                } 
            else if (Move == "UPRIME"){
                Local_Copy_Cube.U_PRIME(1);
                } 
            else if (Move == "U2"){
                Local_Copy_Cube.U(2);
                } 
            else if (Move == "D"){
                Local_Copy_Cube.D(1);
                }  
            else if (Move == "DPRIME"){
                Local_Copy_Cube.D_PRIME(1);
                } 
            else if (Move == "D2"){
                Local_Copy_Cube.D(2);
                } 
            else if (Move == "F"){
                Local_Copy_Cube.F(1);
                }  
            else if (Move == "FPRIME"){
                Local_Copy_Cube.F_PRIME(1);
                } 
            else if (Move == "F2"){
                Local_Copy_Cube.F(2);
                } 
            else if (Move == "B"){
                Local_Copy_Cube.B(1);
                }  
            else if (Move == "BPRIME"){
                Local_Copy_Cube.B_PRIME(1);
                }  
            else if (Move == "B2"){
                Local_Copy_Cube.B(2);
                } 
            else {
                std::cout << "move not found" << std::endl;
            }       
    }
    return Local_Copy_Cube; // to save the original cube
	}

// Helper function to check if a face has a white cross
bool Solver::has_white_cross(RubixCube& cube) {
    // Check if the top face (face 0) has white edges in the correct positions
    // and if they match with their adjacent center colors
    uint64_t top_face = cube.get_face(FACE_UP);
    uint64_t front_face = cube.get_face(FACE_FRONT);
    uint64_t right_face = cube.get_face(FACE_RIGHT);
    uint64_t back_face = cube.get_face(FACE_BACK);
    uint64_t left_face = cube.get_face(FACE_LEFT);

    // Check each edge position
    bool edge1 = GET_COLOR(top_face, TOP) == WHITE && 
                 GET_COLOR(front_face, TOP) == GREEN;
    bool edge3 = GET_COLOR(top_face, RIGHT) == WHITE && 
                 GET_COLOR(right_face, TOP) == RED;
    bool edge5 = GET_COLOR(top_face, BOTTOM) == WHITE && 
                 GET_COLOR(back_face, TOP) == BLUE;
    bool edge7 = GET_COLOR(top_face, LEFT) == WHITE && 
                 GET_COLOR(left_face, TOP) == ORANGE;

    return edge1 && edge3 && edge5 && edge7;
}

// Helper function to check if white face corners are solved
bool Solver::has_white_corners(RubixCube& cube) {
    uint64_t top_face = cube.get_face(FACE_UP);
    uint64_t front_face = cube.get_face(FACE_FRONT);
    uint64_t right_face = cube.get_face(FACE_RIGHT);
    uint64_t back_face = cube.get_face(FACE_BACK);
    uint64_t left_face = cube.get_face(FACE_LEFT);

    // Check each corner position
    bool corner0 = GET_COLOR(top_face, TOP_LEFT) == WHITE && 
                   GET_COLOR(front_face, TOP_LEFT) == GREEN &&
                   GET_COLOR(left_face, TOP_LEFT) == ORANGE;
    
    bool corner2 = GET_COLOR(top_face, TOP_RIGHT) == WHITE && 
                   GET_COLOR(front_face, TOP_RIGHT) == GREEN &&
                   GET_COLOR(right_face, TOP_LEFT) == RED;
    
    bool corner4 = GET_COLOR(top_face, BOTTOM_RIGHT) == WHITE && 
                   GET_COLOR(back_face, TOP_RIGHT) == BLUE &&
                   GET_COLOR(right_face, TOP_RIGHT) == RED;
    
    bool corner6 = GET_COLOR(top_face, BOTTOM_LEFT) == WHITE && 
                   GET_COLOR(back_face, TOP_LEFT) == BLUE &&
                   GET_COLOR(left_face, TOP_RIGHT) == ORANGE;

    return corner0 && corner2 && corner4 && corner6;
}

// Helper function to check if second layer is solved
bool Solver::has_second_layer(RubixCube& cube) {
    uint64_t front_face = cube.get_face(FACE_FRONT);
    uint64_t right_face = cube.get_face(FACE_RIGHT);
    uint64_t back_face = cube.get_face(FACE_BACK);
    uint64_t left_face = cube.get_face(FACE_LEFT);

    // Check each edge in the second layer
    bool front_right = GET_COLOR(front_face, RIGHT) == GREEN && 
                       GET_COLOR(right_face, LEFT) == RED;
    
    bool right_back = GET_COLOR(right_face, RIGHT) == RED && 
                      GET_COLOR(back_face, LEFT) == BLUE;
    
    bool back_left = GET_COLOR(back_face, RIGHT) == BLUE && 
                     GET_COLOR(left_face, LEFT) == ORANGE;
    
    bool left_front = GET_COLOR(left_face, RIGHT) == ORANGE && 
                      GET_COLOR(front_face, LEFT) == GREEN;

    return front_right && right_back && back_left && left_front;
}

// Helper function to check if bottom layer is solved
bool Solver::has_bottom_layer(RubixCube& cube) {
    uint64_t bottom_face = cube.get_face(FACE_BOTTOM);
    uint64_t front_face = cube.get_face(FACE_FRONT);
    uint64_t right_face = cube.get_face(FACE_RIGHT);
    uint64_t back_face = cube.get_face(FACE_BACK);
    uint64_t left_face = cube.get_face(FACE_LEFT);

    // Check if bottom face is all yellow
    bool all_yellow = true;
    for(int i = 0; i < 8; i++) {
        if(GET_COLOR(bottom_face, i) != YELLOW) {
            all_yellow = false;
            break;
        }
    }

    // Check if corners match with their adjacent colors
    bool corner0 = GET_COLOR(front_face, BOTTOM_RIGHT) == GREEN && 
                   GET_COLOR(right_face, BOTTOM_LEFT) == RED;
    
    bool corner2 = GET_COLOR(front_face, BOTTOM_LEFT) == GREEN && 
                   GET_COLOR(left_face, BOTTOM_RIGHT) == ORANGE;
    
    bool corner4 = GET_COLOR(back_face, BOTTOM_LEFT) == BLUE && 
                   GET_COLOR(left_face, BOTTOM_LEFT) == ORANGE;
    
    bool corner6 = GET_COLOR(back_face, BOTTOM_RIGHT) == BLUE && 
                   GET_COLOR(right_face, BOTTOM_RIGHT) == RED;

    return all_yellow && corner0 && corner2 && corner4 && corner6;
}

void Solver::Solve_Cube(RubixCube &given_cube, int Depth_Limit) {
    // Step 1: Solve white cross
    TargetState white_cross_target;
    // Set up white cross target
    for(int i = 0; i < 6; i++) {
        if(i == FACE_UP) {
            // Keep white center and edges
            for(int j = 0; j < 8; j++) {
                if(j == TOP || j == RIGHT || j == BOTTOM || j == LEFT) {
                    SET_COLOR(white_cross_target.faces[i], j, WHITE);
                }
            }
        } else {
            // Keep only the top edge of each face
            uint8_t color = i + 1;
            SET_COLOR(white_cross_target.faces[i], TOP, color); // Set center color
        }
    }
    
    // Set bottom face and all other stickers as don't care
    white_cross_target.set_face_dont_care(FACE_BOTTOM);
    for(int i = 0; i < 6; i++) {
        if(i != FACE_UP) {
            for(int j = 0; j < 8; j++) {
                if(j != TOP) {
                    white_cross_target.set_dont_care(i, j);
                }
            }
        }
    }
    white_cross_target.set_face_dont_care(FACE_BOTTOM);
    
    std::string cross_moves = Solve_IDFS(given_cube, white_cross_target, Depth_Limit);
    if(cross_moves != "") {
        given_cube = Apply_Moves(given_cube, cross_moves);
    }

    // Step 2: Complete white face corners
    TargetState white_corners_target;
    
    // Set up white corners target
    for(int i = 0; i < 6; i++) {
        if(i == FACE_UP) {
            // Keep white face and corners
            for(int j = 0; j < 8; j++) {
                if(j == TOP_LEFT || j == TOP_RIGHT || j == BOTTOM_LEFT || j == BOTTOM_RIGHT) {
                    SET_COLOR(white_corners_target.faces[i], j, WHITE);
                }
            }
        } else {
            // Keep only the corners of each face
            uint8_t color = i + 1;
            SET_COLOR(white_corners_target.faces[i], TOP_LEFT, color);
            SET_COLOR(white_corners_target.faces[i], TOP_RIGHT, color);
            SET_COLOR(white_corners_target.faces[i], BOTTOM_LEFT, color);
            SET_COLOR(white_corners_target.faces[i], BOTTOM_RIGHT, color);
        }
    }
    
    // Set bottom face and middle edges as don't care
    white_corners_target.set_face_dont_care(FACE_BOTTOM);
    for(int i = 0; i < 6; i++) {
        if(i != FACE_UP) {
            for(int j = 0; j < 8; j++) {
                if(j != TOP_LEFT && j != TOP_RIGHT && j != BOTTOM_LEFT && j != BOTTOM_RIGHT) {
                    white_corners_target.set_dont_care(i, j);
                }
            }
        }
    }
    
    std::string corner_moves = Solve_IDFS(given_cube, white_corners_target, Depth_Limit);
    if(corner_moves != "") {
        given_cube = Apply_Moves(given_cube, corner_moves);
    }

    // Step 3: Solve second layer
    TargetState second_layer_target;
    
    // Set up second layer target
    for(int i = 0; i < 6; i++) {
        if(i == FACE_UP) {
            // Keep entire top face
            for(int j = 0; j < 8; j++) {
                SET_COLOR(second_layer_target.faces[i], j, WHITE);
            }
        } else {
            // For side faces, keep only the middle edges
            uint8_t color = i + 1;
            SET_COLOR(second_layer_target.faces[i], LEFT, color);
            SET_COLOR(second_layer_target.faces[i], RIGHT, color);
        }
    }
    
    // Set bottom face as don't care
    second_layer_target.set_face_dont_care(FACE_BOTTOM);

    // Set all bottom layer positions as don't care for all faces except FACE_UP
    for(int i = 0; i < 6; i++) {
        if(i != FACE_UP) {
            second_layer_target.set_dont_care(i, BOTTOM_LEFT);
            second_layer_target.set_dont_care(i, BOTTOM_RIGHT);
            second_layer_target.set_dont_care(i, BOTTOM);
        }
    }
    
    std::string second_layer_moves = Solve_IDFS(given_cube, second_layer_target, Depth_Limit);
    if(second_layer_moves != "") {
        given_cube = Apply_Moves(given_cube, second_layer_moves);
    }

    // Step 4: Solve bottom layer
    TargetState solved_target;
    RubixCube solved_cube;
    // Create a solved cube state
    for(int i = 0; i < 6; i++) {
        solved_target.faces[i] = solved_cube.get_face(i);
    }
    
    std::string bottom_layer_moves = Solve_IDFS(given_cube, solved_target, Depth_Limit);
    if(bottom_layer_moves != "") {
        given_cube = Apply_Moves(given_cube, bottom_layer_moves);
    }
}

// These functions need to be implemented with the actual solving algorithms
std::string Solver::solve_white_cross(RubixCube& cube) {
    std::string moves = "";
    
    // Find white edges and move them to the top face
    uint64_t front_face = cube.get_face(FACE_FRONT);
    uint64_t right_face = cube.get_face(FACE_RIGHT);
    uint64_t back_face = cube.get_face(FACE_BACK);
    uint64_t left_face = cube.get_face(FACE_LEFT);
    uint64_t bottom_face = cube.get_face(FACE_BOTTOM);
    
    // Check front face
    if(GET_COLOR(front_face, TOP) == WHITE) {
        // Edge is already in position
    }
    else if(GET_COLOR(front_face, RIGHT) == WHITE) {
        moves += "F U R UPRIME FPRIME ";
        cube.F(1); cube.U(1); cube.R(1); cube.U_PRIME(1); cube.F_PRIME(1);
    }
    else if(GET_COLOR(front_face, BOTTOM) == WHITE) {
        moves += "F2 ";
        cube.F(2);
    }
    else if(GET_COLOR(front_face, LEFT) == WHITE) {
        moves += "FPRIME U LPRIME UPRIME F ";
        cube.F_PRIME(1); cube.U(1); cube.L_PRIME(1); cube.U_PRIME(1); cube.F(1);
    }

    // Check right face
    if(GET_COLOR(right_face, TOP) == WHITE) {
        moves += "R U F UPRIME RPRIME ";
        cube.R(1); cube.U(1); cube.F(1); cube.U_PRIME(1); cube.R_PRIME(1);
    }
    else if(GET_COLOR(right_face, RIGHT) == WHITE) {
        // Edge is already in position
    }
    else if(GET_COLOR(right_face, BOTTOM) == WHITE) {
        moves += "R2 ";
        cube.R(2);
    }
    else if(GET_COLOR(right_face, LEFT) == WHITE) {
        moves += "RPRIME U BPRIME UPRIME R ";
        cube.R_PRIME(1); cube.U(1); cube.B_PRIME(1); cube.U_PRIME(1); cube.R(1);
    }

    // Check back face
    if(GET_COLOR(back_face, TOP) == WHITE) {
        moves += "B U R UPRIME BPRIME ";
        cube.B(1); cube.U(1); cube.R(1); cube.U_PRIME(1); cube.B_PRIME(1);
    }
    else if(GET_COLOR(back_face, RIGHT) == WHITE) {
        moves += "BPRIME U LPRIME UPRIME B ";
        cube.B_PRIME(1); cube.U(1); cube.L_PRIME(1); cube.U_PRIME(1); cube.B(1);
    }
    else if(GET_COLOR(back_face, BOTTOM) == WHITE) {
        moves += "B2 ";
        cube.B(2);
    }
    else if(GET_COLOR(back_face, LEFT) == WHITE) {
        // Edge is already in position
    }

    // Check left face
    if(GET_COLOR(left_face, TOP) == WHITE) {
        moves += "L U B UPRIME LPRIME ";
        cube.L(1); cube.U(1); cube.B(1); cube.U_PRIME(1); cube.L_PRIME(1);
    }
    else if(GET_COLOR(left_face, RIGHT) == WHITE) {
        moves += "LPRIME U FPRIME UPRIME L ";
        cube.L_PRIME(1); cube.U(1); cube.F_PRIME(1); cube.U_PRIME(1); cube.L(1);
    }
    else if(GET_COLOR(left_face, BOTTOM) == WHITE) {
        moves += "L2 ";
        cube.L(2);
    }
    else if(GET_COLOR(left_face, LEFT) == WHITE) {
        // Edge is already in position
    }

    // Check bottom face
    for(int i = 0; i < 4; i++) {
        if(GET_COLOR(bottom_face, i * 2) == WHITE) {
            switch(i) {
                case 0: // Front edge
                    moves += "F2 ";
                    cube.F(2);
                    break;
                case 1: // Right edge
                    moves += "R2 ";
                    cube.R(2);
                    break;
                case 2: // Back edge
                    moves += "B2 ";
                    cube.B(2);
                    break;
                case 3: // Left edge
                    moves += "L2 ";
                    cube.L(2);
                    break;
            }
        }
    }

    return moves;
}

std::string Solver::solve_white_corners(RubixCube& cube) {
    std::string moves = "";
    
    // Find white corners and move them to the correct position
    // We'll check each face for white corners and move them to the correct position
    
    // Check front face (face 2)
    uint64_t front_face = cube.get_face(FACE_FRONT);
    uint64_t right_face = cube.get_face(FACE_RIGHT);
    uint64_t top_face = cube.get_face(FACE_UP);
    
    // Check top-right corner
    if(GET_COLOR(front_face, TOP_LEFT) == WHITE) {
        // Corner is in position, check if it needs to be oriented
        if(GET_COLOR(top_face, TOP_LEFT) != WHITE) {
            moves += "R U RPRIME ";
            cube.R(1); cube.U(1); cube.R_PRIME(1);
        }
    }
    // Check bottom-right corner
    else if(GET_COLOR(front_face, BOTTOM_RIGHT) == WHITE) {
        moves += "R U RPRIME ";
        cube.R(1); cube.U(1); cube.R_PRIME(1);
    }
    // Check bottom-left corner
    else if(GET_COLOR(front_face, BOTTOM_LEFT) == WHITE) {
        moves += "LPRIME UPRIME L ";
        cube.L_PRIME(1); cube.U_PRIME(1); cube.L(1);
    }
    
    // Check right face (face 3)
    uint64_t back_face = cube.get_face(FACE_BACK);
    
    // Check top-right corner
    if(GET_COLOR(right_face, TOP_LEFT) == WHITE) {
        // Corner is in position, check if it needs to be oriented
        if(GET_COLOR(top_face, TOP_RIGHT) != WHITE) {
            moves += "B U BPRIME ";
            cube.B(1); cube.U(1); cube.B_PRIME(1);
        }
    }
    // Check bottom-right corner
    else if(GET_COLOR(right_face, BOTTOM_RIGHT) == WHITE) {
        moves += "B U BPRIME ";
        cube.B(1); cube.U(1); cube.B_PRIME(1);
    }
    // Check bottom-left corner
    else if(GET_COLOR(right_face, BOTTOM_LEFT) == WHITE) {
        moves += "RPRIME UPRIME R ";
        cube.R_PRIME(1); cube.U_PRIME(1); cube.R(1);
    }
    
    // Check back face (face 4)
    uint64_t left_face = cube.get_face(FACE_LEFT);
    
    // Check top-right corner
    if(GET_COLOR(back_face, TOP_LEFT) == WHITE) {
        // Corner is in position, check if it needs to be oriented
        if(GET_COLOR(top_face, BOTTOM_RIGHT) != WHITE) {
            moves += "L U LPRIME ";
            cube.L(1); cube.U(1); cube.L_PRIME(1);
        }
    }
    // Check bottom-right corner
    else if(GET_COLOR(back_face, BOTTOM_RIGHT) == WHITE) {
        moves += "L U LPRIME ";
        cube.L(1); cube.U(1); cube.L_PRIME(1);
    }
    // Check bottom-left corner
    else if(GET_COLOR(back_face, BOTTOM_LEFT) == WHITE) {
        moves += "BPRIME UPRIME B ";
        cube.B_PRIME(1); cube.U_PRIME(1); cube.B(1);
    }
    
    // Check left face (face 1)
    // Check top-right corner
    if(GET_COLOR(left_face, TOP_LEFT) == WHITE) {
        // Corner is in position, check if it needs to be oriented
        if(GET_COLOR(top_face, TOP_LEFT) != WHITE) {
            moves += "F U FPRIME ";
            cube.F(1); cube.U(1); cube.F_PRIME(1);
        }
    }
    // Check bottom-right corner
    else if(GET_COLOR(left_face, BOTTOM_RIGHT) == WHITE) {
        moves += "F U FPRIME ";
        cube.F(1); cube.U(1); cube.F_PRIME(1);
    }
    // Check bottom-left corner
    else if(GET_COLOR(left_face, BOTTOM_LEFT) == WHITE) {
        moves += "LPRIME UPRIME L ";
        cube.L_PRIME(1); cube.U_PRIME(1); cube.L(1);
    }
    
    // Check bottom face (face 5)
    uint64_t bottom_face = cube.get_face(FACE_BOTTOM);
    
    // Check each corner
    for(int i = 0; i < 4; i++) {
        if(GET_COLOR(bottom_face, i * 2) == WHITE) {
            // Move the corner to the top
            switch(i) {
                case 0: // Front-right corner
                    moves += "R U2 RPRIME ";
                    cube.R(1); cube.U(2); cube.R_PRIME(1);
                    break;
                case 1: // Back-right corner
                    moves += "B U2 BPRIME ";
                    cube.B(1); cube.U(2); cube.B_PRIME(1);
                    break;
                case 2: // Back-left corner
                    moves += "L U2 LPRIME ";
                    cube.L(1); cube.U(2); cube.L_PRIME(1);
                    break;
                case 3: // Front-left corner
                    moves += "F U2 FPRIME ";
                    cube.F(1); cube.U(2); cube.F_PRIME(1);
                    break;
            }
        }
    }
    
    return moves;
}

std::string Solver::solve_second_layer(RubixCube& cube) {
    std::string moves = "";
    
    // Find edges that belong in the second layer and move them to the correct position
    // We'll check each face for edges that need to be moved to the second layer
    
    // Check front face (face 2)
    uint64_t front_face = cube.get_face(FACE_FRONT);
    uint64_t right_face = cube.get_face(FACE_RIGHT);
    uint64_t left_face = cube.get_face(FACE_LEFT);
    
    // Check right edge
    if(GET_COLOR(front_face, RIGHT) != GREEN) {
        // Edge needs to be moved to the second layer
        if(GET_COLOR(right_face, LEFT) == GREEN) {
            // Edge is on the right face, move it to the front
            moves += "U R UPRIME RPRIME UPRIME FPRIME U F ";
            cube.U(1); cube.R(1); cube.U_PRIME(1); cube.R_PRIME(1); 
            cube.U_PRIME(1); cube.F_PRIME(1); cube.U(1); cube.F(1);
        }
        else if(GET_COLOR(left_face, RIGHT) == GREEN) {
            // Edge is on the left face, move it to the front
            moves += "UPRIME LPRIME U L U F UPRIME FPRIME ";
            cube.U_PRIME(1); cube.L_PRIME(1); cube.U(1); cube.L(1);
            cube.U(1); cube.F(1); cube.U_PRIME(1); cube.F_PRIME(1);
        }
    }
    
    // Check left edge
    if(GET_COLOR(front_face, LEFT) != GREEN) {
        // Edge needs to be moved to the second layer
        if(GET_COLOR(right_face, RIGHT) == GREEN) {
            // Edge is on the right face, move it to the front
            moves += "U R UPRIME RPRIME UPRIME FPRIME U F ";
            cube.U(1); cube.R(1); cube.U_PRIME(1); cube.R_PRIME(1);
            cube.U_PRIME(1); cube.F_PRIME(1); cube.U(1); cube.F(1);
        }
        else if(GET_COLOR(left_face, LEFT) == GREEN) {
            // Edge is on the left face, move it to the front
            moves += "UPRIME LPRIME U L U F UPRIME FPRIME ";
            cube.U_PRIME(1); cube.L_PRIME(1); cube.U(1); cube.L(1);
            cube.U(1); cube.F(1); cube.U_PRIME(1); cube.F_PRIME(1);
        }
    }
    
    // Check right face (face 3)
    uint64_t back_face = cube.get_face(FACE_BACK);
    
    // Check right edge
    if(GET_COLOR(right_face, RIGHT) != RED) {
        // Edge needs to be moved to the second layer
        if(GET_COLOR(front_face, RIGHT) == RED) {
            // Edge is on the front face, move it to the right
            moves += "U F UPRIME FPRIME UPRIME RPRIME U R ";
            cube.U(1); cube.F(1); cube.U_PRIME(1); cube.F_PRIME(1);
            cube.U_PRIME(1); cube.R_PRIME(1); cube.U(1); cube.R(1);
        }
        else if(GET_COLOR(back_face, LEFT) == RED) {
            // Edge is on the back face, move it to the right
            moves += "UPRIME BPRIME U B U R UPRIME RPRIME ";
            cube.U_PRIME(1); cube.B_PRIME(1); cube.U(1); cube.B(1);
            cube.U(1); cube.R(1); cube.U_PRIME(1); cube.R_PRIME(1);
        }
    }
    
    // Check left face (face 1)
    // Check right edge
    if(((left_face & 0x0000FF0000000000) >> 40) != ORANGE) {
        // Edge needs to be moved to the second layer
        if(((front_face & 0x0000FF0000000000) >> 40) == ORANGE) {
            // Edge is on the front face, move it to the left
            moves += "U F UPRIME FPRIME UPRIME LPRIME U L ";
            cube.U(1); cube.F(1); cube.U_PRIME(1); cube.F_PRIME(1);
            cube.U_PRIME(1); cube.L_PRIME(1); cube.U(1); cube.L(1);
        }
        else if(((back_face & 0x0000FF0000000000) >> 40) == ORANGE) {
            // Edge is on the back face, move it to the left
            moves += "UPRIME BPRIME U B U L UPRIME LPRIME ";
            cube.U_PRIME(1); cube.B_PRIME(1); cube.U(1); cube.B(1);
            cube.U(1); cube.L(1); cube.U_PRIME(1); cube.L_PRIME(1);
        }
    }
    
    // Check left edge
    if(((left_face & 0x000000000000FF00) >> 8) != ORANGE) {
        // Edge needs to be moved to the second layer
        if(((front_face & 0x000000000000FF00) >> 8) == ORANGE) {
            // Edge is on the front face, move it to the left
            moves += "U F UPRIME FPRIME UPRIME LPRIME U L ";
            cube.U(1); cube.F(1); cube.U_PRIME(1); cube.F_PRIME(1);
            cube.U_PRIME(1); cube.L_PRIME(1); cube.U(1); cube.L(1);
        }
        else if(((back_face & 0x000000000000FF00) >> 8) == ORANGE) {
            // Edge is on the back face, move it to the left
            moves += "UPRIME BPRIME U B U L UPRIME LPRIME ";
            cube.U_PRIME(1); cube.B_PRIME(1); cube.U(1); cube.B(1);
            cube.U(1); cube.L(1); cube.U_PRIME(1); cube.L_PRIME(1);
        }
    }
    
    return moves;
}

std::string Solver::solve_bottom_cross(RubixCube& cube) {
    std::string moves = "";
    
    uint64_t front_face = cube.get_face(FACE_FRONT);
    uint64_t right_face = cube.get_face(FACE_RIGHT);
    uint64_t back_face = cube.get_face(FACE_BACK);
    uint64_t left_face = cube.get_face(FACE_LEFT);
    uint64_t bottom_face = cube.get_face(FACE_BOTTOM);
    
    // Check each face for yellow edges
    if(GET_COLOR(front_face, BOTTOM) == YELLOW) {
        moves += "F2 ";
        cube.F(2);
    }
    
    if(GET_COLOR(right_face, BOTTOM) == YELLOW) {
        moves += "R2 ";
        cube.R(2);
    }
    
    if(GET_COLOR(back_face, BOTTOM) == YELLOW) {
        moves += "B2 ";
        cube.B(2);
    }
    
    if(GET_COLOR(left_face, BOTTOM) == YELLOW) {
        moves += "L2 ";
        cube.L(2);
    }
    
    // Now we need to orient the yellow edges correctly
    // We'll use the "fish" algorithm to orient the edges
    
    // Check if we have a line or an L shape
    int yellow_edges = 0;
    bool edges[4] = {false, false, false, false};
    
    // Check each edge on the bottom face
    for(int i = 0; i < 4; i++) {
        if(GET_COLOR(bottom_face, i * 2) == YELLOW) {
            yellow_edges++;
            edges[i] = true;
        }
    }
    
    // If we have 2 yellow edges, we need to check if they form a line or an L
    if(yellow_edges == 2) {
        // Check if they form a line
        if((edges[0] && edges[2]) || (edges[1] && edges[3])) {
            // We have a line, rotate the cube to get the line horizontal
            if(edges[0] && edges[2]) {
                moves += "U ";
                cube.U(1);
            }
            
            // Apply the fish algorithm
            moves += "F R U RPRIME UPRIME FPRIME ";
            cube.F(1); cube.R(1); cube.U(1); cube.R_PRIME(1); 
            cube.U_PRIME(1); cube.F_PRIME(1);
        }
        else {
            // We have an L shape, rotate the cube to get the L in the correct position
            if(edges[0] && edges[1]) {
                moves += "U ";
                cube.U(1);
            }
            else if(edges[1] && edges[2]) {
                moves += "U2 ";
                cube.U(2);
            }
            else if(edges[2] && edges[3]) {
                moves += "UPRIME ";
                cube.U_PRIME(1);
            }
            
            // Apply the fish algorithm
            moves += "F R U RPRIME UPRIME FPRIME ";
            cube.F(1); cube.R(1); cube.U(1); cube.R_PRIME(1); 
            cube.U_PRIME(1); cube.F_PRIME(1);
        }
    }
    // If we have no yellow edges, we need to create a line
    else if(yellow_edges == 0) {
        // Apply the fish algorithm twice
        moves += "F R U RPRIME UPRIME FPRIME ";
        cube.F(1); cube.R(1); cube.U(1); cube.R_PRIME(1); 
        cube.U_PRIME(1); cube.F_PRIME(1);
        
        moves += "U2 ";
        cube.U(2);
        
        moves += "F R U RPRIME UPRIME FPRIME ";
        cube.F(1); cube.R(1); cube.U(1); cube.R_PRIME(1); 
        cube.U_PRIME(1); cube.F_PRIME(1);
    }
    
    return moves;
}

std::string Solver::solve_bottom_corners(RubixCube& cube) {
    std::string moves = "";
    
    uint64_t front_face = cube.get_face(FACE_FRONT);
    uint64_t right_face = cube.get_face(FACE_RIGHT);
    uint64_t back_face = cube.get_face(FACE_BACK);
    uint64_t left_face = cube.get_face(FACE_LEFT);
    uint64_t bottom_face = cube.get_face(FACE_BOTTOM);
    
    // Check if corners are in the correct position
    bool corners_correct = true;
    bool corners_oriented = true;
    
    // Check front-right corner
    if(GET_COLOR(front_face, BOTTOM_RIGHT) != GREEN || 
       GET_COLOR(right_face, BOTTOM_LEFT) != RED) {
        corners_correct = false;
    }
    if(GET_COLOR(bottom_face, TOP_LEFT) != YELLOW) {
        corners_oriented = false;
    }
    
    // Check back-right corner
    if(GET_COLOR(right_face, BOTTOM_RIGHT) != RED || 
       GET_COLOR(back_face, BOTTOM_LEFT) != BLUE) {
        corners_correct = false;
    }
    if(GET_COLOR(bottom_face, TOP_RIGHT) != YELLOW) {
        corners_oriented = false;
    }
    
    // Check back-left corner
    if(GET_COLOR(back_face, BOTTOM_RIGHT) != BLUE || 
       GET_COLOR(left_face, BOTTOM_LEFT) != ORANGE) {
        corners_correct = false;
    }
    if(GET_COLOR(bottom_face, BOTTOM_RIGHT) != YELLOW) {
        corners_oriented = false;
    }
    
    // Check front-left corner
    if(GET_COLOR(left_face, BOTTOM_RIGHT) != ORANGE || 
       GET_COLOR(front_face, BOTTOM_LEFT) != GREEN) {
        corners_correct = false;
    }
    if(GET_COLOR(bottom_face, BOTTOM_LEFT) != YELLOW) {
        corners_oriented = false;
    }
    
    // If corners are not in the correct position, we need to position them
    if(!corners_correct) {
        // Use the "sexy move" algorithm to position corners
        moves += "R U RPRIME UPRIME ";
        cube.R(1); cube.U(1); cube.R_PRIME(1); cube.U_PRIME(1);
        
        // Check if we need to rotate the cube
        if(GET_COLOR(front_face, BOTTOM_RIGHT) == GREEN && 
           GET_COLOR(right_face, BOTTOM_LEFT) == RED) {
            moves += "U ";
            cube.U(1);
        }
        else if(GET_COLOR(right_face, BOTTOM_RIGHT) == RED && 
                GET_COLOR(back_face, BOTTOM_LEFT) == BLUE) {
            moves += "U2 ";
            cube.U(2);
        }
        else if(GET_COLOR(back_face, BOTTOM_RIGHT) == BLUE && 
                GET_COLOR(left_face, BOTTOM_LEFT) == ORANGE) {
            moves += "UPRIME ";
            cube.U_PRIME(1);
        }
    }
    
    // If corners are not oriented correctly, we need to orient them
    if(!corners_oriented) {
        // Use the "sune" algorithm to orient corners
        moves += "R U RPRIME U R U2 RPRIME ";
        cube.R(1); cube.U(1); cube.R_PRIME(1); cube.U(1); 
        cube.R(1); cube.U(2); cube.R_PRIME(1);
        
        // Check if we need to rotate the cube
        if(GET_COLOR(bottom_face, TOP_LEFT) != YELLOW) {
            moves += "U ";
            cube.U(1);
        }
        else if(GET_COLOR(bottom_face, TOP_RIGHT) != YELLOW) {
            moves += "U2 ";
            cube.U(2);
        }
        else if(GET_COLOR(bottom_face, BOTTOM_LEFT) != YELLOW) {
            moves += "UPRIME ";
            cube.U_PRIME(1);
        }
    }
    
    return moves;
}

