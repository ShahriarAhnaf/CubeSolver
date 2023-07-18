#include "Solver.h"
#include <iostream>

Solver::Solver(){}
Solver::~Solver(){}
std::string Solver::Solve_DFS(RubixCube current_cube, std::string Moves, int depth_remaining){
    if(is_Solved(current_cube)){
        return Moves;
    }
    if (depth_remaining == 0) return "";
    // move through all movesets
    for(std::string bruh : Moveset ){
        std::string result = Solve_DFS(
            Apply_Moves(current_cube, bruh.data()),
            Moves + " " + bruh,
            depth_remaining -1
        );
        if(result != "") return result;
    }
}
std::string Solver::Solve_IDFS()
{
    return "bruh";
}

//applys a series of moves
RubixCube Solver::Apply_Moves(RubixCube El_cube, char* leMoves){
    leMoves = strtok(leMoves, " ");
    while(leMoves != NULL){
            if (leMoves == "L"){
                El_cube.L(1);
                }
            else if (leMoves == "LPRIME"){
                El_cube.L_PRIME(1);
                }
            else if (leMoves == "L2"){
                El_cube.L_PRIME(2);
                }
	        else if (leMoves == "R"){
                El_cube.R(1);
                }
            else if (leMoves == "RPRIME"){
                El_cube.R_PRIME(1);
                } 
            else if (leMoves == "R2"){
                El_cube.R_PRIME(2);
                } 
            else if (leMoves == "U"){
                El_cube.U(1);
                } 
            else if (leMoves == "UPRIME"){
                El_cube.U_PRIME(1);
                } 
            else if (leMoves == "U2"){
                El_cube.U(2);
                } 
            else if (leMoves == D:
                El_cube.D(1);
                }  
            else if (leMoves == "DPRIME"){
                El_cube.D_PRIME(1);
                } 
            else if (leMoves == "D2"){
                El_cube.D(2);
                } 
            else if (leMoves == "F"){
                El_cube.F(1);
                }  
            else if (leMoves == "FPRIME"){
                El_cube.F_PRIME(1);
                } 
            else if (leMoves == "F2"){
                El_cube.F(2);
                } 
            else if (leMoves == "B"){
                El_cube.B(1);
                }  
            else if (leMoves == "BPRIME"){
                El_cube.B_PRIME(1);
                }  
            else if (leMoves == "B2"){
                El_cube.B(2);
                } 
            else {
                std::cout << "move not found" << std::endl;
            }       
        leMoves = strtok (NULL, " "); // getting rid of the first/??
    }
    return El_cube;
	}

