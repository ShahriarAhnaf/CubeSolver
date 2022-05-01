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
                mvprintw(20,20,"bruh");
                break;
            case 'f':
                cube.F(1);
                mvprintw(20,20,"unbruh");
                break;
            case 'q':
                quit = true;
                break;
        }

        cube.draw();
    }

    endwin();
    return 0;
}