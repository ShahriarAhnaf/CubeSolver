#include "Solver.h"
#include <iostream>
#include <cassert>

// Test function to verify if a cube is solved
bool is_cube_solved(const RubixCube& cube) {
    // Create a solved cube to compare against
    
    RubixCube solved_cube;
    return (cube == solved_cube);
}

// Test function to run various test cases
void run_solver_tests() {
    Solver solver;
    RubixCube cube;
    
    // Test Case 1: Simple scramble
    std::cout << "Test Case 1: Simple scramble" << std::endl;
    cube = RubixCube(); // Reset to solved state
    cube.R(1); cube.U(1); cube.R_PRIME(1); cube.U_PRIME(1); // Apply a simple scramble
    solver.Solve_Cube(cube, 20);
    assert(is_cube_solved(cube));
    std::cout << "Test Case 1 passed!" << std::endl;
    
    // Test Case 2: Complex scramble
    std::cout << "Test Case 2: Complex scramble" << std::endl;
    cube = RubixCube(); // Reset to solved state
    cube.R(1); cube.U(1); cube.R_PRIME(1); cube.U_PRIME(1);
    cube.L(1); cube.D(1); cube.L_PRIME(1); cube.D_PRIME(1);
    cube.F(1); cube.B(1); cube.F_PRIME(1); cube.B_PRIME(1);
    solver.Solve_Cube(cube, 20);
    assert(is_cube_solved(cube));
    std::cout << "Test Case 2 passed!" << std::endl;
    
    // Test Case 3: Maximum scramble
    std::cout << "Test Case 3: Maximum scramble" << std::endl;
    cube = RubixCube(); // Reset to solved state
    for(int i = 0; i < 10; i++) {
        cube.R(1); cube.U(1); cube.R_PRIME(1); cube.U_PRIME(1);
        cube.L(1); cube.D(1); cube.L_PRIME(1); cube.D_PRIME(1);
        cube.F(1); cube.B(1); cube.F_PRIME(1); cube.B_PRIME(1);
    }
    solver.Solve_Cube(cube, 20);
    assert(is_cube_solved(cube));
    std::cout << "Test Case 3 passed!" << std::endl;
    
    // Test Case 4: Edge cases
    std::cout << "Test Case 4: Edge cases" << std::endl;
    cube = RubixCube(); // Reset to solved state
    cube.R(2); cube.U(2); cube.F(2); // Apply double moves
    solver.Solve_Cube(cube, 20);
    assert(is_cube_solved(cube));
    std::cout << "Test Case 4 passed!" << std::endl;
    
    std::cout << "All tests passed successfully!" << std::endl;
}

void GO_CRAZY_TEST(RubixCube current_cube){
    current_cube.B(3);
    current_cube.L(3);
    current_cube.F(1);
}

int main(){
    // Run automated tests
    run_solver_tests();
    
    // Original interactive mode
    int x,y;
    RubixCube cube;
    RubixCube before;
    RubixCube holder;
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
                leSolver.Solve_Cube(cube, 20);
                // show solve
                while(mvgetch(0,1) != 'q'){}
                delwin(solver_window);
                break;
            case 't':
                // passing by value to test if it turns into reference??? 
                before = cube;
                if (!(cube == before)) mvprintw(2,1,"operator test FAIL");
                holder = leSolver.Apply_Moves(cube, "L");
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