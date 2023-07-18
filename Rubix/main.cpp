#include "Solver.h"

int main(){

    RubixCube cube;
    bool quit = false;
    initscr();
    while(!quit){
        cube.draw();
        switch(getch()){
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
                // solve on current cube   
            case 'q':
                quit = true;
                break;
        }
    }

    endwin();
    return 0;
}