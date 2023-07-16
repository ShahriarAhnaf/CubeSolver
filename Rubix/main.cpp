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
            case 'r':
                cube.R(1);
                break;
            case 'b':
                cube.B(1);
                break;
            case 'd':
                cube.D(1);
                break;  
            case 'D':
                cube.D_PRIME(1);
                break;    
            case 'q':
                quit = true;
                break;
        }
    }

    endwin();
    return 0;
}