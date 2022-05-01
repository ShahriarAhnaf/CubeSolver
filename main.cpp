#include "Solver.h"

int main(){
    initscr();
    RubixCube cube;
    cube.draw();
    refresh();
    getch();
    endwin();
    return 0;
}