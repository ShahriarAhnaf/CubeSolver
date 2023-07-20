#include "Solver.h"
#include <iostream>

void GO_CRAZY_TEST(RubixCube current_cube){
    current_cube.B(3);
    current_cube.L(3);
    current_cube.F(1);
}
int main(){
    int x,y;
    RubixCube cube;
    RubixCube before;
    Solver leSolver;
    WINDOW *solver_window;
    bool quit = false;
    initscr();
    noecho();

    getmaxyx(stdscr, y,x);

    while(!quit){
        cube.draw( x/4,y/2); // middle of screen
        switch(mvgetch(0, 0)){
            case 'u':
                cube.U(1); 
                break;
            case 'U':
                cube.U_PRIME(1);
                break;
            case 'f':
                cube.F(1);
                break;
            case 'F':
                cube.F_PRIME(1);
                break;
            case 'l':
                cube.L(1);
                break;
            case 'L':
                cube.L_PRIME(1);
                break;
            case 'r':
                cube.R(1);
                break;
            case 'R':
                cube.R_PRIME(1);
                break;
            case 'b':
                cube.B(1);
                break;
            case 'B':
                cube.B_PRIME(1);
                break;
            case 'd':
                cube.D(1);
                break;  
            case 'D':
                cube.D_PRIME(1);
                break; 
            case 's':
                 solver_window = newwin(y/4, x/4, 0, 0);
                // solve on current cube
                leSolver.Solve_Cube(cube);
                // cube = leSolver.Apply_Moves(cube, " "); // testing apply moves
                while(mvgetch(0,1) != 'q'){}
                delwin(solver_window);
                break;
            case 't':
                // passing by value to test if it turns into reference??? 
                before = cube;
                if (!(cube == before)) mvprintw(2,1,"operator test FAIL");
                GO_CRAZY_TEST(cube);
                if (!(cube == before)) mvprintw(1,1,"Pass by value FAIL");
                break;
            case 'q':
                quit = true;
                break;
        }
    }

    endwin();
    return 0;
}