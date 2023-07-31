#include "Solver.h"
#include <iostream> 
#include  <assert.h>

int main(){
    RubixCube Test_Cube = RubixCube(); // solved cube
    Solver Solve = Solver();
    RubixCube Temp = RubixCube();
    Solve.Apply_Moves(Test_Cube, "L R L");
    assert(Test_Cube == Temp);
    return 0;
}