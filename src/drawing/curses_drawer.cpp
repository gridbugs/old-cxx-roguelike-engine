#include "drawing/curses_drawer.hpp"
#include <ncurses.h>
#include "io/curses.hpp"
#include <vector>
#include "util/arith.hpp"
#include <assert.h>

#define STEPS 32

typedef enum {
    COL_WHITE = 16,
    COL_BLACK,
    COL_DARK_BLUE,
    COL_DARK_GREY,
    COL_LIGHT_GREY
} col_t;

typedef enum {
    PAIR_WALL = 16,
    PAIR_FLOOR,
    PAIR_UNKNOWN,
    PAIR_WALL_REMEMBERED,
    PAIR_FLOOR_REMEMBERED
} pair_t;

static void add_char(char ch, int pair) {
    wattron(curses::game_window, COLOR_PAIR(pair));
    waddch(curses::game_window, ch);
    wattroff(curses::game_window, COLOR_PAIR(pair));
}

curses_drawer::curses_drawer() :
    floor_range(STEPS, curses_col(600, 300, 200), curses_col(0, 0, 0)),
    wall_range(STEPS, curses_col(900, 900, 900), curses_col(0, 0, 0))
{
    init_color(COL_WHITE, 1000, 1000, 1000);
    init_color(COL_BLACK, 0, 0, 0);
    init_color(COL_DARK_BLUE, 0, 0, 200);
    init_color(COL_DARK_GREY, 250, 250, 250);
    init_color(COL_LIGHT_GREY, 750, 750, 750);

    init_pair(PAIR_WALL, COL_BLACK, COL_WHITE);
    init_pair(PAIR_FLOOR, COL_WHITE, COL_DARK_BLUE);
    init_pair(PAIR_UNKNOWN, COL_BLACK, COL_BLACK);
    init_pair(PAIR_WALL_REMEMBERED, COL_DARK_GREY, COL_LIGHT_GREY);
    init_pair(PAIR_FLOOR_REMEMBERED, COL_LIGHT_GREY, COL_DARK_GREY);

    col_table.map_range(64, floor_range);
    col_table.map_range(128, wall_range);

    for (int i = 64; i < 256; ++i) {
        init_pair(i, COL_WHITE, i);
    }
}

void curses_drawer::draw_cell(game_cell &c) {
    wmove(curses::game_window, c.y_coord, c.x_coord);
    if (c.is_opaque()) {
        wattron(curses::game_window, COLOR_PAIR(PAIR_WALL));
        waddch(curses::game_window, '#');
        wattroff(curses::game_window, COLOR_PAIR(PAIR_WALL));
    } else {
        wattron(curses::game_window, COLOR_PAIR(PAIR_FLOOR));
        waddch(curses::game_window, '.');
        wattroff(curses::game_window, COLOR_PAIR(PAIR_FLOOR));
    }
}

void curses_drawer::draw_cell(game_cell &c, knowledge_cell &k, character &ch) {
    wmove(curses::game_window, c.y_coord, c.x_coord);
  
    char fg = ' ';
    if (ch.position == c.coord) {
        fg = '@';
        add_char(fg, 64);
    } else if (k.is_unknown()) {
        add_char(fg, PAIR_UNKNOWN);
    } else if (k.is_visible()) {
        
        vec2<int> delta = ch.position - c.coord;
        //int dist = std::max(abs(delta.x), abs(delta.y));
        int dist = static_cast<int>(delta.length());

        int col_idx = arithmetic::constrain(0, dist, STEPS-1);
        if (c.is_opaque()) {
            add_char(fg, 128 + col_idx);
        } else {
            add_char(fg, 64 + col_idx);
        }
    } else {
        if (c.is_opaque()) {
            add_char(fg, PAIR_UNKNOWN);
        } else {
            add_char(fg, PAIR_UNKNOWN);
        }
    }  
}

void curses_drawer::draw_cell(game_cell &c, knowledge_cell &k) {

    wmove(curses::game_window, c.y_coord, c.x_coord);

    if (k.is_unknown()) {
        add_char(' ', PAIR_UNKNOWN);
    } else if (k.is_visible()) {
        if (c.is_opaque()) {
            add_char('#', PAIR_WALL);
        } else {
            add_char('.', PAIR_FLOOR);
        }
    } else {
        if (c.is_opaque()) {
            add_char('#', PAIR_WALL_REMEMBERED);
        } else {
            add_char('.', PAIR_FLOOR_REMEMBERED);
        }
    }


}

void curses_drawer::draw_world(world &w, behaviour &b) {
    drawer::draw_world(w, b);
    wrefresh(curses::game_window);
}

void curses_drawer::draw_world(world &w) {
    drawer::draw_world(w);
    wrefresh(curses::game_window);
}

void curses_drawer::draw_world(world &w, behaviour &b, character &ch) {
    grid<game_cell> &g = w.map;
    const grid<knowledge_cell> &k = b.get_knowledge_grid();
    for (int i = 0; i < g.height; ++i) {
        for (int j = 0; j < g.width; ++j) {
            draw_cell(g[i][j], k[i][j], ch);
        }
    }
}

void curses_drawer::test() {
    int base = 8;
    for (int i = 0; i < 64; ++i) {
        init_pair(base + i, 128 + i, 64 + i);
    }
    
    for (int i = 0; i < 64; ++i) {
        wmove(curses::game_window, 0, i);
        wattron(curses::game_window, COLOR_PAIR(base + i));
        waddch(curses::game_window, '#');
        wattroff(curses::game_window, COLOR_PAIR(base + i));
    }

    init_pair(base, 128+63, 64+63);
    wmove(curses::game_window, 1, 0);
    wattron(curses::game_window, COLOR_PAIR(base));
    waddch(curses::game_window, '#');
    wattroff(curses::game_window, COLOR_PAIR(base));

    wrefresh(curses::game_window);
}
