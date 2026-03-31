#include "Cube.h"
#include "Solver.h"
#include <iostream>
#include <cassert>
#include <chrono>

static int tests_passed = 0;
static int tests_failed = 0;

bool is_cube_solved(const RubixCube& cube) {
    RubixCube solved_cube;
    return cube == solved_cube;
}

void check(bool condition, const char* name) {
    if (condition) {
        std::cout << "  PASS: " << name << std::endl;
        tests_passed++;
    } else {
        std::cout << "  FAIL: " << name << std::endl;
        tests_failed++;
    }
}

// ------ Move correctness tests ------

void test_cube_initialization() {
    std::cout << "[Initialization]" << std::endl;
    RubixCube cube;
    check(is_cube_solved(cube), "New cube is solved");
}

void test_move_prime_identity() {
    std::cout << "[Move + Prime = Identity]" << std::endl;

    { RubixCube c; c.R(1); c.R_PRIME(1); check(is_cube_solved(c), "R R'"); }
    { RubixCube c; c.L(1); c.L_PRIME(1); check(is_cube_solved(c), "L L'"); }
    { RubixCube c; c.U(1); c.U_PRIME(1); check(is_cube_solved(c), "U U'"); }
    { RubixCube c; c.D(1); c.D_PRIME(1); check(is_cube_solved(c), "D D'"); }
    { RubixCube c; c.F(1); c.F_PRIME(1); check(is_cube_solved(c), "F F'"); }
    { RubixCube c; c.B(1); c.B_PRIME(1); check(is_cube_solved(c), "B B'"); }
}

void test_four_turns_identity() {
    std::cout << "[4x turn = Identity]" << std::endl;

    { RubixCube c; c.R(1); c.R(1); c.R(1); c.R(1); check(is_cube_solved(c), "R x4"); }
    { RubixCube c; c.L(1); c.L(1); c.L(1); c.L(1); check(is_cube_solved(c), "L x4"); }
    { RubixCube c; c.U(1); c.U(1); c.U(1); c.U(1); check(is_cube_solved(c), "U x4"); }
    { RubixCube c; c.D(1); c.D(1); c.D(1); c.D(1); check(is_cube_solved(c), "D x4"); }
    { RubixCube c; c.F(1); c.F(1); c.F(1); c.F(1); check(is_cube_solved(c), "F x4"); }
    { RubixCube c; c.B(1); c.B(1); c.B(1); c.B(1); check(is_cube_solved(c), "B x4"); }

    { RubixCube c; c.R_PRIME(1); c.R_PRIME(1); c.R_PRIME(1); c.R_PRIME(1); check(is_cube_solved(c), "R' x4"); }
    { RubixCube c; c.L_PRIME(1); c.L_PRIME(1); c.L_PRIME(1); c.L_PRIME(1); check(is_cube_solved(c), "L' x4"); }
    { RubixCube c; c.U_PRIME(1); c.U_PRIME(1); c.U_PRIME(1); c.U_PRIME(1); check(is_cube_solved(c), "U' x4"); }
    { RubixCube c; c.D_PRIME(1); c.D_PRIME(1); c.D_PRIME(1); c.D_PRIME(1); check(is_cube_solved(c), "D' x4"); }
    { RubixCube c; c.F_PRIME(1); c.F_PRIME(1); c.F_PRIME(1); c.F_PRIME(1); check(is_cube_solved(c), "F' x4"); }
    { RubixCube c; c.B_PRIME(1); c.B_PRIME(1); c.B_PRIME(1); c.B_PRIME(1); check(is_cube_solved(c), "B' x4"); }
}

void test_double_move() {
    std::cout << "[Double move consistency]" << std::endl;

    { RubixCube a, b; a.R(2); b.R(1); b.R(1); check(a == b, "R(2) == R R"); }
    { RubixCube a, b; a.U(2); b.U(1); b.U(1); check(a == b, "U(2) == U U"); }
    { RubixCube a, b; a.D(2); b.D(1); b.D(1); check(a == b, "D(2) == D D"); }
    { RubixCube a, b; a.F(2); b.F(1); b.F(1); check(a == b, "F(2) == F F"); }
    { RubixCube a, b; a.L(2); b.L(1); b.L(1); check(a == b, "L(2) == L L"); }
    { RubixCube a, b; a.B(2); b.B(1); b.B(1); check(a == b, "B(2) == B B"); }
}

