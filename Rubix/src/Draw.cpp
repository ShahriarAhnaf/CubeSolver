#include "Draw.h"
#include <ncurses.h>

// faces 0-5, {x,y} of each face's centre in the net
static const int8_t face_center[6][2] = {{18,1},{9,5},{18,5},{27,5},{36,5},{18,10}};
// sticker position 0-8 -> {x,y} offset from the face centre
static const int8_t sticker_offset[9][2] = {{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{0,0}};
static const char color_char[] = "WBRGOY";

void draw_cube(const RubixCube& cube, int8_t x, int8_t y) {
    for (int f = 0; f < 6; f++) {
        for (int p = 0; p < 9; p++) {
            uint64_t color = p == 8 ? f : GET_COLOR(cube.get_face(f), p);
            mvaddch(y + face_center[f][1] + sticker_offset[p][1],
                    x + face_center[f][0] + sticker_offset[p][0],
                    color < 6 ? color_char[color] : 'X');
        }
    }
}
