#include "Solver.h"
#include <algorithm>
#include <functional>
#include <vector>

Solver::~Solver(){}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void do_moves(RubixCube& cube, const std::string& seq, std::string& record) {
    if (seq.empty()) return;
    cube.apply_moves(seq);
    if (!record.empty()) record += " ";
    record += seq;
}

static uint8_t stk(const RubixCube& cube, int face, int pos) {
    return (uint8_t)GET_COLOR(cube.get_face(face), pos);
}

// ---------------------------------------------------------------------------
// IDFS infrastructure
// ---------------------------------------------------------------------------

bool Solver::matches_target(const RubixCube& cube, const TargetState& target) {
    for (int i = 0; i < 6; i++) {
        uint64_t current_face = cube.get_face(i);
        if ((current_face & target.masks[i]) != (target.faces[i] & target.masks[i]))
            return false;
    }
    return true;
}

bool Solver::Solve_DFS_fast(RubixCube current_cube, const TargetState& target_state,
                            std::vector<int>& path, int depth_remaining, int prev_move) {
    if (target_state.matches_cube(current_cube)) return true;
    if (depth_remaining <= 0) return false;
    dfs_count++;
    for (int i = 0; i < 18; i++) {
        if (is_redundant_move_idx(i, prev_move)) continue;
        RubixCube copy = current_cube;
        copy.apply_move_index(i);
        path.push_back(i);
        if (Solve_DFS_fast(copy, target_state, path, depth_remaining - 1, i))
            return true;
        path.pop_back();
    }
    return false;
}

std::string Solver::Solve_IDFS(RubixCube given_cube, const TargetState& target_state, int Depth_Limit) {
    dfs_count = 0;
    std::vector<int> path;
    for (int depth = 0; depth <= Depth_Limit; depth++) {
        path.clear();
        if (Solve_DFS_fast(given_cube, target_state, path, depth, -1)) {
            std::string result;
            for (int idx : path) {
                if (!result.empty()) result += " ";
                result += Moveset[idx];
            }
            return result;
        }
    }
    return "";
}

RubixCube Solver::Apply_Moves(RubixCube &El_cube, std::string leMoves) {
    RubixCube copy;
    copy = El_cube;
    copy.apply_moves(leMoves);
    return copy;
}

// ===========================================================================
//  Cube layout:
//  Faces:   U=0  L=1  F=2  R=3  B=4  D=5
//  Colors:  W=0  B=1  R=2  G=3  O=4  Y=5
//  Sticker positions:  0(TL) 1(T) 2(TR) 3(R) 4(BR) 5(B) 6(BL) 7(L)
//
//  Adjacency:
//    U edges:  U5-F1  U3-R1  U1-B1  U7-L1
//    U corners: U6-F0-L2  U4-F2-R0  U2-B0-R2  U0-B2-L0
//    D edges:  D1-F5  D3-R5  D5-B5  D7-L5
//    D corners: D0-F6-L4  D2-F4-R6  D4-B6-R4  D6-B4-L6
//    Mid edges: F3-R7  R3-B7  B3-L7  L3-F7
// ===========================================================================

// ---------------------------------------------------------------------------
// Target state builders
// ---------------------------------------------------------------------------

static TargetState make_cross_target() {
    TargetState t;
    for (int i = 0; i < 6; i++) t.set_face_dont_care(i);

    for (int p : {1, 3, 5, 7}) {
        SET_COLOR(t.faces[FACE_UP], p, WHITE);
        t.set_relevant(FACE_UP, p);
    }
    for (int f = FACE_LEFT; f <= FACE_BACK; f++) {
        SET_COLOR(t.faces[f], 1, (uint8_t)f);
        t.set_relevant(f, 1);
    }
    return t;
}

static TargetState make_first_layer_target() {
    TargetState t;
    for (int i = 0; i < 6; i++) t.set_face_dont_care(i);

    // Full U face = WHITE
    for (int p = 0; p < 8; p++) {
        SET_COLOR(t.faces[FACE_UP], p, WHITE);
        t.set_relevant(FACE_UP, p);
    }
    // Top row of each side face
    for (int f = FACE_LEFT; f <= FACE_BACK; f++) {
        for (int p : {0, 1, 2}) {
            SET_COLOR(t.faces[f], p, (uint8_t)f);
            t.set_relevant(f, p);
        }
    }
    return t;
}