void test_sexy_move() {
    std::cout << "[Sexy move (R U R' U') x6 = Identity]" << std::endl;
    RubixCube cube;
    for (int i = 0; i < 6; i++) {
        cube.R(1); cube.U(1); cube.R_PRIME(1); cube.U_PRIME(1);
    }
    check(is_cube_solved(cube), "(R U R' U') x6");
}

void test_apply_moves_string() {
    std::cout << "[apply_moves string parsing]" << std::endl;
    RubixCube a, b;
    a.R(1); a.U(1); a.R_PRIME(1);
    b.apply_moves("R U R'");
    check(a == b, "apply_moves('R U R\\'') matches manual calls");

    RubixCube c, d;
    c.R(2); c.U(2); c.F(2);
    d.apply_moves("R2 U2 F2");
    check(c == d, "apply_moves('R2 U2 F2') matches manual calls");
}

// ------ Solver tests ------

void test_solver_trivial() {
    std::cout << "[Solver: already solved]" << std::endl;
    RubixCube cube;
    Solver solver;
    solver.Solve_Cube(cube, 5);
    check(is_cube_solved(cube), "Solved cube stays solved");
}

void test_solver_one_move() {
    std::cout << "[Solver: 1-move scramble]" << std::endl;

    const char* scrambles[] = {"R", "L", "U", "D", "F", "B", "R'", "U'", "F'"};
    Solver solver;

    for (const char* scramble : scrambles) {
        RubixCube cube;
        cube.apply_moves(scramble);
        std::string solution = solver.Solve_Cube(cube, 5);
        bool ok = is_cube_solved(cube);
        std::string label = std::string("Scramble: ") + scramble;
        check(ok, label.c_str());
    }
}

void test_solver_two_moves() {
    std::cout << "[Solver: 2-move scrambles]" << std::endl;

    const char* scrambles[] = {"R U", "F R", "U L", "D B", "R F'"};
    Solver solver;

    for (const char* scramble : scrambles) {
        RubixCube cube;
        cube.apply_moves(scramble);
        auto t0 = std::chrono::high_resolution_clock::now();
        std::string solution = solver.Solve_Cube(cube, 8);
        auto t1 = std::chrono::high_resolution_clock::now();
        long ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        bool ok = is_cube_solved(cube);
        std::string label = std::string("Scramble: ") + scramble + " (" + std::to_string(ms) + "ms)";
        check(ok, label.c_str());
        if (!solution.empty())
            std::cout << "    Solution: " << solution << std::endl;
    }
}

void test_solver_three_moves() {
    std::cout << "[Solver: 3-move scrambles]" << std::endl;

    const char* scrambles[] = {"R U F", "L D B", "R' U' F'"};
    Solver solver;

    for (const char* scramble : scrambles) {
        RubixCube cube;
        cube.apply_moves(scramble);
        auto t0 = std::chrono::high_resolution_clock::now();
        std::string solution = solver.Solve_Cube(cube, 10);
        auto t1 = std::chrono::high_resolution_clock::now();
        long ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        bool ok = is_cube_solved(cube);
        std::string label = std::string("Scramble: ") + scramble + " (" + std::to_string(ms) + "ms)";
        check(ok, label.c_str());
        if (!solution.empty())
            std::cout << "    Solution: " << solution << std::endl;
    }
}

int main() {
    std::cout << "=== Rubik's Cube Test Suite ===" << std::endl << std::endl;

    test_cube_initialization();
    test_move_prime_identity();
    test_four_turns_identity();
    test_double_move();
    test_sexy_move();
    test_apply_moves_string();
    test_solver_trivial();
    test_solver_one_move();
    test_solver_two_moves();
    test_solver_three_moves();

    std::cout << std::endl;
    std::cout << "Results: " << tests_passed << " passed, "
              << tests_failed << " failed" << std::endl;

    return tests_failed > 0 ? 1 : 0;
}
