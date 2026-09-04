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
    std::string solution = solver.Solve_Cube(cube, 5);
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

// ------ Human-method algorithm tables ------

// Compare the stickers selected by `mask` (same layout on every side face) plus the whole U face.
static bool first_layer_and(const RubixCube& cube, uint64_t side_mask) {
    RubixCube solved;
    if (cube.get_face(FACE_UP) != solved.get_face(FACE_UP)) return false;
    for (int f = FACE_LEFT; f <= FACE_BACK; f++)
        if ((cube.get_face(f) & side_mask) != (solved.get_face(f) & side_mask)) return false;
    return true;
}

void test_algorithms_preserve_layers() {
    std::cout << "[Algorithm tables leave finished layers alone]" << std::endl;
    const uint64_t TOP_ROW = 0xFFFFFF0000000000ULL;        // positions 0,1,2
    const uint64_t TOP_ROW_AND_MIDDLE = 0xFFFFFFFF000000FFULL; // positions 0,1,2,3,7

    // Last-layer algorithms (D layer) must keep the first two layers.
    const char* last_layer[] = {
        "F' R' D' R D F", "F' D' R' D R F",                                         // orient edges
        "R' D' R D' R' D2 R", "R' D2 R D R' D R",                                   // orient corners
        "R' D' R D R F' R2 D R D R' D' R F", "F' R' D R D R' D' R F R' D' R D R F' R' F", // permute corners
        "R' D R' D' R' D' R' D R D R2", "R2 D' R' D' R D R D R D' R" };           // permute edges
    for (const char* alg : last_layer) {
        RubixCube cube;
        cube.apply_moves(alg);
        check(first_layer_and(cube, TOP_ROW_AND_MIDDLE), (std::string("LL keeps F2L: ") + alg).c_str());
    }

    // Second-layer insertions must keep the first layer.
    const char* edge_insert[] = {
        "D' R' D R D F D' F'", "D F D' F' D' R' D R", "D' B' D B D R D' R'", "D R D' R' D' B' D B",
        "D' L' D L D B D' B'", "D B D' B' D' L' D L", "D' F' D F D L D' L'", "D L D' L' D' F' D F" };
    for (const char* alg : edge_insert) {
        RubixCube cube;
        cube.apply_moves(alg);
        check(first_layer_and(cube, TOP_ROW), (std::string("insertion keeps first layer: ") + alg).c_str());
    }

    // Corner insertions touch one first-layer corner; the white cross must survive.
    const char* corner_insert[] = { "R' D' R D", "B' D' B D", "L' D' L D", "F' D' F D" };
    RubixCube solved;
    for (const char* alg : corner_insert) {
        RubixCube cube;
        cube.apply_moves(alg);
        bool ok = true;
        for (int pos : {TOP, RIGHT, BOTTOM, LEFT})
            ok = ok && GET_COLOR(cube.get_face(FACE_UP), pos) == WHITE;
        for (int f = FACE_LEFT; f <= FACE_BACK; f++)
            ok = ok && GET_COLOR(cube.get_face(f), TOP) == GET_COLOR(solved.get_face(f), TOP);
        check(ok, (std::string("corner insertion keeps cross: ") + alg).c_str());
    }
}

void test_solver_full_scrambles() {
    std::cout << "[Solver: full 25-move scrambles]" << std::endl;
    const char* scrambles[] = {
        "L2 F' D R U B' L D2 R' F U2 B L' F2 D' R2 U L B2 D F' R U' L2 B",
        "F R' B' L2 R2 B' B2 F L B' U2 B' R' B' U D' L' L' B2 U2 B B2 U' D2 B",
        "D2 F2 D' L F2 B2 B2 D2 D2 F D2 R2 D2 F' R2 L2 U' D L' L F' B2 L2 D2 F'",
        "U L' D2 U' L2 L2 L2 L' B2 R' U2 D' F2 L U2 B2 F D2 F B2 B D2 U' F L" };
    Solver solver;
    std::string staged; // stage callbacks, concatenated, must equal the returned solution
    solver.on_stage = [&staged](const std::string& m) { staged += m; };
    for (const char* scramble : scrambles) {
        RubixCube cube;
        cube.apply_moves(scramble);
        RubixCube original = cube;
        staged.clear();
        auto t0 = std::chrono::high_resolution_clock::now();
        std::string solution = solver.Solve_Cube(cube, 8);
        auto t1 = std::chrono::high_resolution_clock::now();
        long ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        original.apply_moves(solution); // the returned moves, replayed, must solve the original
        bool ok = !solution.empty() && is_cube_solved(cube) && is_cube_solved(original) && staged == solution;
        std::string label = std::string("Scramble: ") + scramble + " (" + std::to_string(ms) + "ms)";
        check(ok, label.c_str());
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
    test_algorithms_preserve_layers();
    test_solver_full_scrambles();

    std::cout << std::endl;
    std::cout << "Results: " << tests_passed << " passed, "
              << tests_failed << " failed" << std::endl;

    return tests_failed > 0 ? 1 : 0;
}