static TargetState make_two_layers_target() {
    TargetState t;
    for (int i = 0; i < 6; i++) t.set_face_dont_care(i);

    for (int p = 0; p < 8; p++) {
        SET_COLOR(t.faces[FACE_UP], p, WHITE);
        t.set_relevant(FACE_UP, p);
    }
    for (int f = FACE_LEFT; f <= FACE_BACK; f++) {
        for (int p : {0, 1, 2, 3, 7}) {
            SET_COLOR(t.faces[f], p, (uint8_t)f);
            t.set_relevant(f, p);
        }
    }
    return t;
}

static TargetState make_yellow_cross_target() {
    TargetState t = make_two_layers_target();
    for (int p : {1, 3, 5, 7}) {
        SET_COLOR(t.faces[FACE_BOTTOM], p, YELLOW);
        t.set_relevant(FACE_BOTTOM, p);
    }
    return t;
}

static TargetState make_yellow_face_target() {
    TargetState t = make_two_layers_target();
    for (int p = 0; p < 8; p++) {
        SET_COLOR(t.faces[FACE_BOTTOM], p, YELLOW);
        t.set_relevant(FACE_BOTTOM, p);
    }
    return t;
}

static TargetState make_corner_perm_target() {
    TargetState t = make_yellow_face_target();
    for (int f = FACE_LEFT; f <= FACE_BACK; f++) {
        for (int p : {4, 6}) {
            SET_COLOR(t.faces[f], p, (uint8_t)f);
            t.set_relevant(f, p);
        }
    }
    return t;
}

static TargetState make_solved_target() {
    TargetState t;
    RubixCube solved;
    for (int i = 0; i < 6; i++) {
        t.faces[i] = solved.get_face(i);
        t.masks[i] = 0xFFFFFFFFFFFFFFFF;
    }
    return t;
}

// ===========================================================================
// Step 1 : White Cross
// ===========================================================================

std::string Solver::solve_white_cross(RubixCube& cube) {
    TargetState target = make_cross_target();
    if (target.matches_cube(cube)) return "";
    std::string result = Solve_IDFS(cube, target, 8);
    if (!result.empty()) cube.apply_moves(result);
    return result;
}

// ===========================================================================
// Step 2 : White Corners (First Layer) -- heuristic
// ===========================================================================
//
// U corners and their below-D counterparts:
//   UFL(U6,F0,L2) ← DFL(D0,F6,L4)  colors W,R,B  insert: L D L' D'
//   UFR(U4,F2,R0) ← DFR(D2,F4,R6)  colors W,R,G  insert: R' D' R D
//   UBR(U2,B0,R2) ← DBR(D4,B6,R4)  colors W,O,G  insert: B' D' B D
//   UBL(U0,B2,L0) ← DBL(D6,B4,L6)  colors W,O,B  insert: L' D' L D

