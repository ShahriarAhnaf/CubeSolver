#include "Solver.h"
#include <iostream>
#include <sstream>
#include <iterator>

#include <chrono>
#include <thread>

using std::string;
using std::vector;

void Solver::Solve_Cube(RubixCube given_cube){
		if (given_cube == RubixCube()) mvprintw(1,1, "Already Solved!");
        string solved = Solve_DFS(given_cube, "", 1);
        if(solved == "") mvprintw(1,1,"We aint making it out the hood");
        else
            mvprintw(1,1, solved.c_str());
}
Solver::~Solver(){}

// the current cube is implicitly a reference
string Solver::Solve_DFS(RubixCube current_cube, string Moves, int depth_remaining){
    #ifdef STEP_THROUGH_DEBUGGER
        current_cube.draw(9,10);
        mvprintw(2,1, "depth %d grinding move: %s\n\r", depth_remaining, Moves.data());
        refresh();
        while(mvgetch(0,0) != '1'){} //hold the program
    #endif
    if(is_Solved(current_cube))return Moves;
    if (depth_remaining == 0) return "";
    for(const string &bruh : Moveset ){
         // some pass by reference insanity happening while sending by values
         // ApplyMoves seems to change current_cube 
         #ifdef STEP_THROUGH_DEBUGGER
         mvprintw(1,100, "depth %d calling move: %s from %s\n\r", depth_remaining, bruh.data(), Moves.data());
        mvprintw(2,100, "saved Cube");
        mvprintw(20,100, " Cube");
            // Cube.draw(100,5);
            current_cube.draw(100,20);
             refresh();
            while(mvgetch(0,0) != '1'){} //hold the program
         #endif
        string result = Solve_DFS(
            Apply_Moves(current_cube, bruh),
            Moves + " " + bruh,
            depth_remaining -1
        );
        if(result != "") return result;
    }
    return ""; // when all the nodes are done without a result.
}
string Solver::Solve_IDFS()
{
    return "bruh";
}

//applys a series of moves
RubixCube Solver::Apply_Moves(RubixCube &El_cube, string leMoves){
    RubixCube Local_Copy_Cube; // stack solved cube 
    Local_Copy_Cube = El_cube; // supposed to copy over dem values
    vector<string> tokens;
 
    std::istringstream iss(leMoves);
    string s;
    while (iss >> s) {
        tokens.push_back(s);
    }
    // for each move in the string
    for(const string &Move : tokens){
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

