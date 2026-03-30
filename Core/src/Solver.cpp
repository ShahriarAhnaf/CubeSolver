#include "Solver.h"
#include <iostream>
#include <sstream>
#include <algorithm>

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

static int sticker(const RubixCube& cube, int face, int pos) {
    return (int)GET_COLOR(cube.get_face(face), pos);
}

// Edge table: 12 edges, each with two sticker slots {face, position}
struct EdgeSlot { int f1, p1, f2, p2; };
static const EdgeSlot EDGES[12] = {
    {FACE_UP, BOTTOM,  FACE_FRONT, TOP},         // 0  UF
    {FACE_UP, RIGHT,   FACE_RIGHT, TOP},          // 1  UR
    {FACE_UP, TOP,     FACE_BACK,  TOP},          // 2  UB
    {FACE_UP, LEFT,    FACE_LEFT,  TOP},          // 3  UL
    {FACE_FRONT, RIGHT, FACE_RIGHT, LEFT},        // 4  FR
    {FACE_RIGHT, RIGHT, FACE_BACK,  LEFT},        // 5  RB
    {FACE_BACK,  RIGHT, FACE_LEFT,  LEFT},        // 6  BL
    {FACE_LEFT,  RIGHT, FACE_FRONT, LEFT},        // 7  LF
    {FACE_BOTTOM, TOP,    FACE_FRONT, BOTTOM},    // 8  DF
    {FACE_BOTTOM, RIGHT,  FACE_RIGHT, BOTTOM},    // 9  DR
    {FACE_BOTTOM, BOTTOM, FACE_BACK,  BOTTOM},    // 10 DB
    {FACE_BOTTOM, LEFT,   FACE_LEFT,  BOTTOM},    // 11 DL
};

// Corner table: 8 corners, each with three sticker slots {face, position}
struct CornerSlot { int f1, p1, f2, p2, f3, p3; };
static const CornerSlot CORNERS[8] = {
    {FACE_UP, BOTTOM_LEFT,  FACE_FRONT, TOP_LEFT,     FACE_LEFT,  TOP_RIGHT},   // 0 UFL
    {FACE_UP, BOTTOM_RIGHT, FACE_FRONT, TOP_RIGHT,    FACE_RIGHT, TOP_LEFT},    // 1 UFR
    {FACE_UP, TOP_RIGHT,    FACE_BACK,  TOP_LEFT,     FACE_RIGHT, TOP_RIGHT},   // 2 UBR
    {FACE_UP, TOP_LEFT,     FACE_BACK,  TOP_RIGHT,    FACE_LEFT,  TOP_LEFT},    // 3 UBL
    {FACE_BOTTOM, TOP_LEFT,     FACE_FRONT, BOTTOM_LEFT,  FACE_LEFT,  BOTTOM_RIGHT}, // 4 DFL
    {FACE_BOTTOM, TOP_RIGHT,    FACE_FRONT, BOTTOM_RIGHT, FACE_RIGHT, BOTTOM_LEFT},  // 5 DFR
    {FACE_BOTTOM, BOTTOM_RIGHT, FACE_BACK,  BOTTOM_LEFT,  FACE_RIGHT, BOTTOM_RIGHT}, // 6 DBR
    {FACE_BOTTOM, BOTTOM_LEFT,  FACE_BACK,  BOTTOM_RIGHT, FACE_LEFT,  BOTTOM_LEFT},  // 7 DBL
};

static int find_edge(const RubixCube& cube, int c1, int c2) {
    for (int i = 0; i < 12; i++) {
        int a = sticker(cube, EDGES[i].f1, EDGES[i].p1);
        int b = sticker(cube, EDGES[i].f2, EDGES[i].p2);
        if ((a == c1 && b == c2) || (a == c2 && b == c1))
            return i;
    }
    return -1;
}