std::string Solver::solve_white_corners(RubixCube& cube) {
    std::string moves;

    struct Slot {
        int uf,up, sf1,sp1, sf2,sp2;       // U-layer target sticker positions
        int df,dp, dsf1,dsp1, dsf2,dsp2;   // D-layer below sticker positions
        int c2, c3;                          // non-white colors (sf1 side, sf2 side)
        const char* insert;                  // sexy move to insert from D slot below
        const char* eject;                   // eject from U slot to D
    };

    const Slot slots[4] = {
        {0,6, 2,0, 1,2,  5,0, 2,6, 1,4,  RED,BLUE,   "L D L' D'",  "L D L'"},
        {0,4, 2,2, 3,0,  5,2, 2,4, 3,6,  RED,GREEN,  "R' D' R D",  "R' D R"},
        {0,2, 4,0, 3,2,  5,4, 4,6, 3,4,  ORANGE,GREEN, "B' D' B D", "B' D B"},
        {0,0, 4,2, 1,0,  5,6, 4,4, 1,6,  ORANGE,BLUE,  "L' D' L D", "L' D L"},
    };

    // All 8 corner positions for searching
    struct CPos { int f1,p1, f2,p2, f3,p3; };
    const CPos all_corners[8] = {
        {0,6, 2,0, 1,2},  // UFL
        {0,4, 2,2, 3,0},  // UFR
        {0,2, 4,0, 3,2},  // UBR
        {0,0, 4,2, 1,0},  // UBL
        {5,0, 2,6, 1,4},  // DFL
        {5,2, 2,4, 3,6},  // DFR
        {5,4, 4,6, 3,4},  // DBR
        {5,6, 4,4, 1,6},  // DBL
    };

    const char* d_align[4] = {"", "D", "D2", "D'"};
    const char* eject_u[4] = {"L D L'", "R' D R", "B' D B", "L' D L"};

    for (int t = 0; t < 4; t++) {
        const Slot& s = slots[t];

        for (int iter = 0; iter < 30; iter++) {
            if (stk(cube,s.uf,s.up)==WHITE && stk(cube,s.sf1,s.sp1)==s.c2 && stk(cube,s.sf2,s.sp2)==s.c3)
                break;

            // Find corner with colors WHITE, c2, c3
            int found = -1;
            for (int i = 0; i < 8; i++) {
                int a = stk(cube, all_corners[i].f1, all_corners[i].p1);
                int b = stk(cube, all_corners[i].f2, all_corners[i].p2);
                int c = stk(cube, all_corners[i].f3, all_corners[i].p3);
                int arr[3] = {a, b, c};
                int tgt[3] = {WHITE, s.c2, s.c3};
                std::sort(arr, arr+3);
                std::sort(tgt, tgt+3);
                if (arr[0]==tgt[0] && arr[1]==tgt[1] && arr[2]==tgt[2]) { found=i; break; }
            }
            if (found < 0) break;

            if (found <= 3) {
                do_moves(cube, eject_u[found], moves);
                continue;
            }

            int cur_d = found - 4;
            int n = (t - cur_d + 4) % 4;
            if (n) do_moves(cube, d_align[n], moves);

            // Insert: apply sexy move until corner is correct (max 5 reps)
            for (int rep = 0; rep < 6; rep++) {
                if (stk(cube,s.uf,s.up)==WHITE && stk(cube,s.sf1,s.sp1)==s.c2 && stk(cube,s.sf2,s.sp2)==s.c3)
                    break;
                do_moves(cube, s.insert, moves);
            }
        }
    }
    return moves;
}

// ===========================================================================
// Step 3 : Second Layer Edges -- heuristic
// ===========================================================================
//
// For each mid-layer edge slot, find the edge (which has no yellow sticker),
// get it to the D layer, align, and insert with the standard 8-move algorithm.

