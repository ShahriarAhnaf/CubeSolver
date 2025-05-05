#include "Cube.h"
#include "Solver.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <ncurses.h>
#include <thread>
#include <chrono>

// Test function to verify if a cube is solved
bool is_cube_solved(const RubixCube& cube) {
    RubixCube solved_cube;
    return cube == solved_cube;
}

void test_cube_initialization() {
    RubixCube cube;
    assert(is_cube_solved(cube) && "Cube should be initialized in solved state");
    std::cout << "✓ Cube initialization test passed" << std::endl;
}

void test_basic_moves() {
    std::cout << "Testing basic moves..." << std::endl;
    RubixCube cube;
    
    // Test R move
    cube.R(1);
    assert(!is_cube_solved(cube));
    
    // Test R' move
    cube.R_PRIME(1);
    assert(is_cube_solved(cube));
    
    std::cout << "Basic moves test passed!" << std::endl;
}

void test_sequence_moves() {
    std::cout << "Testing sequence moves..." << std::endl;
    RubixCube cube;
    
    // Test R U R' U' sequence
    cube.R(1);
    cube.U(1);
    cube.R_PRIME(1);
    cube.U_PRIME(1);
    assert(is_cube_solved(cube));
    
    std::cout << "Sequence moves test passed!" << std::endl;
}

void test_solver() {
    std::cout << "Testing solver..." << std::endl;
    RubixCube cube;
    
    // Scramble the cube
    cube.R(1);
    cube.U(1);
    cube.R_PRIME(1);
    cube.U_PRIME(1);
    
    // Create solver and solve
    Solver solver;
    std::string solution = solver.Solve_Cube(cube, 4);
    
    // Apply solution
    cube.apply_moves(solution);
    assert(is_cube_solved(cube));
    
    std::cout << "Solver test passed!" << std::endl;
}

void test_interactive_mode() {
    std::cout << "Testing interactive mode..." << std::endl;
    RubixCube cube;
    RubixCube before;
    RubixCube holder;
    Solver solver;
    WINDOW *solver_window;
    bool quit = false;
    int x, y;
    int depth_limit;  // Moved outside switch statement
    std::string solution;  // Moved outside switch statement
    
    initscr();
    noecho();
    getmaxyx(stdscr, y, x);
    
    // Draw initial legend
    WINDOW* legend_win = newwin(12, 25, 1, x - 26);
    box(legend_win, 0, 0);
    mvwprintw(legend_win, 1, 1, "Controls:");
    mvwprintw(legend_win, 2, 1, " u/U : U / U'");
    mvwprintw(legend_win, 3, 1, " d/D : D / D'");
    mvwprintw(legend_win, 4, 1, " f/F : F / F'");
    mvwprintw(legend_win, 5, 1, " b/B : B / B'");
    mvwprintw(legend_win, 6, 1, " l/L : L / L'");
    mvwprintw(legend_win, 7, 1, " r/R : R / R'");
    mvwprintw(legend_win, 8, 1, " m   : Mix cube");
    mvwprintw(legend_win, 9, 1, " s   : Solve");
    mvwprintw(legend_win, 10, 1, " q   : Quit");
    wrefresh(legend_win);
    
    while(!quit) {
        clear();
        cube.draw(x/4, y/2); // middle of screen
        wrefresh(legend_win);
        refresh();
        switch(mvgetch(0, 0)) {
            case 'u': cube.U(1); break;
            case 'U': cube.U_PRIME(1); break;
            case 'f': cube.F(1); break;
            case 'F': cube.F_PRIME(1); break;
            case 'l': cube.L(1); break;
            case 'L': cube.L_PRIME(1); break;
            case 'r': cube.R(1); break;
            case 'R': cube.R_PRIME(1); break;
            case 'b': cube.B(1); break;
            case 'B': cube.B_PRIME(1); break;
            case 'd': cube.D(1); break;
            case 'D': cube.D_PRIME(1); break;
            case 'm': {
                solver_window = newwin(y/4, x/4, y/4, x/4);
                box(solver_window, 0, 0);
                wprintw(solver_window, "Enter number of moves (1-100): ");
                wrefresh(solver_window);
                int num_moves;
                wscanw(solver_window, "%d", &num_moves);
                // Clamp number of moves between 1 and 100
                num_moves = std::max(1, std::min(100, num_moves));
                wclear(solver_window);
                box(solver_window, 0, 0);
                wprintw(solver_window, "Scrambling with %d moves...", num_moves);
                wrefresh(solver_window);
                
                // Perform random moves
                const char moves[] = "UDFBRL";
                const char* primes[] = {"", "'", "2"};
                for(int i = 0; i < num_moves; i++) {
                    char move = moves[rand() % 6];
                    const char* prime = primes[rand() % 3];
                    std::string move_str = std::string(1, move) + prime;
                    cube.apply_moves(move_str);
                    wprintw(solver_window, "\nMove %d: %s", i + 1, move_str.c_str());
                    wrefresh(solver_window);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                
                wprintw(solver_window, "\n\nPress 'q' to continue");
                wrefresh(solver_window);
                while(mvgetch(0,1) != 'q') {}
                delwin(solver_window);
                break;
            }
            case 's':
                solver_window = newwin(y/4, x/4, y/4, x/4);
                box(solver_window, 0, 0);
                wprintw(solver_window, "Enter depth limit (1-20): ");
                wrefresh(solver_window);
                wscanw(solver_window, "%d", &depth_limit);
                // Clamp depth limit between 1 and 20
                depth_limit = std::max(1, std::min(20, depth_limit));
                wclear(solver_window);
                box(solver_window, 0, 0);
                wprintw(solver_window, "Solving with depth %d...", depth_limit);
                wrefresh(solver_window);
                solution = solver.Solve_Cube(cube, depth_limit);
                if (solution.empty()) {
                    wprintw(solver_window, "\nNo solution found");
                } else {
                    wprintw(solver_window, "\nSolution found:\n%s", solution.c_str());
                }
                wprintw(solver_window, "\nPress 'q' to continue");
                wrefresh(solver_window);
                while(mvgetch(0,1) != 'q') {}
                delwin(solver_window);
                break;
            case 'q':
                quit = true;
                break;
        }
    }
    
    delwin(legend_win);
    endwin();
    std::cout << "✓ Interactive mode test passed" << std::endl;
}

int main() {
    std::cout << "Running Cube Tests..." << std::endl;
    
    try {
        test_cube_initialization();
        test_basic_moves();
        test_sequence_moves();
        test_solver();
        test_interactive_mode();
        
        std::cout << "\nAll tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
} 