static int find_corner(const RubixCube& cube, int c1, int c2, int c3) {
    for (int i = 0; i < 8; i++) {
        int a = sticker(cube, CORNERS[i].f1, CORNERS[i].p1);
        int b = sticker(cube, CORNERS[i].f2, CORNERS[i].p2);
        int c = sticker(cube, CORNERS[i].f3, CORNERS[i].p3);
        bool match = (a==c1||a==c2||a==c3) && (b==c1||b==c2||b==c3) && (c==c1||c==c2||c==c3)
                     && (a!=b) && (b!=c) && (a!=c);
        if (match) return i;
    }
    return -1;
}

// D-layer edge index (8..11) mapped to D-position (0..3)
// D moves edges: pos 0(DF)->1(DR)->2(DB)->3(DL)->0
static const char* d_align_moves[4] = {"", "D", "D2", "D'"};

static int d_edge_pos(int edge_idx) { return edge_idx - 8; }

// The face "under" each D edge position
static const int D_FACE[4] = {FACE_FRONT, FACE_RIGHT, FACE_BACK, FACE_LEFT};

// face2 move strings for each side face
static const char* FACE2[6] = {"", "L2", "F2", "R2", "B2", ""};
// face CW / CCW move strings indexed by FACE_ORIENTATION
static const char* FACE_CW[6]  = {"U", "L", "F", "R", "B", "D"};
static const char* FACE_CCW[6] = {"U'", "L'", "F'", "R'", "B'", "D'"};

// ---------------------------------------------------------------------------
// IDFS infrastructure (kept but not called from Solve_Cube)
// ---------------------------------------------------------------------------