std::string Solver::solve_second_layer(RubixCube& cube) {
    std::string moves;

    struct F2LSlot {
        int fa, pa, fb, pb;   // sticker positions in the mid-edge slot
        int ca, cb;           // solved colors
        int d_under_a;        // D-edge index under face_a (DF=0, DR=1, DB=2, DL=3)
        int d_under_b;
        const char* right_insert; // from D under face_a, edge goes to fb side
        const char* left_insert;  // from D under face_b, edge goes to fa side
    };

    const F2LSlot slots[4] = {
        {2,3, 3,7, RED,GREEN, 0,1,
         "D' R' D R D F D' F'", "D F D' F' D' R' D R"},
        {3,3, 4,7, GREEN,ORANGE, 1,2,
         "D' B' D B D R D' R'", "D R D' R' D' B' D B"},
        {4,3, 1,7, ORANGE,BLUE, 2,3,
         "D' L' D L D B D' B'", "D B D' B' D' L' D L"},
        {1,3, 2,7, BLUE,RED, 3,0,
         "D' F' D F D L D' L'", "D L D' L' D' F' D F"},
    };

    // D-edge sticker positions
    struct DEdge { int d_pos; int sf; int sp; };
    const DEdge d_edges[4] = {
        {1, 2, 5}, {3, 3, 5}, {5, 4, 5}, {7, 1, 5},
    };

    // Mid-edge sticker positions
    struct MEdge { int fa; int pa; int fb; int pb; };
    const MEdge m_edges[4] = {
        {2,3, 3,7}, {3,3, 4,7}, {4,3, 1,7}, {1,3, 2,7},
    };

    const char* d_align[4] = {"", "D", "D2", "D'"};
    const char* eject_mid[4] = {
        "D' R' D R D F D' F'",
        "D' B' D B D R D' R'",
        "D' L' D L D B D' B'",
        "D' F' D F D L D' L'",
    };

    for (int t = 0; t < 4; t++) {
        const F2LSlot& s = slots[t];

        for (int iter = 0; iter < 20; iter++) {
            if (stk(cube,s.fa,s.pa)==s.ca && stk(cube,s.fb,s.pb)==s.cb)
                break;

            // Find edge with colors (ca, cb)
            int di = -1, mi = -1;
            for (int i = 0; i < 4; i++) {
                int a = stk(cube, 5, d_edges[i].d_pos);
                int b = stk(cube, d_edges[i].sf, d_edges[i].sp);
                if ((a==s.ca && b==s.cb) || (a==s.cb && b==s.ca)) { di=i; break; }
            }
            if (di < 0) {
                for (int i = 0; i < 4; i++) {
                    int a = stk(cube, m_edges[i].fa, m_edges[i].pa);
                    int b = stk(cube, m_edges[i].fb, m_edges[i].pb);
                    if ((a==s.ca && b==s.cb) || (a==s.cb && b==s.ca)) { mi=i; break; }
                }
            }

            if (di >= 0) {
                int side_color = stk(cube, d_edges[di].sf, d_edges[di].sp);
                if (side_color == s.ca) {
                    int n = (s.d_under_a - di + 4) % 4;
                    if (n) do_moves(cube, d_align[n], moves);
                    do_moves(cube, s.right_insert, moves);
                } else {
                    int n = (s.d_under_b - di + 4) % 4;
                    if (n) do_moves(cube, d_align[n], moves);
                    do_moves(cube, s.left_insert, moves);
                }
            } else if (mi >= 0) {
                do_moves(cube, eject_mid[mi], moves);
            } else {
                break;
            }
        }
    }

    return moves;
}

// ===========================================================================
// Step 4 : Yellow Cross (orient D-face edges)
// ===========================================================================
//
// F' R' D' R D F flips exactly 2 D edges (and changes 2 D corners, which
// is fine since corners are fixed in step 5). It preserves F2L.
// Three OLL edge cases: dot(0)→L-shape(2), L-shape(2)→cross(4), line(2)→cross(4).

std::string Solver::solve_bottom_cross(RubixCube& cube) {
    std::string moves;
    const char* alg = "F' R' D' R D F";
    const char* d_rot[4] = {"", "D", "D2", "D'"};

    auto yellow_edge_count = [](const RubixCube& c) -> int {
        uint64_t d = c.get_face(FACE_BOTTOM);
        int cnt = 0;
        for (int p : {1,3,5,7})
            if (GET_COLOR(d, p) == YELLOW) cnt++;
        return cnt;
    };

    // DFS: up to 3 applications of {D-rot + OLL-cross}
    struct Step { int rot; };
    std::vector<Step> best_path;

    std::function<bool(RubixCube&, std::vector<Step>&, int)> search =
        [&](RubixCube& cur, std::vector<Step>& path, int max_depth) -> bool {
        if (yellow_edge_count(cur) == 4) { best_path = path; return true; }
        if ((int)path.size() >= max_depth) return false;
        for (int r = 0; r < 4; r++) {
            RubixCube next = cur;
            if (r > 0) next.apply_moves(d_rot[r]);
            next.apply_moves(alg);
            path.push_back({r});
            if (search(next, path, max_depth)) return true;
            path.pop_back();
        }
        return false;
    };

    std::vector<Step> path;
    for (int depth = 1; depth <= 3; depth++) {
        path.clear();
        RubixCube tmp = cube;
        if (search(tmp, path, depth)) break;
    }

    for (const auto& step : best_path) {
        if (step.rot > 0) do_moves(cube, d_rot[step.rot], moves);
        do_moves(cube, alg, moves);
    }

    return moves;
}

