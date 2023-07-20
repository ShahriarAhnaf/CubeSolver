#include "Solver.h"
#include <iostream>
#include <sstream>
#include <iterator>

#include <chrono>
#include <thread>

using std::string;
using std::vector;

void Solver::Solve_Cube(RubixCube given_cube){
		Cube = given_cube;
        string solved = Solve_DFS(Cube, "", 1);
		
        if(solved == "") mvprintw(1,1,"We aint making it out the hood");
        else
            mvprintw(1,1, solved.c_str());
}
Solver::~Solver(){}
string Solver::Solve_DFS(RubixCube current_cube, string Moves, int depth_remaining){
    // sleep(1);//chill out the algorithm
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    current_cube.draw(9,10);
    #ifdef STEP_THROUGH_DEBUGGER
        while(mvgetch(0,0) != '1'){} //hold the program
        refresh();
    #endif
    //  mvprintw(2,1, "grinding DFS");
    if(is_Solved(current_cube)){
        return Moves;
    }
    if (depth_remaining == 0) return "";
    // move through all movesets
    for(string bruh : Moveset ){
        mvprintw(2,1, "depth %d grinding move: %s\n", depth_remaining, bruh.data());
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
RubixCube Solver::Apply_Moves(RubixCube El_cube, string leMoves){
   
    vector<string> tokens;
 
    std::istringstream iss(leMoves);
    string s;
    while (iss >> s) {
        tokens.push_back(s);
    }
    // for each move in the string
    for(const string &Move : tokens){
            if (Move == "L"){
                El_cube.L(1);
                }
            else if (Move == "LPRIME"){
                El_cube.L_PRIME(1);
                }
            else if (Move == "L2"){
                El_cube.L_PRIME(2);
                }
	        else if (Move == "R"){
                El_cube.R(1);
                }
            else if (Move == "RPRIME"){
                El_cube.R_PRIME(1);
                } 
            else if (Move == "R2"){
                El_cube.R_PRIME(2);
                } 
            else if (Move == "U"){
                El_cube.U(1);
                } 
            else if (Move == "UPRIME"){
                El_cube.U_PRIME(1);
                } 
            else if (Move == "U2"){
                El_cube.U(2);
                } 
            else if (Move == "D"){
                El_cube.D(1);
                }  
            else if (Move == "DPRIME"){
                El_cube.D_PRIME(1);
                } 
            else if (Move == "D2"){
                El_cube.D(2);
                } 
            else if (Move == "F"){
                El_cube.F(1);
                }  
            else if (Move == "FPRIME"){
                El_cube.F_PRIME(1);
                } 
            else if (Move == "F2"){
                El_cube.F(2);
                } 
            else if (Move == "B"){
                El_cube.B(1);
                }  
            else if (Move == "BPRIME"){
                El_cube.B_PRIME(1);
                }  
            else if (Move == "B2"){
                El_cube.B(2);
                } 
            else {
                std::cout << "move not found" << std::endl;
            }       
    }
    return El_cube;
	}

