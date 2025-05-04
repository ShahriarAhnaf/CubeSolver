#include "Cube.h"
#include "Solver.h"
#include <iostream>
#include <ncurses.h>
#include <string>
#include <algorithm> // for std::min, std::max

int main() {
    RubixCube cube;
    Solver solver;
    WINDOW *solver_window;
    bool quit = false;
    int x, y;
    int depth_limit;
    std::string solution;
    
    initscr();
    noecho();
    getmaxyx(stdscr, y, x);
    // draw_legend(x);
    int legend_width = 20;
    int legend_height = 11;
    int legend_start_x = x - legend_width - 2;
    int legend_start_y = 1;
    WINDOW* legend_win = newwin(legend_height, legend_width, legend_start_y, legend_start_x);
    box(legend_win, 0, 0);
    mvwprintw(legend_win, 1, 1, "Controls:");
    mvwprintw(legend_win, 2, 1, " u/U : U / U'");
    mvwprintw(legend_win, 3, 1, " d/D : D / D'");
    mvwprintw(legend_win, 4, 1, " f/F : F / F'");
    mvwprintw(legend_win, 5, 1, " b/B : B / B'");
    mvwprintw(legend_win, 6, 1, " l/L : L / L'");
    mvwprintw(legend_win, 7, 1, " r/R : R / R'");
    mvwprintw(legend_win, 8, 1, " s   : Solve");
    mvwprintw(legend_win, 9, 1, " q   : Quit");
    wrefresh(legend_win);
    refresh();

    while(!quit) {
        // clear();
        cube.draw(x/4, y/2); // middle of screen
        // wrefresh(legend_win);
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
            case 's':
                solver_window = newwin(y/4, x/4, 0, 0);
                wprintw(solver_window, "Enter depth limit (1-20): ");
                wrefresh(solver_window);
                wscanw(solver_window, "%d", &depth_limit);
                // Clamp depth limit between 1 and 20
                depth_limit = std::max(1, std::min(20, depth_limit));
                wclear(solver_window);
                wprintw(solver_window, "Solving with depth %d...", depth_limit);
                wrefresh(solver_window);
                solution = solver.Solve_Cube(cube, depth_limit);
                if (solution.empty()) {
                    wprintw(solver_window, "No solution found");
                } else {
                    wprintw(solver_window, "Solution found:\n%s", solution.c_str());
                }
                wprintw(solver_window, "\nPress 'q' to continue");
                wrefresh(solver_window);
                while(mvgetch(0,1) != 'q') {}
                delwin(solver_window);
                delwin(legend_win);
                break;
            case 'q':
                quit = true;
                break;
        }
    }
    
    endwin();
    return 0;
} 