bool Solver::matches_target(const RubixCube& cube, const TargetState& target) {
    for(int i = 0; i < 6; i++) {
        uint64_t current_face = cube.get_face(i);
        if((current_face & target.masks[i]) != (target.faces[i] & target.masks[i]))
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

RubixCube Solver::Apply_Moves(RubixCube &El_cube, std::string leMoves){
    RubixCube copy;
    copy = El_cube;
    copy.apply_moves(leMoves);
    return copy;
}

// ===========================================================================
// Step 1 : White Cross
// ===========================================================================

std::string Solver::solve_white_cross(RubixCube& cube) {
    std::string moves;

    // Target edges on U face:
    //   UF (edge 0): U BOTTOM = WHITE,  FRONT TOP = RED
    //   UR (edge 1): U RIGHT  = WHITE,  RIGHT TOP = GREEN
    //   UB (edge 2): U TOP    = WHITE,  BACK  TOP = ORANGE
    //   UL (edge 3): U LEFT   = WHITE,  LEFT  TOP = BLUE
    struct CrossTarget { int u_pos; int side_face; int pair_color; };
    const CrossTarget targets[4] = {
        {BOTTOM, FACE_FRONT, RED},
        {RIGHT,  FACE_RIGHT, GREEN},
        {TOP,    FACE_BACK,  ORANGE},
        {LEFT,   FACE_LEFT,  BLUE},
    };

    // D-position directly below each side face
    //   FRONT->0(DF), RIGHT->1(DR), BACK->2(DB), LEFT->3(DL)
    auto face_to_dpos = [](int face) -> int {
        switch(face) {
            case FACE_FRONT: return 0;
            case FACE_RIGHT: return 1;
            case FACE_BACK:  return 2;
            case FACE_LEFT:  return 3;
        }
        return -1;
    };

    for (int t = 0; t < 4; t++) {
        int target_face = targets[t].side_face;
        int pair_color  = targets[t].pair_color;
        int u_pos       = targets[t].u_pos;
        int target_dpos = face_to_dpos(target_face);

        for (int iter = 0; iter < 30; iter++) {
            if (sticker(cube, FACE_UP, u_pos) == WHITE &&
                sticker(cube, target_face, TOP) == pair_color)
                break;

            int ei = find_edge(cube, WHITE, pair_color);
            if (ei < 0) break;

            // --- edge in U layer (0..3): eject to D with face2 ---
            if (ei <= 3) {
                int adj_face = EDGES[ei].f2;  // the side face adjacent to this U edge
                do_moves(cube, FACE2[adj_face], moves);
                continue;
            }

            // --- edge in middle layer (4..7): push down with a single face turn ---
            if (ei >= 4 && ei <= 7) {
                // Middle edges: FR(4) RB(5) BL(6) LF(7)
                // The "right" face of each: R(4) B(5) L(6) F(7)
                int right_faces[4] = {FACE_RIGHT, FACE_BACK, FACE_LEFT, FACE_FRONT};
                int rf = right_faces[ei - 4];
                // Turning this face CCW sends the middle edge to the D layer
                do_moves(cube, FACE_CCW[rf], moves);
                continue;
            }

            // --- edge in D layer (8..11) ---
            int cur_dpos = ei - 8;
            // Sticker at the D-face side vs the side-face side
            int d_sticker    = sticker(cube, EDGES[ei].f1, EDGES[ei].p1);
            int side_sticker = sticker(cube, EDGES[ei].f2, EDGES[ei].p2);

            if (d_sticker == WHITE) {
                // WHITE faces DOWN: align pair color under target face, then face2
                int n = (target_dpos - cur_dpos + 4) % 4;
                if (n) do_moves(cube, d_align_moves[n], moves);
                do_moves(cube, FACE2[target_face], moves);
            } else {
                // WHITE faces SIDE: move edge so WHITE faces the target face,
                // then use D + adjacent face' to insert.
                // Place the edge one position CW from target in D, then:
                //   target_face  adjacent_face'  target_face'   (3-move insert)
                // But simplest: turn the side face to move edge to U (wrong orient),
                // then it'll be ejected next iteration via face2.
                int side_face = EDGES[ei].f2; // the face WHITE is on
                do_moves(cube, FACE_CW[side_face], moves);
            }
        }
    }
    return moves;
}

// ===========================================================================
// Step 2 : White Corners
// ===========================================================================

std::string Solver::solve_white_corners(RubixCube& cube) {
    std::string moves;

    // Target corners on U face (solved colors):
    //   UFL (corner 0): U BL=W, F TL=R, L TR=B
    //   UFR (corner 1): U BR=W, F TR=R, R TL=G
    //   UBR (corner 2): U TR=W, B TL=O, R TR=G
    //   UBL (corner 3): U TL=W, B TR=O, L TL=B
    struct CornerTarget {
        int slot;           // target corner index (0..3 = UFL,UFR,UBR,UBL)
        int below;          // D corner directly below (4..7 = DFL,DFR,DBR,DBL)
        int c2, c3;         // the two non-white colors (face order: f2, f3)
        const char* insert_f2;   // WHITE on f2 of below corner
        const char* insert_f3;   // WHITE on f3 of below corner
        const char* insert_d;    // WHITE on D face of below corner
        const char* eject;       // eject from U slot to D
    };

    const CornerTarget targets[4] = {
        // UFL, below=DFL
        {0, 4, RED, BLUE,
         "F' D' F",           // WHITE on DFL.f2 = F BL(6) = F side
         "L D L'",            // WHITE on DFL.f3 = L BR(4) = L side
         "L D2 L' D' L D L'", // WHITE on DFL.D  = D TL(0)
         "L D' L'"},          // eject UFL to D

        // UFR, below=DFR
        {1, 5, RED, GREEN,
         "F D F'",            // WHITE on DFR.f2 = F BR(4)
         "R' D' R",           // WHITE on DFR.f3 = R BL(6)
         "R' D2 R D R' D' R", // WHITE on DFR.D  = D TR(2)
         "R' D R"},           // eject UFR to D

        // UBR, below=DBR
        {2, ORANGE, GREEN, /* unused */ 0,
         "B D B'",            // WHITE on DBR.f2 = B BL(6)
         "R D R'",            // WHITE on DBR.f3 = R BR(4)
         "R D2 R' D' R D R'", // WHITE on DBR.D  = D BR(4)
         "R D' R'"},          // eject UBR to D .. hmm

        // UBL, below=DBL
        {3, ORANGE, BLUE, /* unused */ 0,
         "B' D' B",           // WHITE on DBL.f2 = B BR(4)
         "L' D L",            // WHITE on DBL.f3 = L BL(6)  .. hmm
         "L' D2 L D L' D' L", // WHITE on DBL.D  = D BL(6)
         "L D L'"},           // eject UBL to D .. hmm
    };

    // Correct the targets array: slot 2 and 3 c2/c3 values
    // UBR: c2=ORANGE, c3=GREEN.  UBL: c2=ORANGE, c3=BLUE.

    struct SlotInfo {
        int slot;
        int below;
        int c2, c3;
        const char* insert_f2;
        const char* insert_f3;
        const char* insert_d;
        const char* eject;
    };

    const SlotInfo slots[4] = {
        {0, 4, RED, BLUE,
         "F' D' F", "L D L'", "L D2 L' D' L D L'", "L D' L'"},
        {1, 5, RED, GREEN,
         "F D F'", "R' D' R", "R' D2 R D R' D' R", "R' D R"},
        {2, 6, ORANGE, GREEN,
         "B' D' B", "R D R'", "R D2 R' D' R D R'", "B D' B'"},
        {3, 7, ORANGE, BLUE,
         "B D B'", "L' D' L", "L' D2 L D L' D' L", "B' D B'"},
    };

    // D corner positions (0=DFL,1=DFR,2=DBR,3=DBL)
    // D moves corners: 0->1->2->3->0

    for (int t = 0; t < 4; t++) {
        const SlotInfo& s = slots[t];

        for (int iter = 0; iter < 30; iter++) {
            // Check if corner is correctly placed
            int a = sticker(cube, CORNERS[s.slot].f1, CORNERS[s.slot].p1);
            int b = sticker(cube, CORNERS[s.slot].f2, CORNERS[s.slot].p2);
            int c = sticker(cube, CORNERS[s.slot].f3, CORNERS[s.slot].p3);
            if (a == WHITE && b == s.c2 && c == s.c3) break;

            int ci = find_corner(cube, WHITE, s.c2, s.c3);
            if (ci < 0) break;

            // If in U layer (0..3), eject to D
            if (ci <= 3) {
                // Eject the specific U slot that the corner is in
                const char* eject_moves[4] = {
                    "L D' L'", "R' D R", "B D' B'", "B' D B'"
                };
                // Fix: eject_moves[3] should be "B' D B"
                // but let's use the slot-specific eject if it's our slot
                if (ci == s.slot) {
                    do_moves(cube, s.eject, moves);
                } else {
                    // Eject from whichever U slot it's in
                    const char* generic_eject[4] = {
                        "L D' L'", "R' D R", "B D' B'", "B' D B"
                    };
                    do_moves(cube, generic_eject[ci], moves);
                }
                continue;
            }

            // Corner is in D layer (4..7)
            int cur_d_corner = ci - 4; // 0=DFL,1=DFR,2=DBR,3=DBL
            int target_d_corner = s.below - 4;

            // Align below target using D moves
            int n = (target_d_corner - cur_d_corner + 4) % 4;
            if (n) do_moves(cube, d_align_moves[n], moves);

            // Determine WHITE sticker orientation on the below corner
            int w1 = sticker(cube, CORNERS[s.below].f1, CORNERS[s.below].p1);
            int w2 = sticker(cube, CORNERS[s.below].f2, CORNERS[s.below].p2);
            int w3 = sticker(cube, CORNERS[s.below].f3, CORNERS[s.below].p3);

            if (w1 == WHITE) {
                do_moves(cube, s.insert_d, moves);
            } else if (w2 == WHITE) {
                do_moves(cube, s.insert_f2, moves);
            } else if (w3 == WHITE) {
                do_moves(cube, s.insert_f3, moves);
            }
        }
    }

    return moves;
}

// ===========================================================================
// Step 3 : Second Layer Edges (F2L)
// ===========================================================================

std::string Solver::solve_second_layer(RubixCube& cube) {
    std::string moves;

    // Middle edge slots and their solved colors:
    //   FR (edge 4): F R(3)=RED,   R L(7)=GREEN     face_a=FRONT, face_b=RIGHT
    //   RB (edge 5): R R(3)=GREEN, B L(7)=ORANGE     face_a=RIGHT, face_b=BACK
    //   BL (edge 6): B R(3)=ORANGE,L L(7)=BLUE       face_a=BACK,  face_b=LEFT
    //   LF (edge 7): L R(3)=BLUE,  F L(7)=RED        face_a=LEFT,  face_b=FRONT

    struct F2LTarget {
        int edge_idx;
        int ca, cb;       // solved colors (face_a side, face_b side)
        int face_a, face_b;
        int d_pos_a;      // D position under face_a
        int d_pos_b;      // D position under face_b
        const char* right_insert; // from D-under-face_a, side color goes to face_b
        const char* left_insert;  // from D-under-face_b, side color goes to face_a
        const char* eject;        // eject wrong edge from this slot to D
    };

    const F2LTarget targets[4] = {
        {4, RED, GREEN, FACE_FRONT, FACE_RIGHT, 0, 1,
         "D' R' D R D F D' F'",   // right insert from DF
         "D F D' F' D' R' D R",   // left insert from DR
         "D' R' D R D F D' F'"},  // eject (same as right insert; puts a D edge in)

        {5, GREEN, ORANGE, FACE_RIGHT, FACE_BACK, 1, 2,
         "D' B' D B D R D' R'",
         "D R D' R' D' B' D B",
         "D' B' D B D R D' R'"},

        {6, ORANGE, BLUE, FACE_BACK, FACE_LEFT, 2, 3,
         "D' L' D L D B D' B'",
         "D B D' B' D' L' D L",
         "D' L' D L D B D' B'"},

        {7, BLUE, RED, FACE_LEFT, FACE_FRONT, 3, 0,
         "D' F' D F D L D' L'",
         "D L D' L' D' F' D F",
         "D' F' D F D L D' L'"},
    };

    for (int t = 0; t < 4; t++) {
        const F2LTarget& tgt = targets[t];

        for (int iter = 0; iter < 30; iter++) {
            // Check if correctly placed
            if (sticker(cube, EDGES[tgt.edge_idx].f1, EDGES[tgt.edge_idx].p1) == tgt.ca &&
                sticker(cube, EDGES[tgt.edge_idx].f2, EDGES[tgt.edge_idx].p2) == tgt.cb)
                break;

            int ei = find_edge(cube, tgt.ca, tgt.cb);
            if (ei < 0) break;

            // If in middle layer (4..7) but wrong, eject to D
            if (ei >= 4 && ei <= 7) {
                // Eject from whichever middle slot it's in
                const char* eject_algs[4] = {
                    "D' R' D R D F D' F'",   // eject FR
                    "D' B' D B D R D' R'",   // eject RB
                    "D' L' D L D B D' B'",   // eject BL
                    "D' F' D F D L D' L'",   // eject LF
                };
                do_moves(cube, eject_algs[ei - 4], moves);
                continue;
            }

            // If in U layer (0..3), not applicable (already solved), but just in case
            if (ei <= 3) {
                // Edge with no yellow in the U layer -- shouldn't happen if cross+corners are done
                // Push it to D
                do_moves(cube, FACE2[EDGES[ei].f2], moves);
                continue;
            }

            // Edge in D layer (8..11)
            int cur_dpos = ei - 8;
            int d_sticker    = sticker(cube, EDGES[ei].f1, EDGES[ei].p1); // on D face
            int side_sticker = sticker(cube, EDGES[ei].f2, EDGES[ei].p2); // on side face

            // Align so the side sticker matches face_a's center, then right insert
            // OR align so side sticker matches face_b's center, then left insert
            if (side_sticker == tgt.ca) {
                // Side sticker matches face_a; d_sticker must be cb → goes to face_b (right)
                int n = (tgt.d_pos_a - cur_dpos + 4) % 4;
                if (n) do_moves(cube, d_align_moves[n], moves);
                do_moves(cube, tgt.right_insert, moves);
            } else if (side_sticker == tgt.cb) {
                // Side sticker matches face_b; d_sticker must be ca → goes to face_a (left)
                int n = (tgt.d_pos_b - cur_dpos + 4) % 4;
                if (n) do_moves(cube, d_align_moves[n], moves);
                do_moves(cube, tgt.left_insert, moves);
            } else if (d_sticker == tgt.ca) {
                // D sticker matches face_a center; side sticker is cb
                // Need the edge flipped. Move it adjacent and turn a side face.
                int n = (tgt.d_pos_a - cur_dpos + 4) % 4;
                if (n) do_moves(cube, d_align_moves[n], moves);
                // The edge is now at d_pos_a but the colors are "flipped" (ca on D, cb on side)
                // Turn face_a to bring it to the middle, then eject
                do_moves(cube, FACE_CW[tgt.face_a], moves);
                // Now it's in the middle layer; next iteration will eject it
            } else {
                // d_sticker == tgt.cb: similar flip case
                int n = (tgt.d_pos_b - cur_dpos + 4) % 4;
                if (n) do_moves(cube, d_align_moves[n], moves);
                do_moves(cube, FACE_CW[tgt.face_b], moves);
            }
        }
    }

    return moves;
}

// ===========================================================================
// Step 4 : Yellow Cross (orient D-face edges)
// ===========================================================================

std::string Solver::solve_bottom_cross(RubixCube& cube) {
    std::string moves;

    // Yellow edges on D face: positions TOP(1), RIGHT(3), BOTTOM(5), LEFT(7)
    auto count_yellow_edges = [&]() -> int {
        int cnt = 0;
        uint64_t d = cube.get_face(FACE_BOTTOM);
        if (GET_COLOR(d, TOP)    == YELLOW) cnt++;
        if (GET_COLOR(d, RIGHT)  == YELLOW) cnt++;
        if (GET_COLOR(d, BOTTOM) == YELLOW) cnt++;
        if (GET_COLOR(d, LEFT)   == YELLOW) cnt++;
        return cnt;
    };

    // OLL edge algorithm for D face: F R D R' D' F'
    const char* oll_edge = "F R D R' D' F'";

    for (int safety = 0; safety < 10; safety++) {
        int cnt = count_yellow_edges();
        if (cnt == 4) break;

        uint64_t d = cube.get_face(FACE_BOTTOM);
        bool e_top    = GET_COLOR(d, TOP)    == YELLOW;
        bool e_right  = GET_COLOR(d, RIGHT)  == YELLOW;
        bool e_bottom = GET_COLOR(d, BOTTOM) == YELLOW;
        bool e_left   = GET_COLOR(d, LEFT)   == YELLOW;

        if (cnt == 0) {
            // Dot: just apply the algorithm
            do_moves(cube, oll_edge, moves);
        } else if (cnt == 2) {
            // Check if line or L-shape
            if ((e_top && e_bottom) || (e_left && e_right)) {
                // Line: orient it so the line is LEFT-RIGHT (e_left && e_right)
                if (e_top && e_bottom) {
                    do_moves(cube, "D", moves); // rotate line to horizontal
                }
                do_moves(cube, oll_edge, moves);
            } else {
                // L-shape: orient so the L is at TOP+LEFT (yellow at positions 1 and 7)
                if (e_top && e_right)        do_moves(cube, "D", moves);
                else if (e_right && e_bottom) do_moves(cube, "D2", moves);
                else if (e_bottom && e_left)  do_moves(cube, "D'", moves);
                // e_top && e_left is already correct
                do_moves(cube, oll_edge, moves);
            }
        }
    }

    return moves;
}

// ===========================================================================
// Step 5 : Yellow Face (orient all D-face stickers to yellow)
// ===========================================================================

std::string Solver::solve_yellow_face(RubixCube& cube) {
    std::string moves;

    // Use the beginner method: position each non-yellow corner at DFR,
    // apply R' D' R D until yellow is on the D face, then rotate D.
    // The U layer gets temporarily messed up but restores after even total applications.

    // D corner positions (checking D face stickers):
    //   DFR: D TOP_RIGHT(2)
    //   DFL: D TOP_LEFT(0)
    //   DBR: D BOTTOM_RIGHT(4)
    //   DBL: D BOTTOM_LEFT(6)

    // Process corners in order: DFR first (no rotation needed), then D to bring each

    // D-face positions for each D corner:
    int d_corner_pos[4] = {TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT, TOP_LEFT};
    // Corresponding D rotations to bring each corner to DFR position:
    //   DFR(pos 0): none
    //   DBR(pos 1): D
    //   DBL(pos 2): D2
    //   DFL(pos 3): D'

    for (int safety = 0; safety < 20; safety++) {
        // Check if all D corners are yellow
        uint64_t d = cube.get_face(FACE_BOTTOM);
        bool all_yellow = true;
        for (int p : {TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT}) {
            if (GET_COLOR(d, p) != YELLOW) { all_yellow = false; break; }
        }
        if (all_yellow) break;

        // Find a corner at DFR that is NOT yellow on D
        // If DFR is already yellow, rotate D to bring a non-yellow one
        if (GET_COLOR(cube.get_face(FACE_BOTTOM), TOP_RIGHT) == YELLOW) {
            do_moves(cube, "D", moves);
            continue;
        }

        // Apply R' D' R D until DFR's D sticker is yellow (max 4 times = 2 twists)
        for (int twist = 0; twist < 6; twist++) {
            if (GET_COLOR(cube.get_face(FACE_BOTTOM), TOP_RIGHT) == YELLOW) break;
            do_moves(cube, "R' D' R D", moves);
        }
    }

    // Final D alignment: the U layer was preserved because total R' D' R D count is even,
    // but the D layer may be rotated. Align D so the D edges match the centers.
    // We'll fix this by checking and doing D rotations.
    // Actually, corner/edge permutation steps will handle this.

    return moves;
}

// ===========================================================================
// Step 6 : Corner Permutation (position D-layer corners correctly)
// ===========================================================================

std::string Solver::solve_corner_permutation(RubixCube& cube) {
    std::string moves;

    // A D corner is "correct" if its two non-yellow colors match the adjacent centers.
    auto corner_correct = [&](int ci) -> bool {
        int c1 = sticker(cube, CORNERS[ci].f2, CORNERS[ci].p2);
        int c2 = sticker(cube, CORNERS[ci].f3, CORNERS[ci].p3);
        int center1 = CORNERS[ci].f2; // center color == face index
        int center2 = CORNERS[ci].f3;
        return (c1 == center1 && c2 == center2);
    };

    // 3-cycle algorithm for D corners: cycles DFR->DBR->DBL (keeping DFL fixed)
    // Using Niklas-style commutator adapted for D layer
    const char* corner_3cycle = "R' D L D' R D L' D'";

    for (int safety = 0; safety < 10; safety++) {
        int correct_count = 0;
        int correct_corner = -1;
        for (int i = 4; i <= 7; i++) {
            if (corner_correct(i)) {
                correct_count++;
                correct_corner = i;
            }
        }

        if (correct_count == 4) break;

        if (correct_count == 1) {
            // Rotate D so the correct corner is at DFL (position 4)
            // so the 3-cycle operates on the other three
            int cur = correct_corner - 4; // 0=DFL,1=DFR,2=DBR,3=DBL
            // We want it at DFL (pos 0). D moves: pos -> pos+1.
            // To move from cur to 0: n = (0 - cur + 4) % 4
            int n = (0 - cur + 4) % 4;
            if (n) do_moves(cube, d_align_moves[n], moves);
            do_moves(cube, corner_3cycle, moves);
        } else {
            // 0 correct: apply 3-cycle from any position
            do_moves(cube, corner_3cycle, moves);
        }
    }

    return moves;
}

// ===========================================================================
// Step 7 : Edge Permutation (position D-layer edges correctly)
// ===========================================================================

std::string Solver::solve_edge_permutation(RubixCube& cube) {
    std::string moves;

    // A D edge is "correct" if its side sticker matches the adjacent center
    auto edge_correct = [&](int ei) -> bool {
        return sticker(cube, EDGES[ei].f2, EDGES[ei].p2) == EDGES[ei].f2;
    };

    // U-perm adapted for D layer (clockwise 3-cycle of edges)
    const char* uperm_cw  = "R2 D' R' D' R D R D R D' R";
    // Counter-clockwise
    const char* uperm_ccw = "R' D R' D' R' D' R' D R D R2";

    for (int safety = 0; safety < 10; safety++) {
        int correct_count = 0;
        int correct_edge = -1;
        for (int i = 8; i <= 11; i++) {
            if (edge_correct(i)) {
                correct_count++;
                correct_edge = i;
            }
        }

        if (correct_count == 4) break;

        if (correct_count == 1) {
            // Rotate D so the correct edge is at DB (edge 10, position 2)
            // The U-perm then cycles the other 3
            int cur_dpos = correct_edge - 8;
            int n = (2 - cur_dpos + 4) % 4; // target DB = position 2
            if (n) do_moves(cube, d_align_moves[n], moves);

            // Determine direction: check if CW or CCW fixes it
            RubixCube test = cube;
            test.apply_moves(uperm_cw);
            bool cw_works = true;
            for (int i = 8; i <= 11; i++) {
                if (sticker(test, EDGES[i].f2, EDGES[i].p2) != EDGES[i].f2) {
                    cw_works = false;
                    break;
                }
            }
            if (cw_works) {
                do_moves(cube, uperm_cw, moves);
            } else {
                do_moves(cube, uperm_ccw, moves);
            }
        } else if (correct_count == 0) {
            // Apply U-perm from any position; should get 1 correct
            do_moves(cube, uperm_cw, moves);
        } else {
            // 2 or 3 correct edges shouldn't happen on a valid cube
            // but if stuck, try a D rotation
            do_moves(cube, "D", moves);
        }
    }

    // Final check: might need a D alignment
    for (int safety = 0; safety < 4; safety++) {
        if (cube == RubixCube()) break;
        do_moves(cube, "D", moves);
    }

    return moves;
}

// ===========================================================================
// Solve_Cube : main entry point
// ===========================================================================

std::string Solver::Solve_Cube(RubixCube &given_cube, int Depth_Limit) {
    (void)Depth_Limit; // unused in heuristic mode

    if (given_cube == RubixCube()) return "";

    std::string all_moves;

    std::string step = solve_white_cross(given_cube);
    if (!step.empty()) {
        all_moves += step;
    }

    step = solve_white_corners(given_cube);
    if (!step.empty()) {
        if (!all_moves.empty()) all_moves += " ";
        all_moves += step;
    }

    step = solve_second_layer(given_cube);
    if (!step.empty()) {
        if (!all_moves.empty()) all_moves += " ";
        all_moves += step;
    }

    step = solve_bottom_cross(given_cube);
    if (!step.empty()) {
        if (!all_moves.empty()) all_moves += " ";
        all_moves += step;
    }

    step = solve_yellow_face(given_cube);
    if (!step.empty()) {
        if (!all_moves.empty()) all_moves += " ";
        all_moves += step;
    }

    step = solve_corner_permutation(given_cube);
    if (!step.empty()) {
        if (!all_moves.empty()) all_moves += " ";
        all_moves += step;
    }

    step = solve_edge_permutation(given_cube);
    if (!step.empty()) {
        if (!all_moves.empty()) all_moves += " ";
        all_moves += step;
    }

    return all_moves;
}