// ===========================================================================
// Step 5 : Yellow Face (orient D-face corners)
// ===========================================================================
//
// R' D' R D' R' D2 R (Sune adapted for D layer) twists 3 D-layer corners
// while preserving F2L and D edges. Uses brute-force search over all
// D-rotation + alg combos with 3-step lookahead to avoid cycles.

std::string Solver::solve_yellow_face(RubixCube& cube) {
    std::string moves;

    const char* sune = "R' D' R D' R' D2 R";
    const char* anti_sune = "R' D2 R D R' D R";

    auto count_yellow_corners = [](const RubixCube& c) -> int {
        uint64_t d = c.get_face(FACE_BOTTOM);
        int cnt = 0;
        for (int p : {0, 2, 4, 6})
            if (GET_COLOR(d, p) == YELLOW) cnt++;
        return cnt;
    };

    if (count_yellow_corners(cube) == 4) return moves;

    // BFS over {D-rotation + Sune/Anti-Sune} steps, up to depth 4
    const char* d_rot[4] = {"", "D", "D2", "D'"};
    const char* algs[2] = {sune, anti_sune};

    struct Step { int rot; int alg; };

    // DFS search for a sequence of up to 4 (D-rot + algorithm) steps
    // that gives 4 yellow corners
    std::vector<Step> best_path;

    std::function<bool(RubixCube&, std::vector<Step>&, int)> search =
        [&](RubixCube& cur, std::vector<Step>& path, int max_depth) -> bool {
        if (count_yellow_corners(cur) == 4) {
            best_path = path;
            return true;
        }
        if ((int)path.size() >= max_depth) return false;

        for (int r = 0; r < 4; r++) {
            for (int a = 0; a < 2; a++) {
                RubixCube next = cur;
                if (r > 0) next.apply_moves(d_rot[r]);
                next.apply_moves(algs[a]);
                path.push_back({r, a});
                if (search(next, path, max_depth)) return true;
                path.pop_back();
            }
        }
        return false;
    };

    std::vector<Step> path;
    for (int depth = 1; depth <= 5; depth++) {
        path.clear();
        RubixCube tmp = cube;
        if (search(tmp, path, depth)) break;
    }

    for (const auto& step : best_path) {
        if (step.rot > 0) do_moves(cube, d_rot[step.rot], moves);
        do_moves(cube, algs[step.alg], moves);
    }

    return moves;
}

// ===========================================================================
// Step 6 : Corner Permutation
// ===========================================================================
//
// R' D2 R D R' D2 L D' R D L' (T-perm adapted for D layer)
// Swaps two adjacent D-layer corners (and two edges).
// Preserves F2L and D-face orientation (all corners stay yellow-on-D).

std::string Solver::solve_corner_permutation(RubixCube& cube) {
    std::string moves;

    TargetState target = make_corner_perm_target();
    if (target.matches_cube(cube)) return moves;

    // Quick D alignment check
    const char* d_opts[4] = {"", "D", "D2", "D'"};
    for (int i = 1; i < 4; i++) {
        RubixCube test = cube;
        test.apply_moves(d_opts[i]);
        if (target.matches_cube(test)) {
            do_moves(cube, d_opts[i], moves);
            return moves;
        }
    }

    const char* t_perm = "R' D2 R D R' D2 L D' R D L'";
    const char* d_rot[4] = {"", "D", "D2", "D'"};

    // DFS over {D-rot + T-perm} steps, up to depth 4
    struct Step { int rot; };
    std::vector<Step> best_path;

    std::function<bool(RubixCube&, std::vector<Step>&, int)> search =
        [&](RubixCube& cur, std::vector<Step>& path, int max_depth) -> bool {
        if (target.matches_cube(cur)) { best_path = path; return true; }
        // Also check D alignments
        for (int d = 1; d < 4; d++) {
            RubixCube t = cur;
            t.apply_moves(d_rot[d]);
            if (target.matches_cube(t)) {
                path.push_back({d});
                best_path = path;
                path.pop_back();
                return true;
            }
        }
        if ((int)path.size() >= max_depth) return false;
        for (int r = 0; r < 4; r++) {
            RubixCube next = cur;
            if (r > 0) next.apply_moves(d_rot[r]);
            next.apply_moves(t_perm);
            path.push_back({r});
            if (search(next, path, max_depth)) return true;
            path.pop_back();
        }
        return false;
    };

    std::vector<Step> path;
    for (int depth = 1; depth <= 4; depth++) {
        path.clear();
        RubixCube tmp = cube;
        if (search(tmp, path, depth)) break;
    }

    for (size_t i = 0; i < best_path.size(); i++) {
        if (best_path[i].rot > 0) do_moves(cube, d_rot[best_path[i].rot], moves);
        // Last step might just be a D alignment (no T-perm needed)
        if (i < best_path.size() - 1 || !target.matches_cube(cube))
            do_moves(cube, t_perm, moves);
    }

    return moves;
}

// ===========================================================================
// Step 7 : Edge Permutation
// ===========================================================================
//
// R2 D R D R' D' R' D' R' D R' (U-perm CW, adapted for D layer)
// R D' R D R D R D' R' D' R2  (U-perm CCW, adapted for D layer)
// These 3-cycle D-layer edges while preserving F2L and corner positions.

std::string Solver::solve_edge_permutation(RubixCube& cube) {
    std::string moves;

    RubixCube solved;
    if (cube == solved) return moves;

    const char* uperm_cw  = "R2 D R D R' D' R' D' R' D R'";
    const char* uperm_ccw = "R D' R D R D R D' R' D' R2";
    const char* d_rot[4] = {"", "D", "D2", "D'"};

    // DFS over {D-rot + U-perm CW/CCW} steps, up to depth 4
    struct Step { int rot; int alg; };  // alg: 0=CW, 1=CCW
    const char* algs[2] = {uperm_cw, uperm_ccw};
    std::vector<Step> best_path;

    std::function<bool(RubixCube&, std::vector<Step>&, int)> search =
        [&](RubixCube& cur, std::vector<Step>& path, int max_depth) -> bool {
        if (cur == solved) { best_path = path; return true; }
        for (int d = 1; d < 4; d++) {
            RubixCube t = cur;
            t.apply_moves(d_rot[d]);
            if (t == solved) {
                path.push_back({d, -1});
                best_path = path;
                path.pop_back();
                return true;
            }
        }
        if ((int)path.size() >= max_depth) return false;
        for (int r = 0; r < 4; r++) {
            for (int a = 0; a < 2; a++) {
                RubixCube next = cur;
                if (r > 0) next.apply_moves(d_rot[r]);
                next.apply_moves(algs[a]);
                path.push_back({r, a});
                if (search(next, path, max_depth)) return true;
                path.pop_back();
            }
        }
        return false;
    };

    std::vector<Step> path;
    for (int depth = 1; depth <= 4; depth++) {
        path.clear();
        RubixCube tmp = cube;
        if (search(tmp, path, depth)) break;
    }

    for (const auto& step : best_path) {
        if (step.rot > 0) do_moves(cube, d_rot[step.rot], moves);
        if (step.alg >= 0) do_moves(cube, algs[step.alg], moves);
    }

    return moves;
}

// ===========================================================================
// Solve_Cube : main entry point
// ===========================================================================

std::string Solver::Solve_Cube(RubixCube &given_cube, int Depth_Limit) {
    (void)Depth_Limit;

    if (given_cube == RubixCube()) return "";

    std::string all_moves;

    auto append = [&](const std::string& step) {
        if (step.empty()) return;
        if (!all_moves.empty()) all_moves += " ";
        all_moves += step;
    };

    append(solve_white_cross(given_cube));
    append(solve_white_corners(given_cube));
    append(solve_second_layer(given_cube));
    append(solve_bottom_cross(given_cube));
    append(solve_yellow_face(given_cube));
    append(solve_corner_permutation(given_cube));
    append(solve_edge_permutation(given_cube));

    return all_moves;
}
